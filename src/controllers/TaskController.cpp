#include "controllers/TaskController.h"
#include <algorithm>
#include <iostream>

namespace checklist {

TaskController::TaskController(std::shared_ptr<JsonStorage> storage)
    : storage_(std::move(storage)),
      isLoaded_(false),
      autoSave_(true),
      autoSaveDelay_(500),
      presentationMode_(false),
      presentationTaskIndex_(0),
      syncManager_(std::make_shared<SyncManager>()) {
    storage_->setErrorCallback([this](const std::string& msg) {
        setError(msg);
    });

    // Conectar callbacks de sync
    syncManager_->setStatusChangedCallback([this](SyncStatus status) {
        // Aquí se podría emitir una señal para QML
    });
}

TaskController::TaskController()
    : TaskController(std::make_shared<JsonStorage>()) {}

TaskController::TaskController(const std::string& storagePath)
    : TaskController(std::make_shared<JsonStorage>(storagePath)) {}

TaskController::~TaskController() {
    // Cancel any pending auto-save
    std::lock_guard<std::mutex> lock(autoSaveLock_);
}

bool TaskController::canUndo() const {
    return !undoStack_.empty();
}

bool TaskController::canRedo() const {
    return !redoStack_.empty();
}

void TaskController::undo() {
    if (undoStack_.empty()) return;

    auto cmd = undoStack_.top();
    undoStack_.pop();
    cmd->undo();
    redoStack_.push(cmd);

    if (undoStackChangedCb_) undoStackChangedCb_();
    if (redoStackChangedCb_) redoStackChangedCb_();
    if (tasksChangedCb_) tasksChangedCb_();
    scheduleAutoSave();
}

void TaskController::redo() {
    if (redoStack_.empty()) return;

    auto cmd = redoStack_.top();
    redoStack_.pop();
    cmd->redo();
    undoStack_.push(cmd);

    if (undoStackChangedCb_) undoStackChangedCb_();
    if (redoStackChangedCb_) redoStackChangedCb_();
    if (tasksChangedCb_) tasksChangedCb_();
    scheduleAutoSave();
}

void TaskController::clearHistory() {
    while (!undoStack_.empty()) undoStack_.pop();
    while (!redoStack_.empty()) redoStack_.pop();

    if (undoStackChangedCb_) undoStackChangedCb_();
    if (redoStackChangedCb_) redoStackChangedCb_();
}

void TaskController::load() {
    std::lock_guard<std::mutex> lock(autoSaveLock_);

    auto loaded = storage_->load();
    tasks_.clear();
    tasks_ = loaded;

    if (!isLoaded_) {
        isLoaded_ = true;
        if (loadedChangedCb_) loadedChangedCb_();
    }

    if (tasksChangedCb_) tasksChangedCb_();
}

bool TaskController::save() {
    if (storage_->save(tasks_)) {
        if (savedCb_) savedCb_();
        return true;
    }
    return false;
}

std::shared_ptr<Task> TaskController::addTask(const std::string& name, const std::string& category) {
    std::string trimmed = name;
    // Trim
    size_t start = trimmed.find_first_not_of(" \t\r\n");
    size_t end = trimmed.find_last_not_of(" \t\r\n");
    if (start == std::string::npos) trimmed = "";
    else trimmed = trimmed.substr(start, end - start + 1);

    std::string trimmedCategory = category;
    start = trimmedCategory.find_first_not_of(" \t\r\n");
    end = trimmedCategory.find_last_not_of(" \t\r\n");
    if (start == std::string::npos) trimmedCategory = "";
    else trimmedCategory = trimmedCategory.substr(start, end - start + 1);

    if (trimmed.empty()) {
        setError("Task name cannot be empty");
        return nullptr;
    }

    auto task = std::make_shared<Task>(trimmed, Date::today());
    task->setCategory(trimmedCategory);
    int newIndex = static_cast<int>(tasks_.size());
    tasks_.push_back(task);

    if (tasksChangedCb_) tasksChangedCb_();
    if (taskAddedCb_) taskAddedCb_(newIndex);
    scheduleAutoSave();

    pushUndo(std::make_shared<UndoCommand>(
        "Add task: " + trimmed,
        [this, newIndex, task]() {
            if (newIndex >= 0 && newIndex < static_cast<int>(tasks_.size())) {
                tasks_.erase(tasks_.begin() + newIndex);
                if (tasksChangedCb_) tasksChangedCb_();
            }
        },
        [this, newIndex, task, trimmed, trimmedCategory]() {
            auto t = std::make_shared<Task>(trimmed, Date::today());
            t->setCategory(trimmedCategory);
            if (newIndex >= static_cast<int>(tasks_.size())) {
                tasks_.push_back(t);
            } else {
                tasks_.insert(tasks_.begin() + newIndex, t);
            }
            if (tasksChangedCb_) tasksChangedCb_();
            if (taskAddedCb_) taskAddedCb_(newIndex);
        }
    ));

    return task;
}

void TaskController::removeTask(int index) {
    if (index < 0 || index >= static_cast<int>(tasks_.size())) return;

    auto task = tasks_[index];
    std::string taskName = task->name();
    int capturedIndex = index;

    tasks_.erase(tasks_.begin() + index);
    if (tasksChangedCb_) tasksChangedCb_();
    if (taskRemovedCb_) taskRemovedCb_(index);
    scheduleAutoSave();

    pushUndo(std::make_shared<UndoCommand>(
        "Delete task: " + taskName,
        [this, capturedIndex, task]() {
            if (capturedIndex >= static_cast<int>(tasks_.size())) {
                tasks_.push_back(task);
            } else {
                tasks_.insert(tasks_.begin() + capturedIndex, task);
            }
            if (tasksChangedCb_) tasksChangedCb_();
            if (taskAddedCb_) taskAddedCb_(capturedIndex);
        },
        [this, capturedIndex]() {
            auto it = std::find_if(tasks_.begin(), tasks_.end(),
                [&](const std::shared_ptr<Task>& t) { return t->id() == tasks_[capturedIndex]->id(); });
            // Re-find by index since vector changed
            if (capturedIndex >= 0 && capturedIndex < static_cast<int>(tasks_.size())) {
                tasks_.erase(tasks_.begin() + capturedIndex);
                if (tasksChangedCb_) tasksChangedCb_();
                if (taskRemovedCb_) taskRemovedCb_(capturedIndex);
            }
        }
    ));
}

std::shared_ptr<Task> TaskController::getTask(int index) const {
    if (index < 0 || index >= static_cast<int>(tasks_.size())) return nullptr;
    return tasks_[index];
}

void TaskController::addItemToTask(int taskIndex, const std::string& text) {
    auto task = getTask(taskIndex);
    std::string trimmed = text;
    size_t start = trimmed.find_first_not_of(" \t\r\n");
    size_t end = trimmed.find_last_not_of(" \t\r\n");
    if (start == std::string::npos) trimmed = "";
    else trimmed = trimmed.substr(start, end - start + 1);

    if (!task || trimmed.empty()) return;

    task->addItem(std::make_shared<ChecklistItem>(trimmed, false));
    int itemIndex = static_cast<int>(task->itemCount()) - 1;
    scheduleAutoSave();

    pushUndo(std::make_shared<UndoCommand>(
        "Add item",
        [this, taskIndex, itemIndex]() {
            auto t = getTask(taskIndex);
            if (t && itemIndex < static_cast<int>(t->itemCount())) {
                t->removeItem(static_cast<size_t>(itemIndex));
                if (tasksChangedCb_) tasksChangedCb_();
            }
        },
        [this, taskIndex, trimmed]() {
            auto t = getTask(taskIndex);
            if (t) {
                t->addItem(std::make_shared<ChecklistItem>(trimmed, false));
                if (tasksChangedCb_) tasksChangedCb_();
            }
        }
    ));
}

void TaskController::removeItemFromTask(int taskIndex, int itemIndex) {
    auto task = getTask(taskIndex);
    if (!task) return;

    auto item = task->getItem(static_cast<size_t>(itemIndex));
    if (!item) return;

    std::string itemText = item->text();
    bool itemDone = item->done();

    task->removeItem(static_cast<size_t>(itemIndex));
    scheduleAutoSave();

    pushUndo(std::make_shared<UndoCommand>(
        "Delete item: " + itemText,
        [this, taskIndex, itemText, itemDone]() {
            auto t = getTask(taskIndex);
            if (t) {
                t->addItem(std::make_shared<ChecklistItem>(itemText, itemDone));
                if (tasksChangedCb_) tasksChangedCb_();
            }
        },
        [this, taskIndex, itemIndex]() {
            auto t = getTask(taskIndex);
            if (t && itemIndex < static_cast<int>(t->itemCount())) {
                t->removeItem(static_cast<size_t>(itemIndex));
                if (tasksChangedCb_) tasksChangedCb_();
            }
        }
    ));
}

void TaskController::toggleItemInTask(int taskIndex, int itemIndex, bool done) {
    auto task = getTask(taskIndex);
    if (!task) return;

    auto item = task->getItem(static_cast<size_t>(itemIndex));
    if (!item) return;

    // Verificar si la tarea está bloqueada por una dependencia
    if (done && isTaskBlockedByDependency(taskIndex)) {
        setError("Task is blocked by an unfinished dependency");
        return;
    }

    bool previousDone = item->done();
    item->setDone(done);
    scheduleAutoSave();

    pushUndo(std::make_shared<UndoCommand>(
        done ? "Check item" : "Uncheck item",
        [this, taskIndex, itemIndex, previousDone]() {
            auto t = getTask(taskIndex);
            if (t) {
                auto i = t->getItem(static_cast<size_t>(itemIndex));
                if (i) i->setDone(previousDone);
            }
        },
        [this, taskIndex, itemIndex, done]() {
            auto t = getTask(taskIndex);
            if (t) {
                auto i = t->getItem(static_cast<size_t>(itemIndex));
                if (i) i->setDone(done);
            }
        }
    ));
}

void TaskController::setItemTextInTask(int taskIndex, int itemIndex, const std::string& text) {
    auto task = getTask(taskIndex);
    if (!task) return;

    auto item = task->getItem(static_cast<size_t>(itemIndex));
    if (!item) return;

    std::string newText = text;
    size_t start = newText.find_first_not_of(" \t\r\n");
    size_t end = newText.find_last_not_of(" \t\r\n");
    if (start == std::string::npos) newText = "";
    else newText = newText.substr(start, end - start + 1);

    std::string oldText = item->text();
    if (oldText == newText || newText.empty()) return;

    item->setText(newText);
    scheduleAutoSave();

    pushUndo(std::make_shared<UndoCommand>(
        "Edit item",
        [this, taskIndex, itemIndex, oldText]() {
            auto t = getTask(taskIndex);
            if (t) {
                auto i = t->getItem(static_cast<size_t>(itemIndex));
                if (i) i->setText(oldText);
            }
        },
        [this, taskIndex, itemIndex, newText]() {
            auto t = getTask(taskIndex);
            if (t) {
                auto i = t->getItem(static_cast<size_t>(itemIndex));
                if (i) i->setText(newText);
            }
        }
    ));
}

void TaskController::setItemTextInCorrection(int taskIndex, int correctionIndex, int itemIndex, const std::string& text) {
    auto task = getTask(taskIndex);
    if (!task) return;

    auto corr = task->getCorrection(static_cast<size_t>(correctionIndex));
    if (!corr) return;

    auto item = corr->getItem(static_cast<size_t>(itemIndex));
    if (!item) return;

    std::string newText = text;
    size_t start = newText.find_first_not_of(" \t\r\n");
    size_t end = newText.find_last_not_of(" \t\r\n");
    if (start == std::string::npos) newText = "";
    else newText = newText.substr(start, end - start + 1);

    std::string oldText = item->text();
    if (oldText == newText || newText.empty()) return;

    item->setText(newText);
    scheduleAutoSave();

    pushUndo(std::make_shared<UndoCommand>(
        "Edit correction item",
        [this, taskIndex, correctionIndex, itemIndex, oldText]() {
            auto t = getTask(taskIndex);
            if (t) {
                auto c = t->getCorrection(static_cast<size_t>(correctionIndex));
                if (c) {
                    auto i = c->getItem(static_cast<size_t>(itemIndex));
                    if (i) i->setText(oldText);
                }
            }
        },
        [this, taskIndex, correctionIndex, itemIndex, newText]() {
            auto t = getTask(taskIndex);
            if (t) {
                auto c = t->getCorrection(static_cast<size_t>(correctionIndex));
                if (c) {
                    auto i = c->getItem(static_cast<size_t>(itemIndex));
                    if (i) i->setText(newText);
                }
            }
        }
    ));
}

std::shared_ptr<Correction> TaskController::addCorrectionToTask(int taskIndex, const std::string& name) {
    auto task = getTask(taskIndex);
    std::string trimmed = name;
    size_t start = trimmed.find_first_not_of(" \t\r\n");
    size_t end = trimmed.find_last_not_of(" \t\r\n");
    if (start == std::string::npos) trimmed = "";
    else trimmed = trimmed.substr(start, end - start + 1);

    if (!task || trimmed.empty()) return nullptr;

    auto corr = task->addCorrection(std::make_shared<Correction>(trimmed, Date::today()));
    int corrIndex = static_cast<int>(task->correctionCount()) - 1;
    scheduleAutoSave();

    pushUndo(std::make_shared<UndoCommand>(
        "Add correction: " + trimmed,
        [this, taskIndex, corrIndex]() {
            auto t = getTask(taskIndex);
            if (t && corrIndex < static_cast<int>(t->correctionCount())) {
                t->removeCorrection(static_cast<size_t>(corrIndex));
                if (tasksChangedCb_) tasksChangedCb_();
            }
        },
        [this, taskIndex, trimmed]() {
            auto t = getTask(taskIndex);
            if (t) {
                t->addCorrection(std::make_shared<Correction>(trimmed, Date::today()));
                if (tasksChangedCb_) tasksChangedCb_();
            }
        }
    ));

    return corr;
}

void TaskController::removeCorrectionFromTask(int taskIndex, int correctionIndex) {
    auto task = getTask(taskIndex);
    if (!task) return;

    auto corr = task->getCorrection(static_cast<size_t>(correctionIndex));
    if (!corr) return;

    std::string corrName = corr->name();
    task->removeCorrection(static_cast<size_t>(correctionIndex));
    scheduleAutoSave();

    pushUndo(std::make_shared<UndoCommand>(
        "Delete correction: " + corrName,
        [this, taskIndex, corrName]() {
            auto t = getTask(taskIndex);
            if (t) {
                t->addCorrection(std::make_shared<Correction>(corrName, Date::today()));
                if (tasksChangedCb_) tasksChangedCb_();
            }
        },
        [this, taskIndex, correctionIndex]() {
            auto t = getTask(taskIndex);
            if (t && correctionIndex < static_cast<int>(t->correctionCount())) {
                t->removeCorrection(static_cast<size_t>(correctionIndex));
                if (tasksChangedCb_) tasksChangedCb_();
            }
        }
    ));
}

void TaskController::setTaskName(int taskIndex, const std::string& name) {
    auto task = getTask(taskIndex);
    std::string trimmed = name;
    size_t start = trimmed.find_first_not_of(" \t\r\n");
    size_t end = trimmed.find_last_not_of(" \t\r\n");
    if (start == std::string::npos) trimmed = "";
    else trimmed = trimmed.substr(start, end - start + 1);

    if (!task || trimmed.empty()) return;

    std::string oldName = task->name();
    task->setName(trimmed);
    scheduleAutoSave();

    pushUndo(std::make_shared<UndoCommand>(
        "Rename task",
        [this, taskIndex, oldName]() {
            auto t = getTask(taskIndex);
            if (t) t->setName(oldName);
        },
        [this, taskIndex, trimmed]() {
            auto t = getTask(taskIndex);
            if (t) t->setName(trimmed);
        }
    ));
}

void TaskController::setCorrectionName(int taskIndex, int correctionIndex, const std::string& name) {
    auto task = getTask(taskIndex);
    std::string trimmed = name;
    size_t start = trimmed.find_first_not_of(" \t\r\n");
    size_t end = trimmed.find_last_not_of(" \t\r\n");
    if (start == std::string::npos) trimmed = "";
    else trimmed = trimmed.substr(start, end - start + 1);

    if (!task || trimmed.empty()) return;

    auto corr = task->getCorrection(static_cast<size_t>(correctionIndex));
    if (!corr) return;

    std::string oldName = corr->name();
    corr->setName(trimmed);
    scheduleAutoSave();

    pushUndo(std::make_shared<UndoCommand>(
        "Rename correction",
        [this, taskIndex, correctionIndex, oldName]() {
            auto t = getTask(taskIndex);
            if (t) {
                auto c = t->getCorrection(static_cast<size_t>(correctionIndex));
                if (c) c->setName(oldName);
            }
        },
        [this, taskIndex, correctionIndex, trimmed]() {
            auto t = getTask(taskIndex);
            if (t) {
                auto c = t->getCorrection(static_cast<size_t>(correctionIndex));
                if (c) c->setName(trimmed);
            }
        }
    ));
}

void TaskController::setTaskCategory(int taskIndex, const std::string& category) {
    auto task = getTask(taskIndex);
    std::string trimmed = category;
    size_t start = trimmed.find_first_not_of(" \t\r\n");
    size_t end = trimmed.find_last_not_of(" \t\r\n");
    if (start == std::string::npos) trimmed = "";
    else trimmed = trimmed.substr(start, end - start + 1);

    if (!task) return;

    std::string oldCategory = task->category();
    if (trimmed == oldCategory) return;

    task->setCategory(trimmed);
    if (tasksChangedCb_) tasksChangedCb_();
    scheduleAutoSave();

    pushUndo(std::make_shared<UndoCommand>(
        "Change task theme",
        [this, taskIndex, oldCategory]() {
            auto t = getTask(taskIndex);
            if (t) t->setCategory(oldCategory);
        },
        [this, taskIndex, trimmed]() {
            auto t = getTask(taskIndex);
            if (t) t->setCategory(trimmed);
        }
    ));
}

void TaskController::importFromJsonString(const std::string& jsonString) {
    std::string error;
    auto tasksJson = JsonStorage::tryParseIntoArray(jsonString, error);
    if (tasksJson.empty() && !error.empty()) {
        setError("Error parsing JSON: " + error);
        return;
    }

    int imported = 0;
    for (const auto& json : tasksJson) {
        auto task = JsonStorage::jsonToTask(json);
        if (task) {
            tasks_.push_back(task);
            imported++;
        }
    }

    if (imported > 0) {
        if (tasksChangedCb_) tasksChangedCb_();
        scheduleAutoSave();
    }
}

void TaskController::scheduleAutoSave() {
    if (!autoSave_) return;

    std::lock_guard<std::mutex> lock(autoSaveLock_);

    // In a real implementation, we'd use a timer here.
    // For simplicity, save directly.
    debounceSave();
}

void TaskController::debounceSave() {
    // Simple direct save for now
    // A full implementation would use a timer with delay
    save();
}

void TaskController::setError(const std::string& message) {
    if (lastError_ != message) {
        lastError_ = message;
        if (errorChangedCb_) errorChangedCb_();
    }
}

void TaskController::pushUndo(std::shared_ptr<UndoCommand> cmd) {
    undoStack_.push(cmd);
    while (!redoStack_.empty()) redoStack_.pop();

    if (undoStackChangedCb_) undoStackChangedCb_();
    if (redoStackChangedCb_) redoStackChangedCb_();
}

void TaskController::setAutoSave(bool value) {
    if (autoSave_ != value) {
        autoSave_ = value;
        if (autoSaveChangedCb_) autoSaveChangedCb_();
        if (!autoSave_) {
            std::lock_guard<std::mutex> lock(autoSaveLock_);
        }
    }
}

void TaskController::setAutoSaveDelay(int ms) {
    if (autoSaveDelay_ != ms) {
        autoSaveDelay_ = ms;
        if (autoSaveDelayChangedCb_) autoSaveDelayChangedCb_();
    }
}

// --- Tiempo y Pomodoro ---

void TaskController::setTaskEstimatedDuration(int taskIndex, int64_t seconds) {
    auto task = getTask(taskIndex);
    if (!task) return;

    task->setEstimatedDurationSeconds(seconds);
    scheduleAutoSave();

    if (taskDurationChangedCb_) taskDurationChangedCb_(taskIndex);
}

int64_t TaskController::getTaskEstimatedDuration(int taskIndex) const {
    auto task = getTask(taskIndex);
    if (!task) return 0;
    return task->estimatedDurationSeconds();
}

int64_t TaskController::getTaskActualDuration(int taskIndex) const {
    auto task = getTask(taskIndex);
    if (!task) return 0;
    return task->actualDurationSeconds();
}

void TaskController::startTaskTiming(int taskIndex) {
    auto task = getTask(taskIndex);
    if (!task) return;

    task->startTiming();
    scheduleAutoSave();

    if (taskTimingStateChangedCb_) taskTimingStateChangedCb_(taskIndex);
}

void TaskController::stopTaskTiming(int taskIndex) {
    auto task = getTask(taskIndex);
    if (!task) return;

    task->stopTiming();
    scheduleAutoSave();

    if (taskTimingStateChangedCb_) taskTimingStateChangedCb_(taskIndex);
    if (taskDurationChangedCb_) taskDurationChangedCb_(taskIndex);
}

bool TaskController::isTaskTiming(int taskIndex) const {
    auto task = getTask(taskIndex);
    if (!task) return false;
    return task->isTiming();
}

void TaskController::incrementTaskPomodoro(int taskIndex) {
    auto task = getTask(taskIndex);
    if (!task) return;

    task->incrementPomodoro();
    scheduleAutoSave();

    if (taskPomodoroChangedCb_) taskPomodoroChangedCb_(taskIndex);
}

void TaskController::resetTaskPomodoros(int taskIndex) {
    auto task = getTask(taskIndex);
    if (!task) return;

    task->resetPomodoros();
    scheduleAutoSave();

    if (taskPomodoroChangedCb_) taskPomodoroChangedCb_(taskIndex);
}

int TaskController::getTaskPomodoroCount(int taskIndex) const {
    auto task = getTask(taskIndex);
    if (!task) return 0;
    return task->pomodoroCount();
}

int64_t TaskController::getTaskRemainingTime(int taskIndex) const {
    auto task = getTask(taskIndex);
    if (!task) return -1;
    return task->remainingTimeSeconds();
}

int TaskController::getTaskProgressPercentage(int taskIndex) const {
    auto task = getTask(taskIndex);
    if (!task) return 0;
    return task->progressPercentage();
}

// --- Dependencias entre tareas ---

void TaskController::setTaskDependency(int taskIndex, TaskDependency type, const std::string& dependsOnId) {
    auto task = getTask(taskIndex);
    if (!task) return;

    // Validar que no se cree una dependencia circular (simplificado: no permitir depender de uno mismo)
    if (dependsOnId == task->id()) {
        setError("Cannot create dependency on self");
        return;
    }

    // Validar que el ID de dependencia exista (si no es None)
    if (type != TaskDependency::None && !dependsOnId.empty()) {
        bool exists = false;
        for (const auto& t : tasks_) {
            if (t->id() == dependsOnId) {
                exists = true;
                break;
            }
        }
        if (!exists) {
            setError("Dependency task not found");
            return;
        }
    }

    task->setDependency(type, dependsOnId);
    scheduleAutoSave();

    if (taskDependencyChangedCb_) taskDependencyChangedCb_(taskIndex);
}

TaskDependency TaskController::getTaskDependencyType(int taskIndex) const {
    auto task = getTask(taskIndex);
    if (!task) return TaskDependency::None;
    return task->dependencyType();
}

const std::string& TaskController::getTaskDependencyId(int taskIndex) const {
    static const std::string emptyId;
    auto task = getTask(taskIndex);
    if (!task) return emptyId;
    return task->dependencyId();
}

void TaskController::removeTaskDependency(int taskIndex) {
    auto task = getTask(taskIndex);
    if (!task) return;

    task->setDependency(TaskDependency::None, "");
    scheduleAutoSave();

    if (taskDependencyChangedCb_) taskDependencyChangedCb_(taskIndex);
}

bool TaskController::isTaskBlockedByDependency(int taskIndex) const {
    auto task = getTask(taskIndex);
    if (!task) return false;

    if (task->dependencyType() == TaskDependency::None || task->dependencyId().empty()) {
        return false;
    }

    // Buscar la tarea dependiente
    for (const auto& depTask : tasks_) {
        if (depTask->id() == task->dependencyId()) {
            return task->isBlockedByDependency(depTask);
        }
    }

    // Tarea dependiente no encontrada
    return true;
}

int TaskController::findTaskIndexById(const std::string& id) const {
    for (size_t i = 0; i < tasks_.size(); ++i) {
        if (tasks_[i]->id() == id) {
            return static_cast<int>(i);
        }
    }
    return -1;
}

// --- Etiquetas (Tags) ---

void TaskController::addTaskTag(int taskIndex, const std::string& tag) {
    auto task = getTask(taskIndex);
    if (!task) return;

    task->addTag(tag);
    scheduleAutoSave();

    if (taskTagsChangedCb_) taskTagsChangedCb_(taskIndex);
}

void TaskController::removeTaskTag(int taskIndex, const std::string& tag) {
    auto task = getTask(taskIndex);
    if (!task) return;

    task->removeTag(tag);
    scheduleAutoSave();

    if (taskTagsChangedCb_) taskTagsChangedCb_(taskIndex);
}

void TaskController::clearTaskTags(int taskIndex) {
    auto task = getTask(taskIndex);
    if (!task) return;

    task->clearTags();
    scheduleAutoSave();

    if (taskTagsChangedCb_) taskTagsChangedCb_(taskIndex);
}

void TaskController::setTaskTags(int taskIndex, const std::vector<std::string>& tags) {
    auto task = getTask(taskIndex);
    if (!task) return;

    task->setTags(tags);
    scheduleAutoSave();

    if (taskTagsChangedCb_) taskTagsChangedCb_(taskIndex);
}

const std::vector<std::string>& TaskController::getTaskTags(int taskIndex) const {
    static const std::vector<std::string> emptyTags;
    auto task = getTask(taskIndex);
    if (!task) return emptyTags;
    return task->tags();
}

size_t TaskController::getTaskTagCount(int taskIndex) const {
    auto task = getTask(taskIndex);
    if (!task) return 0;
    return task->tagCount();
}

bool TaskController::taskHasTag(int taskIndex, const std::string& tag) const {
    auto task = getTask(taskIndex);
    if (!task) return false;
    return task->hasTag(tag);
}

std::vector<std::string> TaskController::getAllTags() const {
    std::vector<std::string> allTags;
    for (const auto& task : tasks_) {
        for (const auto& tag : task->tags()) {
            if (std::find(allTags.begin(), allTags.end(), tag) == allTags.end()) {
                allTags.push_back(tag);
            }
        }
    }
    return allTags;
}

// --- Plantillas de tareas recurrentes ---

void TaskController::addTemplate(const std::shared_ptr<TaskTemplate>& template_) {
    if (!template_) return;

    templates_.push_back(template_);
    scheduleAutoSave();

    if (templatesChangedCb_) templatesChangedCb_();
}

void TaskController::removeTemplate(const std::string& templateId) {
    auto it = std::find_if(templates_.begin(), templates_.end(),
        [&](const std::shared_ptr<TaskTemplate>& t) { return t->id() == templateId; });

    if (it != templates_.end()) {
        templates_.erase(it);
        scheduleAutoSave();

        if (templatesChangedCb_) templatesChangedCb_();
    }
}

void TaskController::updateTemplate(const std::shared_ptr<TaskTemplate>& template_) {
    if (!template_) return;

    auto it = std::find_if(templates_.begin(), templates_.end(),
        [&](const std::shared_ptr<TaskTemplate>& t) { return t->id() == template_->id(); });

    if (it != templates_.end()) {
        *it = template_;
    } else {
        templates_.push_back(template_);
    }

    scheduleAutoSave();

    if (templatesChangedCb_) templatesChangedCb_();
}

std::shared_ptr<TaskTemplate> TaskController::getTemplate(const std::string& templateId) const {
    for (const auto& t : templates_) {
        if (t->id() == templateId) {
            return t;
        }
    }
    return nullptr;
}

std::shared_ptr<TaskTemplate> TaskController::getTemplateByIndex(int index) const {
    if (index < 0 || index >= static_cast<int>(templates_.size())) return nullptr;
    return templates_[index];
}

size_t TaskController::templateCount() const {
    return templates_.size();
}

std::vector<std::shared_ptr<TaskTemplate>> TaskController::getTemplatesByRecurrence(TemplateRecurrence recurrence) const {
    std::vector<std::shared_ptr<TaskTemplate>> result;
    for (const auto& t : templates_) {
        if (t->recurrence() == recurrence) {
            result.push_back(t);
        }
    }
    return result;
}

std::shared_ptr<Task> TaskController::createTaskFromTemplate(const std::string& templateId, const std::string& taskName) {
    auto template_ = getTemplate(templateId);
    if (!template_) return nullptr;

    auto task = template_->instantiate(taskName);
    tasks_.push_back(task);

    if (tasksChangedCb_) tasksChangedCb_();
    scheduleAutoSave();

    return task;
}

std::shared_ptr<Task> TaskController::createTaskFromTemplateByIndex(int templateIndex, const std::string& taskName) {
    auto template_ = getTemplateByIndex(templateIndex);
    if (!template_) return nullptr;

    return createTaskFromTemplate(template_->id(), taskName);
}

std::vector<std::string> TaskController::getAllTemplateIds() const {
    std::vector<std::string> ids;
    for (const auto& t : templates_) {
        ids.push_back(t->id());
    }
    return ids;
}

// --- Estadísticas de productividad ---

WeeklyReport TaskController::getWeeklyReport(int days) const {
    return StatsCalculator::generateWeeklyReport(tasks_, days);
}

std::vector<DailyStat> TaskController::getDailyStats(int days) const {
    return StatsCalculator::getDailyCompletedStats(tasks_, days);
}

ProductivityStreak TaskController::getProductivityStreak() const {
    return StatsCalculator::calculateStreak(tasks_);
}

std::vector<CategoryTimeStat> TaskController::getCategoryTimeDistribution() const {
    return StatsCalculator::getCategoryTimeDistribution(tasks_);
}

// --- Modo Presentación / Kiosk ---

void TaskController::setPresentationMode(bool enabled) {
    if (presentationMode_ != enabled) {
        presentationMode_ = enabled;
        if (!enabled) {
            presentationTaskIndex_ = 0;
        }
    }
}

void TaskController::setPresentationTaskIndex(int index) {
    if (index < 0 || index >= static_cast<int>(tasks_.size())) return;
    presentationTaskIndex_ = index;
}

void TaskController::nextPresentationTask() {
    if (!presentationMode_) return;
    if (presentationTaskIndex_ < static_cast<int>(tasks_.size()) - 1) {
        presentationTaskIndex_++;
    } else {
        presentationTaskIndex_ = 0; // Loop back to first
    }
}

void TaskController::previousPresentationTask() {
    if (!presentationMode_) return;
    if (presentationTaskIndex_ > 0) {
        presentationTaskIndex_--;
    } else {
        presentationTaskIndex_ = static_cast<int>(tasks_.size()) - 1; // Loop to last
    }
}

// --- Adjuntos/Archivos ---

void TaskController::addTaskAttachment(int taskIndex, const TaskAttachment& attachment) {
    auto task = getTask(taskIndex);
    if (!task) return;

    task->addAttachment(attachment);
    scheduleAutoSave();
}

void TaskController::removeTaskAttachment(int taskIndex, const std::string& attachmentId) {
    auto task = getTask(taskIndex);
    if (!task) return;

    task->removeAttachment(attachmentId);
    scheduleAutoSave();
}

void TaskController::clearTaskAttachments(int taskIndex) {
    auto task = getTask(taskIndex);
    if (!task) return;

    task->clearAttachments();
    scheduleAutoSave();
}

void TaskController::setTaskAttachments(int taskIndex, const std::vector<TaskAttachment>& attachments) {
    auto task = getTask(taskIndex);
    if (!task) return;

    task->setAttachments(attachments);
    scheduleAutoSave();
}

const std::vector<TaskAttachment>& TaskController::getTaskAttachments(int taskIndex) const {
    static const std::vector<TaskAttachment> emptyAttachments;
    auto task = getTask(taskIndex);
    if (!task) return emptyAttachments;
    return task->attachments();
}

size_t TaskController::getTaskAttachmentCount(int taskIndex) const {
    auto task = getTask(taskIndex);
    if (!task) return 0;
    return task->attachmentCount();
}

// --- Sync offline-first ---

void TaskController::setSyncConfig(const SyncConfig& config) {
    syncManager_->setConfig(config);
}

void TaskController::syncNow() {
    syncManager_->syncNow();
}

void TaskController::setSyncOnline(bool online) {
    syncManager_->setOnline(online);
}

// --- iCal / Calendario ---

std::string TaskController::exportTasksToICal() const {
    auto exporter = std::make_shared<ICalExporter>();
    return exporter->exportAll(tasks_);
}

std::string TaskController::exportTaskToICal(int taskIndex) const {
    auto exporter = std::make_shared<ICalExporter>();
    auto task = getTask(taskIndex);
    if (!task) return "";
    return exporter->exportTask(task);
}

std::vector<ImportedTask> TaskController::importTasksFromICal(const std::string& icsData) const {
    auto importer = std::make_shared<ICalImporter>();
    return importer->importFromString(icsData);
}

std::vector<std::shared_ptr<Task>> TaskController::importTasksFromICalAndAdd(const std::string& icsData) {
    auto importer = std::make_shared<ICalImporter>();
    auto imported = importer->importFromString(icsData);

    std::vector<std::shared_ptr<Task>> createdTasks;
    for (const auto& imp : imported) {
        auto task = importer->importTaskToTask(imp);
        tasks_.push_back(task);
        createdTasks.push_back(task);
    }

    if (!createdTasks.empty()) {
        if (tasksChangedCb_) tasksChangedCb_();
        scheduleAutoSave();
    }

    return createdTasks;
}

} // namespace checklist
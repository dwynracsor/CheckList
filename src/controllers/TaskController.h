#pragma once

#include <string>
#include <vector>
#include <memory>
#include <stack>
#include <functional>
#include <mutex>
#include <chrono>
#include <thread>
#include <atomic>

#include "domain/Task.h"
#include "domain/TaskTemplate.h"
#include "domain/StatsCalculator.h"
#include "ical/ICalExporter.h"
#include "ical/ICalImporter.h"
#include "persistence/JsonStorage.h"
#include "sync/SyncManager.h"
#include "undo/UndoCommand.h"

namespace checklist {

class TaskController {
public:
    using Ptr = std::shared_ptr<TaskController>;

    TaskController();
    explicit TaskController(const std::string& storagePath);
    explicit TaskController(std::shared_ptr<JsonStorage> storage);
    ~TaskController();

    // Delete copy/move
    TaskController(const TaskController&) = delete;
    TaskController& operator=(const TaskController&) = delete;

    // Accessors
    const std::vector<std::shared_ptr<Task>>& tasks() const { return tasks_; }
    size_t taskCount() const { return tasks_.size(); }
    bool isLoaded() const { return isLoaded_; }
    const std::string& lastError() const { return lastError_; }

    bool autoSave() const { return autoSave_; }
    void setAutoSave(bool value);

    int autoSaveDelay() const { return autoSaveDelay_; }
    void setAutoSaveDelay(int ms);

    // Undo/Redo
    bool canUndo() const;
    bool canRedo() const;
    void undo();
    void redo();
    void clearHistory();

    // Events / Callbacks
    using TasksChangedCallback = std::function<void()>;
    using LoadedChangedCallback = std::function<void()>;
    using ErrorChangedCallback = std::function<void()>;
    using AutoSaveChangedCallback = std::function<void()>;
    using AutoSaveDelayChangedCallback = std::function<void()>;
    using UndoStackChangedCallback = std::function<void()>;
    using RedoStackChangedCallback = std::function<void()>;
    using SavedCallback = std::function<void()>;
    using TaskAddedCallback = std::function<void(int)>;
    using TaskRemovedCallback = std::function<void(int)>;
    using TaskDurationChangedCallback = std::function<void(int taskIndex)>;
    using TaskPomodoroChangedCallback = std::function<void(int taskIndex)>;
    using TaskTimingStateChangedCallback = std::function<void(int taskIndex)>;
    using TaskDependencyChangedCallback = std::function<void(int taskIndex)>;
    using TaskTagsChangedCallback = std::function<void(int taskIndex)>;

    void setTasksChangedCallback(TasksChangedCallback cb) { tasksChangedCb_ = std::move(cb); }
    void setLoadedChangedCallback(LoadedChangedCallback cb) { loadedChangedCb_ = std::move(cb); }
    void setErrorChangedCallback(ErrorChangedCallback cb) { errorChangedCb_ = std::move(cb); }
    void setAutoSaveChangedCallback(AutoSaveChangedCallback cb) { autoSaveChangedCb_ = std::move(cb); }
    void setAutoSaveDelayChangedCallback(AutoSaveDelayChangedCallback cb) { autoSaveDelayChangedCb_ = std::move(cb); }
    void setUndoStackChangedCallback(UndoStackChangedCallback cb) { undoStackChangedCb_ = std::move(cb); }
    void setRedoStackChangedCallback(RedoStackChangedCallback cb) { redoStackChangedCb_ = std::move(cb); }
    void setSavedCallback(SavedCallback cb) { savedCb_ = std::move(cb); }
    void setTaskAddedCallback(TaskAddedCallback cb) { taskAddedCb_ = std::move(cb); }
    void setTaskRemovedCallback(TaskRemovedCallback cb) { taskRemovedCb_ = std::move(cb); }
    void setTaskDurationChangedCallback(TaskDurationChangedCallback cb) { taskDurationChangedCb_ = std::move(cb); }
    void setTaskPomodoroChangedCallback(TaskPomodoroChangedCallback cb) { taskPomodoroChangedCb_ = std::move(cb); }
    void setTaskTimingStateChangedCallback(TaskTimingStateChangedCallback cb) { taskTimingStateChangedCb_ = std::move(cb); }
    void setTaskDependencyChangedCallback(TaskDependencyChangedCallback cb) { taskDependencyChangedCb_ = std::move(cb); }
    void setTaskTagsChangedCallback(TaskTagsChangedCallback cb) { taskTagsChangedCb_ = std::move(cb); }
    void setTemplatesChangedCallback(std::function<void()> cb) { templatesChangedCb_ = std::move(cb); }

    // Core operations
    void load();
    bool save();

    // Task operations
    std::shared_ptr<Task> addTask(const std::string& name, const std::string& category = "");
    void removeTask(int index);
    std::shared_ptr<Task> getTask(int index) const;

    // Item operations on task
    void addItemToTask(int taskIndex, const std::string& text);
    void removeItemFromTask(int taskIndex, int itemIndex);
    void toggleItemInTask(int taskIndex, int itemIndex, bool done);
    void setItemTextInTask(int taskIndex, int itemIndex, const std::string& text);

    // Item operations on correction
    void setItemTextInCorrection(int taskIndex, int correctionIndex, int itemIndex, const std::string& text);

    // Correction operations on task
    std::shared_ptr<Correction> addCorrectionToTask(int taskIndex, const std::string& name);
    void removeCorrectionFromTask(int taskIndex, int correctionIndex);

    // Name editing
    void setTaskName(int taskIndex, const std::string& name);
    void setCorrectionName(int taskIndex, int correctionIndex, const std::string& name);
    void setTaskCategory(int taskIndex, const std::string& category);

    // --- Tiempo y Pomodoro ---

    void setTaskEstimatedDuration(int taskIndex, int64_t seconds);
    int64_t getTaskEstimatedDuration(int taskIndex) const;
    int64_t getTaskActualDuration(int taskIndex) const;

    void startTaskTiming(int taskIndex);
    void stopTaskTiming(int taskIndex);
    bool isTaskTiming(int taskIndex) const;

    void incrementTaskPomodoro(int taskIndex);
    void resetTaskPomodoros(int taskIndex);
    int getTaskPomodoroCount(int taskIndex) const;

    int64_t getTaskRemainingTime(int taskIndex) const;
    int getTaskProgressPercentage(int taskIndex) const;

    // --- Dependencias entre tareas ---

    void setTaskDependency(int taskIndex, TaskDependency type, const std::string& dependsOnId);
    TaskDependency getTaskDependencyType(int taskIndex) const;
    const std::string& getTaskDependencyId(int taskIndex) const;
    void removeTaskDependency(int taskIndex);

    // Verificar si una tarea está bloqueada por su dependencia
    bool isTaskBlockedByDependency(int taskIndex) const;

    // Buscar índice de tarea por ID (útil para QML)
    int findTaskIndexById(const std::string& id) const;

    // --- Etiquetas (Tags) ---

    void addTaskTag(int taskIndex, const std::string& tag);
    void removeTaskTag(int taskIndex, const std::string& tag);
    void clearTaskTags(int taskIndex);
    void setTaskTags(int taskIndex, const std::vector<std::string>& tags);

    const std::vector<std::string>& getTaskTags(int taskIndex) const;
    size_t getTaskTagCount(int taskIndex) const;
    bool taskHasTag(int taskIndex, const std::string& tag) const;

    // Obtener todos los tags únicos de todas las tareas
    std::vector<std::string> getAllTags() const;

    // --- Plantillas de tareas recurrentes ---

    void addTemplate(const std::shared_ptr<TaskTemplate>& template_);
    void removeTemplate(const std::string& templateId);
    void updateTemplate(const std::shared_ptr<TaskTemplate>& template_);

    std::shared_ptr<TaskTemplate> getTemplate(const std::string& templateId) const;
    std::shared_ptr<TaskTemplate> getTemplateByIndex(int index) const;
    size_t templateCount() const;

    std::vector<std::shared_ptr<TaskTemplate>> getTemplatesByRecurrence(TemplateRecurrence recurrence) const;

    // Crear una tarea desde una plantilla
    std::shared_ptr<Task> createTaskFromTemplate(const std::string& templateId, const std::string& taskName);
    std::shared_ptr<Task> createTaskFromTemplateByIndex(int templateIndex, const std::string& taskName);

    // Obtener todos los IDs de plantillas (útil para QML)
    std::vector<std::string> getAllTemplateIds() const;

    // --- Estadísticas de productividad ---

    WeeklyReport getWeeklyReport(int days = 7) const;
    std::vector<DailyStat> getDailyStats(int days = 7) const;
    ProductivityStreak getProductivityStreak() const;
    std::vector<CategoryTimeStat> getCategoryTimeDistribution() const;

    // --- Adjuntos/Archivos ---

    void addTaskAttachment(int taskIndex, const TaskAttachment& attachment);
    void removeTaskAttachment(int taskIndex, const std::string& attachmentId);
    void clearTaskAttachments(int taskIndex);
    void setTaskAttachments(int taskIndex, const std::vector<TaskAttachment>& attachments);

    const std::vector<TaskAttachment>& getTaskAttachments(int taskIndex) const;
    size_t getTaskAttachmentCount(int taskIndex) const;

    // --- Modo Presentación / Kiosk ---

    bool presentationMode() const { return presentationMode_; }
    int presentationTaskIndex() const { return presentationTaskIndex_; }

    void setPresentationMode(bool enabled);
    void setPresentationTaskIndex(int index);
    void nextPresentationTask();
    void previousPresentationTask();

    // --- iCal / Calendario ---
    std::string exportTasksToICal() const;
    std::string exportTaskToICal(int taskIndex) const;
    std::vector<ImportedTask> importTasksFromICal(const std::string& icsData) const;
    std::vector<std::shared_ptr<Task>> importTasksFromICalAndAdd(const std::string& icsData);

    // --- Import from JSON
    void importFromJsonString(const std::string& jsonString);

    // JSON storage access
    std::shared_ptr<JsonStorage> storage() const { return storage_; }

    // --- Sync offline-first ---
    std::shared_ptr<SyncManager> syncManager() const { return syncManager_; }
    SyncStatus syncStatus() const { return syncManager_->status(); }
    int syncPendingCount() const { return syncManager_->pendingChangesCount(); }
    bool syncIsOnline() const { return syncManager_->isOnline(); }

    void setSyncConfig(const SyncConfig& config);
    void syncNow();
    void setSyncOnline(bool online);

private:
    std::vector<std::shared_ptr<Task>> tasks_;
    std::vector<std::shared_ptr<TaskTemplate>> templates_;
    bool presentationMode_;
    int presentationTaskIndex_;
    std::shared_ptr<SyncManager> syncManager_;
    std::stack<std::shared_ptr<UndoCommand>> undoStack_;
    std::stack<std::shared_ptr<UndoCommand>> redoStack_;
    std::shared_ptr<JsonStorage> storage_;
    std::mutex autoSaveLock_;
    std::atomic<bool> isLoaded_;
    std::atomic<bool> autoSave_;
    int autoSaveDelay_;
    std::string lastError_;

    // Callbacks
    TasksChangedCallback tasksChangedCb_;
    LoadedChangedCallback loadedChangedCb_;
    ErrorChangedCallback errorChangedCb_;
    AutoSaveChangedCallback autoSaveChangedCb_;
    AutoSaveDelayChangedCallback autoSaveDelayChangedCb_;
    UndoStackChangedCallback undoStackChangedCb_;
    RedoStackChangedCallback redoStackChangedCb_;
    SavedCallback savedCb_;
    TaskAddedCallback taskAddedCb_;
    TaskRemovedCallback taskRemovedCb_;
    TaskDurationChangedCallback taskDurationChangedCb_;
    TaskPomodoroChangedCallback taskPomodoroChangedCb_;
    TaskTimingStateChangedCallback taskTimingStateChangedCb_;
    TaskDependencyChangedCallback taskDependencyChangedCb_;
    TaskTagsChangedCallback taskTagsChangedCb_;
    std::function<void()> templatesChangedCb_;

    // Helpers
    void scheduleAutoSave();
    void debounceSave();
    void setError(const std::string& message);
    void pushUndo(std::shared_ptr<UndoCommand> cmd);
};

} // namespace checklist
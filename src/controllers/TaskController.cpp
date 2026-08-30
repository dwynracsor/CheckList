#include "controllers/TaskController.h"
#include "domain/Task.h"
#include "domain/Correction.h"
#include "domain/ChecklistItem.h"
#include "persistence/JsonStorage.h"
#include "undo/UndoCommand.h"

#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

TaskController::TaskController(QObject *parent)
    : QObject(parent)
    , m_storage(new JsonStorage(this))
    , m_autoSaveTimer(new QTimer(this))
    , m_isLoaded(false)
    , m_autoSave(true)
    , m_autoSaveDelay(500)
{
    init();
}

TaskController::TaskController(const QString &storagePath, QObject *parent)
    : QObject(parent)
    , m_storage(new JsonStorage(storagePath, this))
    , m_autoSaveTimer(new QTimer(this))
    , m_isLoaded(false)
    , m_autoSave(true)
    , m_autoSaveDelay(500)
{
    init();
}

void TaskController::init()
{
    m_autoSaveTimer->setSingleShot(true);
    m_autoSaveTimer->setInterval(m_autoSaveDelay);
    connect(m_autoSaveTimer, &QTimer::timeout, this, &TaskController::onAutoSaveTimer);
    connect(m_storage, &JsonStorage::error, this, &TaskController::setError);
}

TaskController::~TaskController()
{
    qDeleteAll(m_tasks);
}

// --- Properties ---

QQmlListProperty<Task> TaskController::tasks()
{
    return QQmlListProperty<Task>(
        this,
        nullptr,
        &TaskController::tasksAppend,
        &TaskController::tasksCount,
        &TaskController::tasksAt,
        &TaskController::tasksClear
    );
}

int TaskController::taskCount() const
{
    return m_tasks.size();
}

bool TaskController::isLoaded() const
{
    return m_isLoaded;
}

QString TaskController::lastError() const
{
    return m_lastError;
}

bool TaskController::autoSave() const
{
    return m_autoSave;
}

void TaskController::setAutoSave(bool enabled)
{
    if (m_autoSave != enabled) {
        m_autoSave = enabled;
        emit autoSaveChanged();
    }
}

int TaskController::autoSaveDelay() const
{
    return m_autoSaveDelay;
}

void TaskController::setAutoSaveDelay(int ms)
{
    if (m_autoSaveDelay != ms) {
        m_autoSaveDelay = ms;
        m_autoSaveTimer->setInterval(ms);
        emit autoSaveDelayChanged();
    }
}

// --- Core Operations ---

void TaskController::load()
{
    QList<Task*> loaded = m_storage->load();

    // Clear existing
    qDeleteAll(m_tasks);
    m_tasks.clear();

    // Set parent for proper ownership
    for (Task *task : loaded) {
        task->setParent(this);
    }

    m_tasks = loaded;

    if (!m_isLoaded) {
        m_isLoaded = true;
        emit loadedChanged();
    }

    emit tasksChanged();
}

void TaskController::save()
{
    if (m_storage->save(m_tasks)) {
        emit saved();
    }
}

// --- Task Operations ---

Task* TaskController::addTask(const QString &name)
{
    if (name.trimmed().isEmpty()) {
        setError(tr("El nombre de la tarea no puede estar vacío"));
        return nullptr;
    }

    Task *task = new Task(name.trimmed(), QDate::currentDate(), this);
    int newIndex = m_tasks.size();
    m_tasks.append(task);

    emit tasksChanged();
    emit taskAdded(newIndex);
    scheduleAutoSave();

    // Undo command
    auto undoCmd = [this, newIndex]() {
        if (newIndex >= 0 && newIndex < m_tasks.size()) {
            Task *t = m_tasks.takeAt(newIndex);
            t->deleteLater();
            emit tasksChanged();
        }
    };
    auto redoCmd = [this, name, newIndex]() {
        Task *t = new Task(name.trimmed(), QDate::currentDate(), this);
        if (newIndex >= m_tasks.size()) {
            m_tasks.append(t);
        } else {
            m_tasks.insert(newIndex, t);
        }
        emit tasksChanged();
        emit taskAdded(newIndex);
    };
    pushUndo(std::make_shared<UndoCommand>("Agregar tarea: " + name.trimmed(), undoCmd, redoCmd));

    return task;
}

void TaskController::removeTask(int index)
{
    if (index < 0 || index >= m_tasks.size()) return;

    Task *task = m_tasks.takeAt(index);
    QString taskName = task->name();

    // Store task data for undo
    QString savedId = task->id();
    QString savedName = task->name();
    QDate savedDate = QDate::currentDate();

    emit tasksChanged();
    emit taskRemoved(index);
    scheduleAutoSave();

    // Undo command
    int capturedIndex = index;
    auto undoCmd = [this, task, capturedIndex, savedName]() {
        task->setParent(this);
        if (capturedIndex >= m_tasks.size()) {
            m_tasks.append(task);
        } else {
            m_tasks.insert(capturedIndex, task);
        }
        emit tasksChanged();
        emit taskAdded(capturedIndex);
    };
    auto redoCmd = [this, task, capturedIndex]() {
        int idx = m_tasks.indexOf(task);
        if (idx >= 0) {
            m_tasks.removeAt(idx);
            task->setParent(nullptr);
            task->deleteLater();
            emit tasksChanged();
            emit taskRemoved(capturedIndex);
        }
    };
    pushUndo(std::make_shared<UndoCommand>("Eliminar tarea: " + taskName, undoCmd, redoCmd));

    task->setParent(nullptr);
    task->deleteLater();
}

Task* TaskController::getTask(int index) const
{
    if (index < 0 || index >= m_tasks.size()) return nullptr;
    return m_tasks.at(index);
}

// --- Item Operations on Task ---

void TaskController::addItemToTask(int taskIndex, const QString &text)
{
    Task *task = getTask(taskIndex);
    if (!task || text.trimmed().isEmpty()) return;

    task->addNewItem(text.trimmed());
    int itemIndex = task->itemCount() - 1;
    scheduleAutoSave();

    auto undoCmd = [this, taskIndex, itemIndex]() {
        Task *t = getTask(taskIndex);
        if (t && itemIndex < t->itemCount()) {
            t->removeItem(itemIndex);
            emit tasksChanged();
        }
    };
    auto redoCmd = [this, taskIndex, text]() {
        Task *t = getTask(taskIndex);
        if (t) {
            t->addNewItem(text.trimmed());
            emit tasksChanged();
        }
    };
    pushUndo(std::make_shared<UndoCommand>("Agregar ítem", undoCmd, redoCmd));
}

void TaskController::removeItemFromTask(int taskIndex, int itemIndex)
{
    Task *task = getTask(taskIndex);
    if (!task) return;

    ChecklistItem *item = task->getItem(itemIndex);
    if (!item) return;

    // Save item data for undo
    QString itemText = item->text();
    bool itemDone = item->done();

    task->removeItem(itemIndex);
    scheduleAutoSave();

    auto undoCmd = [this, taskIndex, itemIndex, itemText, itemDone]() {
        Task *t = getTask(taskIndex);
        if (t) {
            ChecklistItem *newItem = new ChecklistItem(itemText, itemDone, t);
            t->addItem(newItem);
            emit tasksChanged();
        }
    };
    auto redoCmd = [this, taskIndex, itemIndex]() {
        Task *t = getTask(taskIndex);
        if (t && itemIndex < t->itemCount()) {
            t->removeItem(itemIndex);
            emit tasksChanged();
        }
    };
    pushUndo(std::make_shared<UndoCommand>("Eliminar ítem: " + itemText, undoCmd, redoCmd));
}

void TaskController::toggleItemInTask(int taskIndex, int itemIndex, bool done)
{
    Task *task = getTask(taskIndex);
    if (!task) return;

    ChecklistItem *item = task->getItem(itemIndex);
    if (item) {
        bool previousDone = item->done();
        item->setDone(done);
        scheduleAutoSave();

        auto undoCmd = [this, taskIndex, itemIndex, previousDone]() {
            Task *t = getTask(taskIndex);
            if (t) {
                ChecklistItem *i = t->getItem(itemIndex);
                if (i) i->setDone(previousDone);
            }
        };
        auto redoCmd = [this, taskIndex, itemIndex, done]() {
            Task *t = getTask(taskIndex);
            if (t) {
                ChecklistItem *i = t->getItem(itemIndex);
                if (i) i->setDone(done);
            }
        };
        pushUndo(std::make_shared<UndoCommand>(done ? "Marcar ítem" : "Desmarcar ítem", undoCmd, redoCmd));
    }
}

void TaskController::setItemTextInTask(int taskIndex, int itemIndex, const QString &text)
{
    Task *task = getTask(taskIndex);
    if (!task) return;

    ChecklistItem *item = task->getItem(itemIndex);
    if (!item) return;

    QString newText = text.trimmed();
    QString oldText = item->text();
    if (oldText == newText || newText.isEmpty()) return;

    item->setText(newText);
    scheduleAutoSave();

    auto undoCmd = [this, taskIndex, itemIndex, oldText]() {
        Task *t = getTask(taskIndex);
        if (t) {
            ChecklistItem *i = t->getItem(itemIndex);
            if (i) i->setText(oldText);
        }
    };
    auto redoCmd = [this, taskIndex, itemIndex, newText]() {
        Task *t = getTask(taskIndex);
        if (t) {
            ChecklistItem *i = t->getItem(itemIndex);
            if (i) i->setText(newText);
        }
    };
    pushUndo(std::make_shared<UndoCommand>("Editar ítem", undoCmd, redoCmd));
}

// --- Correction Operations on Task ---

Correction* TaskController::addCorrectionToTask(int taskIndex, const QString &name)
{
    Task *task = getTask(taskIndex);
    if (!task || name.trimmed().isEmpty()) return nullptr;

    Correction *corr = task->addNewCorrection(name.trimmed());
    int corrIndex = task->correctionCount() - 1;
    scheduleAutoSave();

    auto undoCmd = [this, taskIndex, corrIndex]() {
        Task *t = getTask(taskIndex);
        if (t && corrIndex < t->correctionCount()) {
            t->removeCorrection(corrIndex);
            emit tasksChanged();
        }
    };
    auto redoCmd = [this, taskIndex, name]() {
        Task *t = getTask(taskIndex);
        if (t) {
            t->addNewCorrection(name.trimmed());
            emit tasksChanged();
        }
    };
    pushUndo(std::make_shared<UndoCommand>("Agregar corrección: " + name.trimmed(), undoCmd, redoCmd));

    return corr;
}

void TaskController::removeCorrectionFromTask(int taskIndex, int correctionIndex)
{
    Task *task = getTask(taskIndex);
    if (!task) return;

    Correction *corr = task->getCorrection(correctionIndex);
    if (!corr) return;

    QString corrName = corr->name();
    task->removeCorrection(correctionIndex);
    scheduleAutoSave();

    auto undoCmd = [this, taskIndex, correctionIndex, corrName]() {
        Task *t = getTask(taskIndex);
        if (t) {
            Correction *c = new Correction(corrName, QDate::currentDate(), t);
            if (correctionIndex >= t->correctionCount()) {
                t->addCorrection(c);
            } else {
                // Insert at position by adding all after
                t->addCorrection(c);
            }
            emit tasksChanged();
        }
    };
    auto redoCmd = [this, taskIndex, correctionIndex]() {
        Task *t = getTask(taskIndex);
        if (t && correctionIndex < t->correctionCount()) {
            t->removeCorrection(correctionIndex);
            emit tasksChanged();
        }
    };
    pushUndo(std::make_shared<UndoCommand>("Eliminar corrección: " + corrName, undoCmd, redoCmd));
}

// --- Item Operations on Correction ---

void TaskController::addItemToCorrection(int taskIndex, int correctionIndex, const QString &text)
{
    Task *task = getTask(taskIndex);
    if (!task || text.trimmed().isEmpty()) return;

    Correction *corr = task->getCorrection(correctionIndex);
    if (!corr) return;

    corr->addNewItem(text.trimmed());
    int itemIndex = corr->itemCount() - 1;
    scheduleAutoSave();

    auto undoCmd = [this, taskIndex, correctionIndex, itemIndex]() {
        Task *t = getTask(taskIndex);
        if (t) {
            Correction *c = t->getCorrection(correctionIndex);
            if (c && itemIndex < c->itemCount()) {
                c->removeItem(itemIndex);
                emit tasksChanged();
            }
        }
    };
    auto redoCmd = [this, taskIndex, correctionIndex, text]() {
        Task *t = getTask(taskIndex);
        if (t) {
            Correction *c = t->getCorrection(correctionIndex);
            if (c) {
                c->addNewItem(text.trimmed());
                emit tasksChanged();
            }
        }
    };
    pushUndo(std::make_shared<UndoCommand>("Agregar ítem a corrección", undoCmd, redoCmd));
}

void TaskController::removeItemFromCorrection(int taskIndex, int correctionIndex, int itemIndex)
{
    Task *task = getTask(taskIndex);
    if (!task) return;

    Correction *corr = task->getCorrection(correctionIndex);
    if (!corr) return;

    ChecklistItem *item = corr->getItem(itemIndex);
    if (!item) return;

    QString itemText = item->text();
    bool itemDone = item->done();

    corr->removeItem(itemIndex);
    scheduleAutoSave();

    auto undoCmd = [this, taskIndex, correctionIndex, itemIndex, itemText, itemDone]() {
        Task *t = getTask(taskIndex);
        if (t) {
            Correction *c = t->getCorrection(correctionIndex);
            if (c) {
                ChecklistItem *newItem = new ChecklistItem(itemText, itemDone, c);
                c->addItem(newItem);
                emit tasksChanged();
            }
        }
    };
    auto redoCmd = [this, taskIndex, correctionIndex, itemIndex]() {
        Task *t = getTask(taskIndex);
        if (t) {
            Correction *c = t->getCorrection(correctionIndex);
            if (c && itemIndex < c->itemCount()) {
                c->removeItem(itemIndex);
                emit tasksChanged();
            }
        }
    };
    pushUndo(std::make_shared<UndoCommand>("Eliminar ítem de corrección: " + itemText, undoCmd, redoCmd));
}

void TaskController::toggleItemInCorrection(int taskIndex, int correctionIndex, int itemIndex, bool done)
{
    Task *task = getTask(taskIndex);
    if (!task) return;

    Correction *corr = task->getCorrection(correctionIndex);
    if (!corr) return;

    ChecklistItem *item = corr->getItem(itemIndex);
    if (item) {
        bool previousDone = item->done();
        item->setDone(done);
        scheduleAutoSave();

        auto undoCmd = [this, taskIndex, correctionIndex, itemIndex, previousDone]() {
            Task *t = getTask(taskIndex);
            if (t) {
                Correction *c = t->getCorrection(correctionIndex);
                if (c) {
                    ChecklistItem *i = c->getItem(itemIndex);
                    if (i) i->setDone(previousDone);
                }
            }
        };
        auto redoCmd = [this, taskIndex, correctionIndex, itemIndex, done]() {
            Task *t = getTask(taskIndex);
            if (t) {
                Correction *c = t->getCorrection(correctionIndex);
                if (c) {
                    ChecklistItem *i = c->getItem(itemIndex);
                    if (i) i->setDone(done);
                }
            }
        };
        pushUndo(std::make_shared<UndoCommand>(done ? "Marcar ítem corrección" : "Desmarcar ítem corrección", undoCmd, redoCmd));
    }
}

// --- Name Editing ---

void TaskController::setTaskName(int taskIndex, const QString &name)
{
    Task *task = getTask(taskIndex);
    if (!task || name.trimmed().isEmpty()) return;

    QString oldName = task->name();
    task->setName(name.trimmed());
    scheduleAutoSave();

    auto undoCmd = [this, taskIndex, oldName]() {
        Task *t = getTask(taskIndex);
        if (t) t->setName(oldName);
    };
    auto redoCmd = [this, taskIndex, name]() {
        Task *t = getTask(taskIndex);
        if (t) t->setName(name.trimmed());
    };
    pushUndo(std::make_shared<UndoCommand>("Renombrar tarea", undoCmd, redoCmd));
}

void TaskController::setCorrectionName(int taskIndex, int correctionIndex, const QString &name)
{
    Task *task = getTask(taskIndex);
    if (!task || name.trimmed().isEmpty()) return;

    Correction *corr = task->getCorrection(correctionIndex);
    if (!corr) return;

    QString oldName = corr->name();
    corr->setName(name.trimmed());
    scheduleAutoSave();

    auto undoCmd = [this, taskIndex, correctionIndex, oldName]() {
        Task *t = getTask(taskIndex);
        if (t) {
            Correction *c = t->getCorrection(correctionIndex);
            if (c) c->setName(oldName);
        }
    };
    auto redoCmd = [this, taskIndex, correctionIndex, name]() {
        Task *t = getTask(taskIndex);
        if (t) {
            Correction *c = t->getCorrection(correctionIndex);
            if (c) c->setName(name.trimmed());
        }
    };
    pushUndo(std::make_shared<UndoCommand>("Renombrar corrección", undoCmd, redoCmd));
}

// --- Import from web format ---

void TaskController::importFromJsonString(const QString &jsonString)
{
    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(jsonString.toUtf8(), &parseError);

    if (parseError.error != QJsonParseError::NoError) {
        setError("Error al parsear JSON: " + parseError.errorString());
        return;
    }

    QJsonArray array = doc.isArray() ? doc.array() : QJsonArray();
    int imported = 0;

    for (const QJsonValue &value : array) {
        if (!value.isObject()) continue;

        QJsonObject json = value.toObject();
        Task *task = JsonStorage::jsonToTask(json, this);
        if (task) {
            m_tasks.append(task);
            imported++;
        }
    }

    if (imported > 0) {
        emit tasksChanged();
        scheduleAutoSave();
    }
}

// --- Private Slots ---

void TaskController::onAutoSaveTimer()
{
    save();
}

// --- Private Methods ---

void TaskController::scheduleAutoSave()
{
    if (m_autoSave) {
        m_autoSaveTimer->start();
    }
}

void TaskController::setError(const QString &message)
{
    if (m_lastError != message) {
        m_lastError = message;
        emit errorChanged();
    }
}

// --- Undo/Redo ---

bool TaskController::canUndo() const
{
    return !m_undoStack.isEmpty();
}

bool TaskController::canRedo() const
{
    return !m_redoStack.isEmpty();
}

void TaskController::undo()
{
    if (m_undoStack.isEmpty()) return;

    auto cmd = m_undoStack.pop();
    cmd->undo();
    m_redoStack.push(cmd);

    emit undoStackChanged();
    emit redoStackChanged();
    emit tasksChanged();
    scheduleAutoSave();
}

void TaskController::redo()
{
    if (m_redoStack.isEmpty()) return;

    auto cmd = m_redoStack.pop();
    cmd->redo();
    m_undoStack.push(cmd);

    emit undoStackChanged();
    emit redoStackChanged();
    emit tasksChanged();
    scheduleAutoSave();
}

void TaskController::clearHistory()
{
    m_undoStack.clear();
    m_redoStack.clear();
    emit undoStackChanged();
    emit redoStackChanged();
}

void TaskController::pushUndo(std::shared_ptr<UndoCommand> cmd)
{
    m_undoStack.push(cmd);
    m_redoStack.clear();
    emit undoStackChanged();
    emit redoStackChanged();
}

// QQmlListProperty callbacks
void TaskController::tasksAppend(QQmlListProperty<Task> *list, Task *task)
{
    TaskController *self = qobject_cast<TaskController*>(list->object);
    if (self) {
        task->setParent(self);
        self->m_tasks.append(task);
        emit self->tasksChanged();
    }
}

qsizetype TaskController::tasksCount(QQmlListProperty<Task> *list)
{
    TaskController *self = qobject_cast<TaskController*>(list->object);
    return self ? self->m_tasks.size() : 0;
}

Task* TaskController::tasksAt(QQmlListProperty<Task> *list, qsizetype index)
{
    TaskController *self = qobject_cast<TaskController*>(list->object);
    return self ? self->m_tasks.at(index) : nullptr;
}

void TaskController::tasksClear(QQmlListProperty<Task> *list)
{
    TaskController *self = qobject_cast<TaskController*>(list->object);
    if (self) {
        qDeleteAll(self->m_tasks);
        self->m_tasks.clear();
        emit self->tasksChanged();
    }
}

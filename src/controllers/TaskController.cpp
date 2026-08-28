#include "controllers/TaskController.h"
#include "domain/Task.h"
#include "domain/Correction.h"
#include "domain/ChecklistItem.h"
#include "persistence/JsonStorage.h"

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
    m_autoSaveTimer->setSingleShot(true);
    m_autoSaveTimer->setInterval(m_autoSaveDelay);
    connect(m_autoSaveTimer, &QTimer::timeout, this, &TaskController::onAutoSaveTimer);
    connect(m_storage, &JsonStorage::error, this, &TaskController::setError);
}

TaskController::TaskController(const QString &storagePath, QObject *parent)
    : QObject(parent)
    , m_storage(new JsonStorage(storagePath, this))
    , m_autoSaveTimer(new QTimer(this))
    , m_isLoaded(false)
    , m_autoSave(true)
    , m_autoSaveDelay(500)
{
    m_autoSaveTimer->setSingleShot(true);
    m_autoSaveTimer->setInterval(m_autoSaveDelay);
    connect(m_autoSaveTimer, &QTimer::timeout, this, &TaskController::onAutoSaveTimer);
    connect(m_storage, &JsonStorage::error, this, &TaskController::setError);
}

TaskController::~TaskController() = default;

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
        setError("El nombre de la tarea no puede estar vacío");
        return nullptr;
    }

    Task *task = new Task(name.trimmed(), QDate::currentDate(), this);
    m_tasks.append(task);

    emit tasksChanged();
    emit taskAdded(m_tasks.size() - 1);
    scheduleAutoSave();

    return task;
}

void TaskController::removeTask(int index)
{
    if (index < 0 || index >= m_tasks.size()) return;

    Task *task = m_tasks.takeAt(index);
    task->deleteLater();

    emit tasksChanged();
    emit taskRemoved(index);
    scheduleAutoSave();
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
    scheduleAutoSave();
}

void TaskController::removeItemFromTask(int taskIndex, int itemIndex)
{
    Task *task = getTask(taskIndex);
    if (!task) return;

    task->removeItem(itemIndex);
    scheduleAutoSave();
}

void TaskController::toggleItemInTask(int taskIndex, int itemIndex, bool done)
{
    Task *task = getTask(taskIndex);
    if (!task) return;

    ChecklistItem *item = task->getItem(itemIndex);
    if (item) {
        item->setDone(done);
        scheduleAutoSave();
    }
}

// --- Correction Operations on Task ---

Correction* TaskController::addCorrectionToTask(int taskIndex, const QString &name)
{
    Task *task = getTask(taskIndex);
    if (!task || name.trimmed().isEmpty()) return nullptr;

    Correction *corr = task->addNewCorrection(name.trimmed());
    scheduleAutoSave();

    return corr;
}

void TaskController::removeCorrectionFromTask(int taskIndex, int correctionIndex)
{
    Task *task = getTask(taskIndex);
    if (!task) return;

    task->removeCorrection(correctionIndex);
    scheduleAutoSave();
}

// --- Item Operations on Correction ---

void TaskController::addItemToCorrection(int taskIndex, int correctionIndex, const QString &text)
{
    Task *task = getTask(taskIndex);
    if (!task || text.trimmed().isEmpty()) return;

    Correction *corr = task->getCorrection(correctionIndex);
    if (!corr) return;

    corr->addNewItem(text.trimmed());
    scheduleAutoSave();
}

void TaskController::removeItemFromCorrection(int taskIndex, int correctionIndex, int itemIndex)
{
    Task *task = getTask(taskIndex);
    if (!task) return;

    Correction *corr = task->getCorrection(correctionIndex);
    if (!corr) return;

    corr->removeItem(itemIndex);
    scheduleAutoSave();
}

void TaskController::toggleItemInCorrection(int taskIndex, int correctionIndex, int itemIndex, bool done)
{
    Task *task = getTask(taskIndex);
    if (!task) return;

    Correction *corr = task->getCorrection(correctionIndex);
    if (!corr) return;

    ChecklistItem *item = corr->getItem(itemIndex);
    if (item) {
        item->setDone(done);
        scheduleAutoSave();
    }
}

// --- Name Editing ---

void TaskController::setTaskName(int taskIndex, const QString &name)
{
    Task *task = getTask(taskIndex);
    if (!task || name.trimmed().isEmpty()) return;

    task->setName(name.trimmed());
    scheduleAutoSave();
}

void TaskController::setCorrectionName(int taskIndex, int correctionIndex, const QString &name)
{
    Task *task = getTask(taskIndex);
    if (!task || name.trimmed().isEmpty()) return;

    Correction *corr = task->getCorrection(correctionIndex);
    if (!corr) return;

    corr->setName(name.trimmed());
    scheduleAutoSave();
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

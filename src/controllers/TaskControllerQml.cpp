#include "controllers/TaskControllerQml.h"
#include <QQmlListProperty>

namespace checklist {

TaskControllerQml::TaskControllerQml(QObject* parent)
    : QObject(parent), controller_(std::make_shared<TaskController>()) {}

TaskControllerQml::TaskControllerQml(const std::string& storagePath, QObject* parent)
    : QObject(parent), controller_(std::make_shared<TaskController>(storagePath)) {}

TaskControllerQml::~TaskControllerQml() {
    qDeleteAll(tasksCache_);
    tasksCache_.clear();
}

QQmlListProperty<TaskQml> TaskControllerQml::tasks() {
    return QQmlListProperty<TaskQml>(this, &tasksCache_);
}

void TaskControllerQml::load() {
    controller_->load();
    rebuildTasksCache();
    emit isLoadedChanged();
    emit tasksChanged();
    emit taskCountChanged();
}

bool TaskControllerQml::save() {
    return controller_->save();
}

void TaskControllerQml::addTask(const QString& name, const QString& category) {
    controller_->addTask(name.toStdString(), category.toStdString());
    rebuildTasksCache();
    emit tasksChanged();
    emit taskCountChanged();
    emit taskAdded(static_cast<int>(controller_->taskCount()) - 1);
}

void TaskControllerQml::removeTask(int index) {
    controller_->removeTask(index);
    rebuildTasksCache();
    emit tasksChanged();
    emit taskCountChanged();
    emit taskRemoved(index);
}

TaskQml* TaskControllerQml::getTask(int index) {
    if (index >= 0 && index < static_cast<int>(tasksCache_.size())) {
        return tasksCache_[index];
    }
    return nullptr;
}

void TaskControllerQml::undo() {
    controller_->undo();
    rebuildTasksCache();
    emit tasksChanged();
    emit canUndoChanged();
    emit canRedoChanged();
}

void TaskControllerQml::redo() {
    controller_->redo();
    rebuildTasksCache();
    emit tasksChanged();
    emit canUndoChanged();
    emit canRedoChanged();
}

void TaskControllerQml::clearHistory() {
    controller_->clearHistory();
    emit canUndoChanged();
    emit canRedoChanged();
}

void TaskControllerQml::importFromJsonString(const QString& jsonString) {
    controller_->importFromJsonString(jsonString.toStdString());
    rebuildTasksCache();
    emit tasksChanged();
    emit taskCountChanged();
}

void TaskControllerQml::nextPresentationTask() {
    controller_->nextPresentationTask();
    emit presentationTaskIndexChanged();
}

void TaskControllerQml::previousPresentationTask() {
    controller_->previousPresentationTask();
    emit presentationTaskIndexChanged();
}

// --- Adjuntos/Archivos ---

void TaskControllerQml::addTaskAttachment(int taskIndex, const QString& fileName, const QString& filePath, const QString& fileType, int64_t fileSize) {
    TaskAttachment att;
    att.id = Task::generateId();
    att.fileName = fileName.toStdString();
    att.filePath = filePath.toStdString();
    att.fileType = fileType.toStdString();
    att.fileSize = fileSize;
    att.addedDate = Date::today().toString();

    auto task = controller_->getTask(taskIndex);
    if (task) {
        task->addAttachment(att);
        controller_->save();
    }
}

QStringList TaskControllerQml::getTaskAttachmentFileNames(int taskIndex) const {
    QStringList names;
    const auto& atts = controller_->getTaskAttachments(taskIndex);
    for (const auto& att : atts) {
        names << QString::fromStdString(att.fileName);
    }
    return names;
}

QString TaskControllerQml::getTaskAttachmentFilePath(int taskIndex, int attachmentIndex) const {
    const auto& atts = controller_->getTaskAttachments(taskIndex);
    if (attachmentIndex >= 0 && attachmentIndex < static_cast<int>(atts.size())) {
        return QString::fromStdString(atts[attachmentIndex].filePath);
    }
    return QString();
}

QString TaskControllerQml::getTaskAttachmentFileType(int taskIndex, int attachmentIndex) const {
    const auto& atts = controller_->getTaskAttachments(taskIndex);
    if (attachmentIndex >= 0 && attachmentIndex < static_cast<int>(atts.size())) {
        return QString::fromStdString(atts[attachmentIndex].fileType);
    }
    return QString();
}

int64_t TaskControllerQml::getTaskAttachmentFileSize(int taskIndex, int attachmentIndex) const {
    const auto& atts = controller_->getTaskAttachments(taskIndex);
    if (attachmentIndex >= 0 && attachmentIndex < static_cast<int>(atts.size())) {
        return atts[attachmentIndex].fileSize;
    }
    return 0;
}

int TaskControllerQml::getTaskAttachmentCount(int taskIndex) const {
    return static_cast<int>(controller_->getTaskAttachmentCount(taskIndex));
}

void TaskControllerQml::removeTaskAttachment(int taskIndex, const QString& attachmentId) {
    controller_->removeTaskAttachment(taskIndex, attachmentId.toStdString());
}

void TaskControllerQml::clearTaskAttachments(int taskIndex) {
    controller_->clearTaskAttachments(taskIndex);
}

// --- Sync offline-first ---

QString TaskControllerQml::syncStatusString() const {
    auto status = controller_->syncStatus();
    switch (status) {
        case SyncStatus::Offline: return "Offline";
        case SyncStatus::Synced: return "Sincronizado";
        case SyncStatus::Syncing: return "Sincronizando...";
        case SyncStatus::Conflict: return "Conflicto";
        case SyncStatus::Error: return "Error";
        default: return "Desconocido";
    }
}

int TaskControllerQml::syncPendingCount() const {
    return controller_->syncPendingCount();
}

bool TaskControllerQml::syncIsOnline() const {
    return controller_->syncIsOnline();
}

void TaskControllerQml::setSyncConfig(const QString& serverUrl, const QString& apiKey, int intervalSeconds, bool autoSync) {
    SyncConfig config;
    config.serverUrl = serverUrl.toStdString();
    config.apiKey = apiKey.toStdString();
    config.syncIntervalSeconds = intervalSeconds;
    config.autoSync = autoSync;
    config.syncOnChange = autoSync;
    controller_->setSyncConfig(config);
}

void TaskControllerQml::syncNow() {
    controller_->syncNow();
}

void TaskControllerQml::setSyncOnline(bool online) {
    controller_->setSyncOnline(online);
}

// --- iCal / Calendario ---

QString TaskControllerQml::exportTasksToICal() const {
    return QString::fromStdString(controller_->exportTasksToICal());
}

QString TaskControllerQml::exportTaskToICal(int taskIndex) const {
    return QString::fromStdString(controller_->exportTaskToICal(taskIndex));
}

void TaskControllerQml::importTasksFromICal(const QString& icsData) {
    controller_->importTasksFromICalAndAdd(icsData.toStdString());
    rebuildTasksCache();
    emit tasksChanged();
    emit taskCountChanged();
}

} // namespace checklist
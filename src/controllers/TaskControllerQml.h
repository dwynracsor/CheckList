#pragma once

#include <QObject>
#include <QQmlListProperty>
#include <QDate>
#include <QString>
#include <QVector>
#include <memory>

#include "controllers/TaskController.h"
#include "controllers/TaskQml.h"
#include "nlohmann/json.hpp"

namespace checklist {

class TaskControllerQml : public QObject {
    Q_OBJECT
    Q_PROPERTY(QQmlListProperty<TaskQml> tasks READ tasks NOTIFY tasksChanged)
    Q_PROPERTY(int taskCount READ taskCount NOTIFY taskCountChanged)
    Q_PROPERTY(bool isLoaded READ isLoaded NOTIFY isLoadedChanged)
    Q_PROPERTY(bool canUndo READ canUndo NOTIFY canUndoChanged)
    Q_PROPERTY(bool canRedo READ canRedo NOTIFY canRedoChanged)
    Q_PROPERTY(QString lastError READ lastError NOTIFY lastErrorChanged)
    Q_PROPERTY(bool autoSave READ autoSave WRITE setAutoSave NOTIFY autoSaveChanged)
    Q_PROPERTY(int autoSaveDelay READ autoSaveDelay WRITE setAutoSaveDelay NOTIFY autoSaveDelayChanged)
    Q_PROPERTY(bool presentationMode READ presentationMode NOTIFY presentationModeChanged)
    Q_PROPERTY(int presentationTaskIndex READ presentationTaskIndex NOTIFY presentationTaskIndexChanged)

public:
    explicit TaskControllerQml(QObject* parent = nullptr);
    explicit TaskControllerQml(const std::string& storagePath, QObject* parent = nullptr);
    ~TaskControllerQml();

    QQmlListProperty<TaskQml> tasks();
    int taskCount() const { return static_cast<int>(controller_->taskCount()); }
    bool isLoaded() const { return controller_->isLoaded(); }
    bool canUndo() const { return controller_->canUndo(); }
    bool canRedo() const { return controller_->canRedo(); }
    QString lastError() const { return QString::fromStdString(controller_->lastError()); }

    bool autoSave() const { return controller_->autoSave(); }
    void setAutoSave(bool value) { controller_->setAutoSave(value); emit autoSaveChanged(); }

    int autoSaveDelay() const { return controller_->autoSaveDelay(); }
    void setAutoSaveDelay(int ms) { controller_->setAutoSaveDelay(ms); emit autoSaveDelayChanged(); }

    bool presentationMode() const { return controller_->presentationMode(); }
    void setPresentationMode(bool enabled) { controller_->setPresentationMode(enabled); emit presentationModeChanged(); }

    int presentationTaskIndex() const { return controller_->presentationTaskIndex(); }
    void setPresentationTaskIndex(int index) { controller_->setPresentationTaskIndex(index); emit presentationTaskIndexChanged(); }

    Q_INVOKABLE void load();
    Q_INVOKABLE bool save();
    Q_INVOKABLE void addTask(const QString& name, const QString& category = "");
    Q_INVOKABLE void removeTask(int index);
    Q_INVOKABLE TaskQml* getTask(int index);
    Q_INVOKABLE void undo();
    Q_INVOKABLE void redo();
    Q_INVOKABLE void clearHistory();
    Q_INVOKABLE void importFromJsonString(const QString& jsonString);
    Q_INVOKABLE void nextPresentationTask();
    Q_INVOKABLE void previousPresentationTask();

    // --- Tiempo y Pomodoro ---
    Q_INVOKABLE void setTaskEstimatedDuration(int taskIndex, int64_t seconds);
    Q_INVOKABLE int64_t getTaskEstimatedDuration(int taskIndex) const;
    Q_INVOKABLE int64_t getTaskActualDuration(int taskIndex) const;
    Q_INVOKABLE void startTaskTiming(int taskIndex);
    Q_INVOKABLE void stopTaskTiming(int taskIndex);
    Q_INVOKABLE bool isTaskTiming(int taskIndex) const;
    Q_INVOKABLE void incrementTaskPomodoro(int taskIndex);
    Q_INVOKABLE void resetTaskPomodoros(int taskIndex);
    Q_INVOKABLE int getTaskPomodoroCount(int taskIndex) const;
    Q_INVOKABLE int64_t getTaskRemainingTime(int taskIndex) const;
    Q_INVOKABLE int getTaskProgressPercentage(int taskIndex) const;

    // --- Dependencias ---
    Q_INVOKABLE void setTaskDependency(int taskIndex, int dependencyType, const QString& dependsOnId);
    Q_INVOKABLE int getTaskDependencyType(int taskIndex) const;
    Q_INVOKABLE QString getTaskDependencyId(int taskIndex) const;
    Q_INVOKABLE void removeTaskDependency(int taskIndex);
    Q_INVOKABLE bool isTaskBlockedByDependency(int taskIndex) const;
    Q_INVOKABLE int findTaskIndexById(const QString& id) const;

    // --- Etiquetas (Tags) ---
    Q_INVOKABLE void addTaskTag(int taskIndex, const QString& tag);
    Q_INVOKABLE void removeTaskTag(int taskIndex, const QString& tag);
    Q_INVOKABLE void clearTaskTags(int taskIndex);
    Q_INVOKABLE void setTaskTags(int taskIndex, const QStringList& tags);
    Q_INVOKABLE QStringList getTaskTags(int taskIndex) const;
    Q_INVOKABLE int getTaskTagCount(int taskIndex) const;
    Q_INVOKABLE bool taskHasTag(int taskIndex, const QString& tag) const;
    Q_INVOKABLE QStringList getAllTags() const;

    // --- Plantillas de tareas ---
    Q_INVOKABLE int templateCount() const;
    Q_INVOKABLE QString getTemplateName(int index) const;
    Q_INVOKABLE QString getTemplateId(int index) const;
    Q_INVOKABLE int getTemplateRecurrence(int index) const;
    Q_INVOKABLE int getTemplateItemCount(int index) const;
    Q_INVOKABLE QString getTemplateItemText(int templateIndex, int itemIndex) const;
    Q_INVOKABLE bool getTemplateItemDone(int templateIndex, int itemIndex) const;
    Q_INVOKABLE void addTemplate(const QString& name, const QStringList& items);
    Q_INVOKABLE void removeTemplate(const QString& templateId);
    Q_INVOKABLE void createTaskFromTemplate(const QString& templateId, const QString& taskName);
    Q_INVOKABLE QStringList getAllTemplateIds() const;
    Q_INVOKABLE int findTemplateIndexById(const QString& templateId) const;

    // --- Estadísticas de productividad ---
    Q_INVOKABLE QString getWeeklyReportJson(int days = 7) const;
    Q_INVOKABLE QString getDailyStatsJson(int days = 7) const;
    Q_INVOKABLE QString getProductivityStreakJson() const;
    Q_INVOKABLE QString getCategoryTimeDistributionJson() const;

    // --- Adjuntos/Archivos ---
    Q_INVOKABLE void addTaskAttachment(int taskIndex, const QString& fileName, const QString& filePath, const QString& fileType, int64_t fileSize);
    Q_INVOKABLE void removeTaskAttachment(int taskIndex, const QString& attachmentId);
    Q_INVOKABLE void clearTaskAttachments(int taskIndex);
    Q_INVOKABLE QStringList getTaskAttachmentFileNames(int taskIndex) const;
    Q_INVOKABLE QString getTaskAttachmentFilePath(int taskIndex, int attachmentIndex) const;
    Q_INVOKABLE QString getTaskAttachmentFileType(int taskIndex, int attachmentIndex) const;
    Q_INVOKABLE int64_t getTaskAttachmentFileSize(int taskIndex, int attachmentIndex) const;
    Q_INVOKABLE int getTaskAttachmentCount(int taskIndex) const;

    // --- Sync offline-first ---
    Q_INVOKABLE QString syncStatusString() const;
    Q_INVOKABLE int syncPendingCount() const;
    Q_INVOKABLE bool syncIsOnline() const;
    Q_INVOKABLE void setSyncConfig(const QString& serverUrl, const QString& apiKey, int intervalSeconds, bool autoSync);
    Q_INVOKABLE void syncNow();
    Q_INVOKABLE void setSyncOnline(bool online);

    // --- iCal / Calendario ---
    Q_INVOKABLE QString exportTasksToICal() const;
    Q_INVOKABLE QString exportTaskToICal(int taskIndex) const;
    Q_INVOKABLE void importTasksFromICal(const QString& icsData);

signals:
    void tasksChanged();
    void taskCountChanged();
    void isLoadedChanged();
    void canUndoChanged();
    void canRedoChanged();
    void lastErrorChanged();
    void autoSaveChanged();
    void autoSaveDelayChanged();
    void presentationModeChanged();
    void presentationTaskIndexChanged();
    void taskAdded(int index);
    void taskRemoved(int index);
    void saved();
    void templatesChanged();

private:
    std::shared_ptr<TaskController> controller_;
    QVector<TaskQml*> tasksCache_;

    void rebuildTasksCache();
};

} // namespace checklist
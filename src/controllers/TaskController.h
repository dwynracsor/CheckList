#ifndef TASKCONTROLLER_H
#define TASKCONTROLLER_H

#include <QObject>
#include <QList>
#include <QTimer>
#include <QQmlListProperty>

class Task;
class Correction;
class JsonStorage;

class TaskController : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QQmlListProperty<Task> tasks READ tasks NOTIFY tasksChanged)
    Q_PROPERTY(int taskCount READ taskCount NOTIFY tasksChanged)
    Q_PROPERTY(bool isLoaded READ isLoaded NOTIFY loadedChanged)
    Q_PROPERTY(QString lastError READ lastError NOTIFY errorChanged)
    Q_PROPERTY(bool autoSave READ autoSave WRITE setAutoSave NOTIFY autoSaveChanged)
    Q_PROPERTY(int autoSaveDelay READ autoSaveDelay WRITE setAutoSaveDelay NOTIFY autoSaveDelayChanged)

public:
    explicit TaskController(QObject *parent = nullptr);
    explicit TaskController(const QString &storagePath, QObject *parent = nullptr);
    ~TaskController() override;

    QQmlListProperty<Task> tasks();
    int taskCount() const;
    bool isLoaded() const;
    QString lastError() const;
    bool autoSave() const;
    void setAutoSave(bool enabled);
    int autoSaveDelay() const;
    void setAutoSaveDelay(int ms);

    // Q_INVOKABLE methods for QML
    Q_INVOKABLE void load();
    Q_INVOKABLE void save();
    Q_INVOKABLE Task* addTask(const QString &name);
    Q_INVOKABLE void removeTask(int index);
    Q_INVOKABLE Task* getTask(int index) const;

    // Item operations on task
    Q_INVOKABLE void addItemToTask(int taskIndex, const QString &text);
    Q_INVOKABLE void removeItemFromTask(int taskIndex, int itemIndex);
    Q_INVOKABLE void toggleItemInTask(int taskIndex, int itemIndex, bool done);

    // Correction operations on task
    Q_INVOKABLE Correction* addCorrectionToTask(int taskIndex, const QString &name);
    Q_INVOKABLE void removeCorrectionFromTask(int taskIndex, int correctionIndex);

    // Item operations on correction
    Q_INVOKABLE void addItemToCorrection(int taskIndex, int correctionIndex, const QString &text);
    Q_INVOKABLE void removeItemFromCorrection(int taskIndex, int correctionIndex, int itemIndex);
    Q_INVOKABLE void toggleItemInCorrection(int taskIndex, int correctionIndex, int itemIndex, bool done);

    // Name editing
    Q_INVOKABLE void setTaskName(int taskIndex, const QString &name);
    Q_INVOKABLE void setCorrectionName(int taskIndex, int correctionIndex, const QString &name);

    // Load from web format (for migration)
    Q_INVOKABLE void importFromJsonString(const QString &jsonString);

signals:
    void tasksChanged();
    void loadedChanged();
    void errorChanged();
    void autoSaveChanged();
    void autoSaveDelayChanged();
    void taskAdded(int index);
    void taskRemoved(int index);
    void saved();

private slots:
    void onAutoSaveTimer();

private:
    static void tasksAppend(QQmlListProperty<Task> *list, Task *task);
    static qsizetype tasksCount(QQmlListProperty<Task> *list);
    static Task* tasksAt(QQmlListProperty<Task> *list, qsizetype index);
    static void tasksClear(QQmlListProperty<Task> *list);

    void scheduleAutoSave();
    void setError(const QString &message);

    QList<Task*> m_tasks;
    JsonStorage *m_storage;
    QTimer *m_autoSaveTimer;
    bool m_isLoaded;
    bool m_autoSave;
    int m_autoSaveDelay;
    QString m_lastError;
};

#endif // TASKCONTROLLER_H

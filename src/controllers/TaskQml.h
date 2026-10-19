#pragma once

#include <QObject>
#include <QQmlListProperty>
#include <QDate>
#include <QString>
#include <QVector>
#include <memory>

#include "controllers/TaskController.h"
#include "domain/Task.h"

namespace checklist {

class TaskItemQml : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString id READ id CONSTANT)
    Q_PROPERTY(QString text READ text NOTIFY textChanged)
    Q_PROPERTY(bool done READ done NOTIFY doneChanged)

public:
    explicit TaskItemQml(std::shared_ptr<ChecklistItem> item, QObject* parent = nullptr);

    QString id() const { return QString::fromStdString(item_->id()); }
    QString text() const { return QString::fromStdString(item_->text()); }
    bool done() const { return item_->done(); }

    Q_INVOKABLE void setText(const QString& text);
    Q_INVOKABLE void setDone(bool done);
    Q_INVOKABLE void toggle();

signals:
    void textChanged();
    void doneChanged();

private:
    std::shared_ptr<ChecklistItem> item_;
};

class CorrectionQml : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString id READ id CONSTANT)
    Q_PROPERTY(QString name READ name NOTIFY nameChanged)
    Q_PROPERTY(QDate date READ date CONSTANT)
    Q_PROPERTY(QQmlListProperty<TaskItemQml> items READ items NOTIFY itemsChanged)
    Q_PROPERTY(int itemCount READ itemCount NOTIFY itemCountChanged)

public:
    explicit CorrectionQml(std::shared_ptr<Correction> correction, QObject* parent = nullptr);

    QString id() const { return QString::fromStdString(correction_->id()); }
    QString name() const { return QString::fromStdString(correction_->name()); }
    QDate date() const { return QDate(correction_->date().year, correction_->date().month, correction_->date().day); }
    int itemCount() const { return static_cast<int>(correction_->itemCount()); }

    QQmlListProperty<TaskItemQml> items();

    Q_INVOKABLE void setName(const QString& name);
    Q_INVOKABLE void addItem(const QString& text);
    Q_INVOKABLE void removeItem(int index);
    Q_INVOKABLE void toggleItem(int index);

signals:
    void nameChanged();
    void itemsChanged();
    void itemCountChanged();

private:
    std::shared_ptr<Correction> correction_;
    QVector<TaskItemQml*> itemsCache_;
};

class TaskQml : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString id READ id CONSTANT)
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QString category READ category WRITE setCategory NOTIFY categoryChanged)
    Q_PROPERTY(QDate date READ date CONSTANT)
    Q_PROPERTY(QQmlListProperty<TaskItemQml> items READ items NOTIFY itemsChanged)
    Q_PROPERTY(QQmlListProperty<CorrectionQml> corrections READ corrections NOTIFY correctionsChanged)
    Q_PROPERTY(int itemCount READ itemCount NOTIFY itemCountChanged)
    Q_PROPERTY(int correctionCount READ correctionCount NOTIFY correctionCountChanged)
    Q_PROPERTY(bool isFinished READ isFinished NOTIFY isFinishedChanged)
    Q_PROPERTY(qint64 estimatedDurationSeconds READ estimatedDurationSeconds NOTIFY durationChanged)
    Q_PROPERTY(qint64 actualDurationSeconds READ actualDurationSeconds NOTIFY durationChanged)
    Q_PROPERTY(qint64 remainingTimeSeconds READ remainingTimeSeconds NOTIFY durationChanged)
    Q_PROPERTY(int pomodoroCount READ pomodoroCount NOTIFY pomodoroChanged)
    Q_PROPERTY(bool isTiming READ isTiming NOTIFY timingStateChanged)
    Q_PROPERTY(int progressPercentage READ progressPercentage NOTIFY progressChanged)
    Q_PROPERTY(int dependencyType READ dependencyType NOTIFY dependencyChanged)
    Q_PROPERTY(QString dependencyId READ dependencyId NOTIFY dependencyChanged)
    Q_PROPERTY(bool isBlockedByDependency READ isBlockedByDependency NOTIFY dependencyChanged)
    Q_PROPERTY(QStringList tags READ tags NOTIFY tagsChanged)

public:
    explicit TaskQml(std::shared_ptr<Task> task, QObject* parent = nullptr);

    QString id() const { return QString::fromStdString(task_->id()); }
    QString name() const { return QString::fromStdString(task_->name()); }
    QString category() const { return QString::fromStdString(task_->category()); }
    QDate date() const { return QDate(task_->date().year, task_->date().month, task_->date().day); }
    int itemCount() const { return static_cast<int>(task_->itemCount()); }
    int correctionCount() const { return static_cast<int>(task_->correctionCount()); }
    bool isFinished() const { return task_->isFinished(); }

    qint64 estimatedDurationSeconds() const { return task_->estimatedDurationSeconds(); }
    qint64 actualDurationSeconds() const { return task_->actualDurationSeconds(); }
    qint64 remainingTimeSeconds() const { return task_->remainingTimeSeconds(); }
    int pomodoroCount() const { return task_->pomodoroCount(); }
    bool isTiming() const { return task_->isTiming(); }
    int progressPercentage() const { return task_->progressPercentage(); }

    int dependencyType() const { return static_cast<int>(task_->dependencyType()); }
    QString dependencyId() const { return QString::fromStdString(task_->dependencyId()); }
    bool isBlockedByDependency() const { return task_->dependencyType() != TaskDependency::None && !task_->dependencyId().empty(); }

    QStringList tags() const;

    QQmlListProperty<TaskItemQml> items();
    QQmlListProperty<CorrectionQml> corrections();

    Q_INVOKABLE void setName(const QString& name);
    Q_INVOKABLE void setCategory(const QString& category);
    Q_INVOKABLE void addItem(const QString& text);
    Q_INVOKABLE void removeItem(int index);
    Q_INVOKABLE void toggleItem(int index, bool done);
    Q_INVOKABLE void setItemText(int index, const QString& text);
    Q_INVOKABLE void addCorrection(const QString& name);
    Q_INVOKABLE void removeCorrection(int index);
    Q_INVOKABLE void setCorrectionName(int correctionIndex, const QString& name);
    Q_INVOKABLE void setCorrectionItemText(int correctionIndex, int itemIndex, const QString& text);

    // --- Tiempo y Pomodoro ---
    Q_INVOKABLE void setEstimatedDuration(qint64 seconds);
    Q_INVOKABLE void startTiming();
    Q_INVOKABLE void stopTiming();
    Q_INVOKABLE void incrementPomodoro();
    Q_INVOKABLE void resetPomodoros();

    // --- Dependencias ---
    Q_INVOKABLE void setDependency(int dependencyType, const QString& dependsOnId);
    Q_INVOKABLE void removeDependency();

    // --- Etiquetas ---
    Q_INVOKABLE void addTag(const QString& tag);
    Q_INVOKABLE void removeTag(const QString& tag);
    Q_INVOKABLE void clearTags();
    Q_INVOKABLE void setTags(const QStringList& tags);
    Q_INVOKABLE int tagCount() const;
    Q_INVOKABLE bool hasTag(const QString& tag) const;

    // --- Adjuntos ---
    Q_INVOKABLE int attachmentCount() const;
    Q_INVOKABLE QString attachmentFileName(int index) const;
    Q_INVOKABLE QString attachmentFilePath(int index) const;
    Q_INVOKABLE QString attachmentFileType(int index) const;
    Q_INVOKABLE int64_t attachmentFileSize(int index) const;
    Q_INVOKABLE void addAttachment(const QString& fileName, const QString& filePath, const QString& fileType, int64_t fileSize);
    Q_INVOKABLE void removeAttachment(int index);
    Q_INVOKABLE void clearAttachments();

signals:
    void nameChanged();
    void categoryChanged();
    void itemsChanged();
    void correctionsChanged();
    void itemCountChanged();
    void correctionCountChanged();
    void isFinishedChanged();
    void durationChanged();
    void pomodoroChanged();
    void timingStateChanged();
    void progressChanged();
    void dependencyChanged();
    void tagsChanged();
    void attachmentChanged();

private:
    std::shared_ptr<Task> task_;
    QVector<TaskItemQml*> itemsCache_;
    QVector<CorrectionQml*> correctionsCache_;
};

} // namespace checklist
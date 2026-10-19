#include "controllers/TaskQml.h"
#include <QQmlListProperty>

namespace checklist {

// --- TaskItemQml ---

TaskItemQml::TaskItemQml(std::shared_ptr<ChecklistItem> item, QObject* parent)
    : QObject(parent), item_(std::move(item)) {}

void TaskItemQml::setText(const QString& text) {
    item_->setText(text.toStdString());
    emit textChanged();
}

void TaskItemQml::setDone(bool done) {
    item_->setDone(done);
    emit doneChanged();
}

void TaskItemQml::toggle() {
    item_->toggle();
    emit doneChanged();
}

// --- CorrectionQml ---

CorrectionQml::CorrectionQml(std::shared_ptr<Correction> correction, QObject* parent)
    : QObject(parent), correction_(std::move(correction)) {}

QQmlListProperty<TaskItemQml> CorrectionQml::items() {
    return QQmlListProperty<TaskItemQml>(this, &itemsCache_);
}

void CorrectionQml::setName(const QString& name) {
    correction_->setName(name.toStdString());
    emit nameChanged();
}

void CorrectionQml::addItem(const QString& text) {
    correction_->addNewItem(text.toStdString());
    emit itemsChanged();
    emit itemCountChanged();
}

void CorrectionQml::removeItem(int index) {
    correction_->removeItem(static_cast<size_t>(index));
    emit itemsChanged();
    emit itemCountChanged();
}

void CorrectionQml::toggleItem(int index) {
    correction_->toggleItem(static_cast<size_t>(index));
    emit itemCountChanged();
}

// --- TaskQml ---

TaskQml::TaskQml(std::shared_ptr<Task> task, QObject* parent)
    : QObject(parent), task_(std::move(task)) {
    // Conectar callbacks de Task para emitir señales QML
    task_->setDurationChangedCallback([this]() {
        emit durationChanged();
        emit progressChanged();
    });
    task_->setPomodoroChangedCallback([this]() {
        emit pomodoroChanged();
    });
    task_->setTimingStateChangedCallback([this]() {
        emit timingStateChanged();
        emit durationChanged();
        emit progressChanged();
    });
    task_->setTagsChangedCallback([this]() {
        emit tagsChanged();
    });
    task_->setAttachmentChangedCallback([this]() {
        emit attachmentChanged();
    });
}

QQmlListProperty<TaskItemQml> TaskQml::items() {
    return QQmlListProperty<TaskItemQml>(this, &itemsCache_);
}

QQmlListProperty<CorrectionQml> TaskQml::corrections() {
    return QQmlListProperty<CorrectionQml>(this, &correctionsCache_);
}

void TaskQml::setName(const QString& name) {
    task_->setName(name.toStdString());
    emit nameChanged();
}

void TaskQml::setCategory(const QString& category) {
    task_->setCategory(category.toStdString());
    emit categoryChanged();
}

void TaskQml::addItem(const QString& text) {
    task_->addItem(std::make_shared<ChecklistItem>(text.toStdString(), false));
    emit itemsChanged();
    emit itemCountChanged();
    emit isFinishedChanged();
}

void TaskQml::removeItem(int index) {
    task_->removeItem(static_cast<size_t>(index));
    emit itemsChanged();
    emit itemCountChanged();
    emit isFinishedChanged();
}

void TaskQml::toggleItem(int index, bool done) {
    task_->getItem(static_cast<size_t>(index))->setDone(done);
    emit itemsChanged();
    emit isFinishedChanged();
}

void TaskQml::setItemText(int index, const QString& text) {
    auto item = task_->getItem(static_cast<size_t>(index));
    if (item) item->setText(text.toStdString());
    emit itemsChanged();
}

void TaskQml::addCorrection(const QString& name) {
    task_->addCorrection(std::make_shared<Correction>(name.toStdString(), Date::today()));
    emit correctionsChanged();
    emit correctionCountChanged();
}

void TaskQml::removeCorrection(int index) {
    task_->removeCorrection(static_cast<size_t>(index));
    emit correctionsChanged();
    emit correctionCountChanged();
}

void TaskQml::setCorrectionName(int correctionIndex, const QString& name) {
    auto corr = task_->getCorrection(static_cast<size_t>(correctionIndex));
    if (corr) corr->setName(name.toStdString());
}

void TaskQml::setCorrectionItemText(int correctionIndex, int itemIndex, const QString& text) {
    auto corr = task_->getCorrection(static_cast<size_t>(correctionIndex));
    if (corr) {
        auto item = corr->getItem(static_cast<size_t>(itemIndex));
        if (item) item->setText(text.toStdString());
    }
}

// --- Tiempo y Pomodoro ---

void TaskQml::setEstimatedDuration(qint64 seconds) {
    task_->setEstimatedDurationSeconds(static_cast<int64_t>(seconds));
    emit durationChanged();
    emit progressChanged();
}

void TaskQml::startTiming() {
    task_->startTiming();
    emit timingStateChanged();
    emit durationChanged();
    emit progressChanged();
}

void TaskQml::stopTiming() {
    task_->stopTiming();
    emit timingStateChanged();
    emit durationChanged();
    emit progressChanged();
}

void TaskQml::incrementPomodoro() {
    task_->incrementPomodoro();
    emit pomodoroChanged();
}

void TaskQml::resetPomodoros() {
    task_->resetPomodoros();
    emit pomodoroChanged();
}

// --- Dependencias ---

void TaskQml::setDependency(int dependencyType, const QString& dependsOnId) {
    task_->setDependency(static_cast<TaskDependency>(dependencyType), dependsOnId.toStdString());
    emit dependencyChanged();
}

void TaskQml::removeDependency() {
    task_->setDependency(TaskDependency::None, "");
    emit dependencyChanged();
}

// --- Etiquetas ---

QStringList TaskQml::tags() const {
    QStringList qTags;
    const auto& stdTags = task_->tags();
    for (const auto& tag : stdTags) {
        qTags << QString::fromStdString(tag);
    }
    return qTags;
}

void TaskQml::addTag(const QString& tag) {
    task_->addTag(tag.toStdString());
    emit tagsChanged();
}

void TaskQml::removeTag(const QString& tag) {
    task_->removeTag(tag.toStdString());
    emit tagsChanged();
}

void TaskQml::clearTags() {
    task_->clearTags();
    emit tagsChanged();
}

void TaskQml::setTags(const QStringList& tags) {
    std::vector<std::string> stdTags;
    for (const auto& tag : tags) {
        stdTags.push_back(tag.toStdString());
    }
    task_->setTags(stdTags);
    emit tagsChanged();
}

int TaskQml::tagCount() const {
    return static_cast<int>(task_->tagCount());
}

bool TaskQml::hasTag(const QString& tag) const {
    return task_->hasTag(tag.toStdString());
}

} // namespace checklist
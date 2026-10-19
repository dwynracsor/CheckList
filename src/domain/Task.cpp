#include "Task.h"
#include "ChecklistItem.h"
#include "Correction.h"
#include <algorithm>
#include <sstream>
#include <chrono>
#include <cmath>

namespace checklist {

Task::Task()
    : id_("task_" + std::to_string(std::hash<std::string>{}("default"))),
      name_(""), date_(Date::today()), category_(""),
      estimatedDurationSeconds_(0), actualDurationSeconds_(0),
      pomodoroCount_(0), isTiming_(false),
      sessionStartTime_(std::chrono::system_clock::now()),
      dependencyType_(TaskDependency::None), dependencyId_("") {}

Task::Task(const std::string& name, const Date& date)
    : Task(generateId(), name, date) {}

Task::Task(const std::string& id, const std::string& name, const Date& date)
    : id_(id), name_(name), date_(date), category_(""),
      estimatedDurationSeconds_(0), actualDurationSeconds_(0),
      pomodoroCount_(0), isTiming_(false),
      sessionStartTime_(std::chrono::system_clock::now()),
      dependencyType_(TaskDependency::None), dependencyId_("") {}

Task::~Task() = default;

Task::Task(const Task& other)
    : id_(other.id_), name_(other.name_), date_(other.date_), category_(other.category_),
      estimatedDurationSeconds_(other.estimatedDurationSeconds_),
      actualDurationSeconds_(other.actualDurationSeconds_),
      pomodoroCount_(other.pomodoroCount_),
      isTiming_(other.isTiming_.load()),
      sessionStartTime_(other.sessionStartTime_),
      dependencyType_(other.dependencyType_),
      dependencyId_(other.dependencyId_),
      tags_(other.tags_),
      attachments_(other.attachments_) {
    for (const auto& item : other.items_) {
        if (item) {
            items_.push_back(std::make_shared<ChecklistItem>(*item));
        }
    }
    for (const auto& corr : other.corrections_) {
        if (corr) {
            corrections_.push_back(std::make_shared<Correction>(*corr));
        }
    }
}

Task& Task::operator=(const Task& other) {
    if (this != &other) {
        id_ = other.id_;
        name_ = other.name_;
        date_ = other.date_;
        category_ = other.category_;
        estimatedDurationSeconds_ = other.estimatedDurationSeconds_;
        actualDurationSeconds_ = other.actualDurationSeconds_;
        pomodoroCount_ = other.pomodoroCount_;
        isTiming_ = other.isTiming_.load();
        sessionStartTime_ = other.sessionStartTime_;
        dependencyType_ = other.dependencyType_;
        dependencyId_ = other.dependencyId_;
        tags_ = other.tags_;
        attachments_ = other.attachments_;

        items_.clear();
        for (const auto& item : other.items_) {
            if (item) {
                items_.push_back(std::make_shared<ChecklistItem>(*item));
            }
        }

        corrections_.clear();
        for (const auto& corr : other.corrections_) {
            if (corr) {
                corrections_.push_back(std::make_shared<Correction>(*corr));
            }
        }
    }
    return *this;
}

Task::Task(Task&& other) noexcept
    : id_(std::move(other.id_)), name_(std::move(other.name_)),
      date_(std::move(other.date_)), category_(std::move(other.category_)),
      estimatedDurationSeconds_(other.estimatedDurationSeconds_),
      actualDurationSeconds_(other.actualDurationSeconds_),
      pomodoroCount_(other.pomodoroCount_),
      isTiming_(other.isTiming_.exchange(false)),
      sessionStartTime_(other.sessionStartTime_),
      dependencyType_(other.dependencyType_),
      dependencyId_(std::move(other.dependencyId_)),
      tags_(std::move(other.tags_)),
      attachments_(std::move(other.attachments_)) {
    items_ = std::move(other.items_);
    corrections_ = std::move(other.corrections_);
}

Task& Task::operator=(Task&& other) noexcept {
    if (this != &other) {
        id_ = std::move(other.id_);
        name_ = std::move(other.name_);
        date_ = std::move(other.date_);
        category_ = std::move(other.category_);
        estimatedDurationSeconds_ = other.estimatedDurationSeconds_;
        actualDurationSeconds_ = other.actualDurationSeconds_;
        pomodoroCount_ = other.pomodoroCount_;
        isTiming_ = other.isTiming_.exchange(false);
        sessionStartTime_ = other.sessionStartTime_;
        dependencyType_ = other.dependencyType_;
        dependencyId_ = std::move(other.dependencyId_);
        tags_ = std::move(other.tags_);
        attachments_ = std::move(other.attachments_);
        items_ = std::move(other.items_);
        corrections_ = std::move(other.corrections_);
    }
    return *this;
}

ChecklistItem* Task::getItem(size_t index) {
    if (index < items_.size()) {
        return items_[index].get();
    }
    return nullptr;
}

const ChecklistItem* Task::getItem(size_t index) const {
    if (index < items_.size()) {
        return items_[index].get();
    }
    return nullptr;
}

Correction* Task::getCorrection(size_t index) {
    if (index < corrections_.size()) {
        return corrections_[index].get();
    }
    return nullptr;
}

const Correction* Task::getCorrection(size_t index) const {
    if (index < corrections_.size()) {
        return corrections_[index].get();
    }
    return nullptr;
}

void Task::addItem(const std::shared_ptr<ChecklistItem>& item) {
    if (item) {
        items_.push_back(item);
        invokeItemCountChanged();
        invokeItemsChanged();
    }
}

void Task::removeItem(size_t index) {
    if (index < items_.size()) {
        items_.erase(items_.begin() + static_cast<long>(index));
        invokeItemCountChanged();
        invokeItemsChanged();
    }
}

std::shared_ptr<Correction> Task::addCorrection(const std::shared_ptr<Correction>& correction) {
    if (correction) {
        corrections_.push_back(correction);
        invokeCorrectionCountChanged();
        invokeCorrectionsChanged();
        return correction;
    }
    return nullptr;
}

void Task::removeCorrection(size_t index) {
    if (index < corrections_.size()) {
        corrections_.erase(corrections_.begin() + static_cast<long>(index));
        invokeCorrectionCountChanged();
        invokeCorrectionsChanged();
    }
}

void Task::setName(const std::string& name) {
    if (name_ != name) {
        name_ = name;
        invokeNameChanged();
    }
}

void Task::setCategory(const std::string& category) {
    if (category_ != category) {
        category_ = category;
        invokeCategoryChanged();
    }
}

bool Task::isFinished() const {
    if (items_.empty()) return false;
    return std::all_of(items_.begin(), items_.end(),
        [](const std::shared_ptr<ChecklistItem>& item) { return item && item->done(); });
}

// --- Tiempo y Pomodoro ---

void Task::setEstimatedDurationSeconds(int64_t seconds) {
    if (seconds < 0) seconds = 0;
    if (estimatedDurationSeconds_ != seconds) {
        estimatedDurationSeconds_ = seconds;
        invokeDurationChanged();
    }
}

void Task::incrementPomodoro() {
    pomodoroCount_++;
    invokePomodoroChanged();
}

void Task::resetPomodoros() {
    if (pomodoroCount_ != 0) {
        pomodoroCount_ = 0;
        invokePomodoroChanged();
    }
}

void Task::startTiming() {
    if (!isTiming_) {
        isTiming_ = true;
        sessionStartTime_ = std::chrono::system_clock::now();
        invokeTimingStateChanged();
    }
}

void Task::stopTiming() {
    if (isTiming_) {
        auto now = std::chrono::system_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - sessionStartTime_).count();
        if (elapsed > 0) {
            actualDurationSeconds_ += elapsed;
            invokeDurationChanged();
        }
        isTiming_ = false;
        invokeTimingStateChanged();
    }
}

int64_t Task::remainingTimeSeconds() const {
    if (estimatedDurationSeconds_ <= 0) return -1; // No estimation set
    auto remaining = estimatedDurationSeconds_ - actualDurationSeconds_;
    if (isTiming_) {
        auto now = std::chrono::system_clock::now();
        auto sessionElapsed = std::chrono::duration_cast<std::chrono::seconds>(
            now - sessionStartTime_).count();
        remaining -= sessionElapsed;
    }
    return remaining > 0 ? remaining : 0;
}

int Task::progressPercentage() const {
    if (estimatedDurationSeconds_ <= 0) return 0;
    int64_t total = actualDurationSeconds_;
    if (isTiming_) {
        auto now = std::chrono::system_clock::now();
        auto sessionElapsed = std::chrono::duration_cast<std::chrono::seconds>(
            now - sessionStartTime_).count();
        total += sessionElapsed;
    }
    if (total <= 0) return 0;
    auto pct = static_cast<double>(total) / static_cast<double>(estimatedDurationSeconds_) * 100.0;
    if (pct > 100.0) return 100;
    return static_cast<int>(std::round(pct));
}

// --- Dependencias entre tareas ---

void Task::setDependency(TaskDependency type, const std::string& dependsOnId) {
    if (dependencyType_ != type || dependencyId_ != dependsOnId) {
        dependencyType_ = type;
        dependencyId_ = dependsOnId;
        invokeDependencyChanged();
    }
}

bool Task::isBlockedByDependency(const std::shared_ptr<Task>& dependencyTask) const {
    if (dependencyType_ == TaskDependency::None || dependencyId_.empty()) {
        return false; // No hay dependencia
    }
    if (!dependencyTask) {
        return true; // La tarea dependiente no existe, está bloqueada
    }
    if (!dependencyTask->isFinished()) {
        return true; // La dependencia no está completada
    }
    return false; // Dependencia cumplida
}

bool Task::canComplete() const {
    return dependencyType_ == TaskDependency::None || dependencyId_.empty();
}

// --- Etiquetas (Tags) ---

bool Task::hasTag(const std::string& tag) const {
    return std::find(tags_.begin(), tags_.end(), tag) != tags_.end();
}

void Task::addTag(const std::string& tag) {
    if (tag.empty()) return;
    if (!hasTag(tag)) {
        tags_.push_back(tag);
        invokeTagsChanged();
    }
}

void Task::removeTag(const std::string& tag) {
    auto it = std::find(tags_.begin(), tags_.end(), tag);
    if (it != tags_.end()) {
        tags_.erase(it);
        invokeTagsChanged();
    }
}

void Task::clearTags() {
    if (!tags_.empty()) {
        tags_.clear();
        invokeTagsChanged();
    }
}

void Task::setTags(const std::vector<std::string>& newTags) {
    if (tags_ != newTags) {
        tags_ = newTags;
        invokeTagsChanged();
    }
}

void Task::invokeNameChanged() const {
    if (nameChangedCb_) nameChangedCb_();
}

void Task::invokeCategoryChanged() const {
    if (categoryChangedCb_) categoryChangedCb_();
}

void Task::invokeItemCountChanged() const {
    if (itemCountChangedCb_) itemCountChangedCb_();
}

void Task::invokeCorrectionCountChanged() const {
    if (correctionCountChangedCb_) correctionCountChangedCb_();
}

void Task::invokeItemsChanged() const {
    if (itemsChangedCb_) itemsChangedCb_();
}

void Task::invokeCorrectionsChanged() const {
    if (correctionsChangedCb_) correctionsChangedCb_();
}

void Task::invokeDurationChanged() const {
    if (durationChangedCb_) durationChangedCb_();
}

void Task::invokePomodoroChanged() const {
    if (pomodoroChangedCb_) pomodoroChangedCb_();
}

void Task::invokeTimingStateChanged() const {
    if (timingStateChangedCb_) timingStateChangedCb_();
}

void Task::invokeDependencyChanged() const {
    if (dependencyChangedCb_) dependencyChangedCb_();
}

void Task::invokeTagsChanged() const {
    if (tagsChangedCb_) tagsChangedCb_();
}

std::vector<std::string> Task::tokenize(const std::string& text) {
    std::vector<std::string> tokens;
    if (text.empty()) return tokens;

    std::string word;
    for (char c : text) {
        if (std::isalnum(static_cast<unsigned char>(c)) || c == '_') {
            word += c;
        } else {
            if (!word.empty()) {
                tokens.push_back(word);
                word.clear();
            }
        }
    }
    if (!word.empty()) {
        tokens.push_back(word);
    }
    return tokens;
}

std::string Task::generateId() {
    auto now = std::chrono::system_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()).count();
    std::ostringstream oss;
    oss << "task_" << ms;
    return oss.str();
}

// --- Adjuntos ---

const TaskAttachment* Task::getAttachment(size_t index) const {
    if (index < attachments_.size()) {
        return &attachments_[index];
    }
    return nullptr;
}

void Task::addAttachment(const TaskAttachment& attachment) {
    attachments_.push_back(attachment);
    invokeAttachmentChanged();
}

void Task::removeAttachment(const std::string& attachmentId) {
    auto it = std::find_if(attachments_.begin(), attachments_.end(),
        [&](const TaskAttachment& a) { return a.id == attachmentId; });
    if (it != attachments_.end()) {
        attachments_.erase(it);
        invokeAttachmentChanged();
    }
}

void Task::clearAttachments() {
    if (!attachments_.empty()) {
        attachments_.clear();
        invokeAttachmentChanged();
    }
}

void Task::setAttachments(const std::vector<TaskAttachment>& newAttachments) {
    if (attachments_ != newAttachments) {
        attachments_ = newAttachments;
        invokeAttachmentChanged();
    }
}

void Task::invokeAttachmentChanged() const {
    if (attachmentChangedCb_) attachmentChangedCb_();
}

} // namespace checklist
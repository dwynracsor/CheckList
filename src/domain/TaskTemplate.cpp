#include "TaskTemplate.h"
#include "Task.h"
#include "ChecklistItem.h"
#include <algorithm>
#include <sstream>
#include <chrono>

namespace checklist {

TaskTemplate::TaskTemplate()
    : id_(generateId()),
      name_(""),
      recurrence_(TemplateRecurrence::None),
      recurrenceInterval_(1) {}

TaskTemplate::TaskTemplate(const std::string& name, const std::vector<TaskTemplateItem>& items)
    : id_(generateId()),
      name_(name),
      items_(items),
      recurrence_(TemplateRecurrence::None),
      recurrenceInterval_(1) {}

TaskTemplate::TaskTemplate(const std::string& id, const std::string& name,
                           const std::vector<TaskTemplateItem>& items,
                           TemplateRecurrence recurrence)
    : id_(id),
      name_(name),
      items_(items),
      recurrence_(recurrence),
      recurrenceInterval_(1) {}

TaskTemplate::~TaskTemplate() = default;

TaskTemplate::TaskTemplate(const TaskTemplate& other)
    : id_(other.id_), name_(other.name_), items_(other.items_),
      recurrence_(other.recurrence_), recurrenceInterval_(other.recurrenceInterval_) {}

TaskTemplate& TaskTemplate::operator=(const TaskTemplate& other) {
    if (this != &other) {
        id_ = other.id_;
        name_ = other.name_;
        items_ = other.items_;
        recurrence_ = other.recurrence_;
        recurrenceInterval_ = other.recurrenceInterval_;
    }
    return *this;
}

TaskTemplate::TaskTemplate(TaskTemplate&& other) noexcept
    : id_(std::move(other.id_)), name_(std::move(other.name_)),
      items_(std::move(other.items_)),
      recurrence_(other.recurrence_),
      recurrenceInterval_(other.recurrenceInterval_) {}

TaskTemplate& TaskTemplate::operator=(TaskTemplate&& other) noexcept {
    if (this != &other) {
        id_ = std::move(other.id_);
        name_ = std::move(other.name_);
        items_ = std::move(other.items_);
        recurrence_ = other.recurrence_;
        recurrenceInterval_ = other.recurrenceInterval_;
    }
    return *this;
}

void TaskTemplate::setName(const std::string& name) {
    if (name_ != name) {
        name_ = name;
        invokeNameChanged();
    }
}

void TaskTemplate::setItems(const std::vector<TaskTemplateItem>& items) {
    if (items_ != items) {
        items_ = items;
        invokeItemsChanged();
    }
}

void TaskTemplate::setRecurrence(TemplateRecurrence recurrence, int interval) {
    if (recurrence_ != recurrence || recurrenceInterval_ != interval) {
        recurrence_ = recurrence;
        recurrenceInterval_ = interval > 0 ? interval : 1;
        invokeRecurrenceChanged();
    }
}

std::shared_ptr<Task> TaskTemplate::instantiate(const std::string& taskName) const {
    auto task = std::make_shared<Task>(taskName, Date::today());
    for (const auto& item : items_) {
        task->addItem(std::make_shared<ChecklistItem>(item.text, item.done));
    }
    return task;
}

std::string TaskTemplate::generateId() {
    auto now = std::chrono::system_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()).count();
    std::ostringstream oss;
    oss << "template_" << ms;
    return oss.str();
}

void TaskTemplate::invokeNameChanged() const {
    if (nameChangedCb_) nameChangedCb_();
}

void TaskTemplate::invokeItemsChanged() const {
    if (itemsChangedCb_) itemsChangedCb_();
}

void TaskTemplate::invokeRecurrenceChanged() const {
    if (recurrenceChangedCb_) recurrenceChangedCb_();
}

} // namespace checklist
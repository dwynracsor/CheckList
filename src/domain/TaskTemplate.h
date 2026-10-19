#pragma once

#include <string>
#include <vector>
#include <chrono>
#include <memory>
#include <functional>

namespace checklist {

enum class TemplateRecurrence {
    None,        // No recurrente (plantilla simple)
    Daily,       // Diaria
    Weekly,      // Semanal
    Monthly      // Mensual
};

struct TaskTemplateItem {
    std::string text;
    bool done = false;

    bool operator==(const TaskTemplateItem& other) const {
        return text == other.text && done == other.done;
    }
    bool operator!=(const TaskTemplateItem& other) const {
        return !(*this == other);
    }
};

class TaskTemplate {
public:
    using Ptr = std::shared_ptr<TaskTemplate>;

    TaskTemplate();
    TaskTemplate(const std::string& name, const std::vector<TaskTemplateItem>& items = {});
    TaskTemplate(const std::string& id, const std::string& name,
                 const std::vector<TaskTemplateItem>& items,
                 TemplateRecurrence recurrence = TemplateRecurrence::None);
    ~TaskTemplate();

    // Copy/move
    TaskTemplate(const TaskTemplate& other);
    TaskTemplate& operator=(const TaskTemplate& other);
    TaskTemplate(TaskTemplate&& other) noexcept;
    TaskTemplate& operator=(TaskTemplate&& other) noexcept;

    const std::string& id() const { return id_; }
    const std::string& name() const { return name_; }
    const std::vector<TaskTemplateItem>& items() const { return items_; }
    size_t itemCount() const { return items_.size(); }
    TemplateRecurrence recurrence() const { return recurrence_; }
    int recurrenceInterval() const { return recurrenceInterval_; }

    void setName(const std::string& name);
    void setItems(const std::vector<TaskTemplateItem>& items);
    void setRecurrence(TemplateRecurrence recurrence, int interval = 1);

    // Generar una Task desde esta plantilla
    std::shared_ptr<class Task> instantiate(const std::string& taskName) const;

    // Event callbacks
    using NameChangedCallback = std::function<void()>;
    using ItemsChangedCallback = std::function<void()>;
    using RecurrenceChangedCallback = std::function<void()>;

    void setNameChangedCallback(NameChangedCallback cb) { nameChangedCb_ = std::move(cb); }
    void setItemsChangedCallback(ItemsChangedCallback cb) { itemsChangedCb_ = std::move(cb); }
    void setRecurrenceChangedCallback(RecurrenceChangedCallback cb) { recurrenceChangedCb_ = std::move(cb); }

    void invokeNameChanged() const;
    void invokeItemsChanged() const;
    void invokeRecurrenceChanged() const;

private:
    std::string id_;
    std::string name_;
    std::vector<TaskTemplateItem> items_;
    TemplateRecurrence recurrence_;
    int recurrenceInterval_; // Cada cuánto se repite (ej. cada 2 semanas)

    // Callbacks
    NameChangedCallback nameChangedCb_;
    ItemsChangedCallback itemsChangedCb_;
    RecurrenceChangedCallback recurrenceChangedCb_;

public:
    static std::string generateId();

private:
};

} // namespace checklist
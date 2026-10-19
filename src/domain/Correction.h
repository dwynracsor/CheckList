#pragma once

#include <string>
#include <vector>
#include <memory>
#include <functional>
#include "Date.h"
#include "ChecklistItem.h"

namespace checklist {

class Correction {
public:
    using Ptr = std::shared_ptr<Correction>;

    Correction();
    Correction(const std::string& name, const Date& date);
    Correction(const std::string& id, const std::string& name, const Date& date);
    ~Correction();

    // Copy/move
    Correction(const Correction& other);
    Correction& operator=(const Correction& other);
    Correction(Correction&& other) noexcept;
    Correction& operator=(Correction&& other) noexcept;

    const std::string& id() const { return id_; }
    const std::string& name() const { return name_; }
    void setName(const std::string& n) { name_ = n; invokeNameChanged(); }
    const Date& date() const { return date_; }

    size_t itemCount() const { return items_.size(); }

    ChecklistItem* getItem(size_t index);
    const ChecklistItem* getItem(size_t index) const;
    void addItem(const std::shared_ptr<ChecklistItem>& item);
    void removeItem(size_t index);
    void addNewItem(const std::string& text);
    void toggleItem(size_t index);

    // Event callbacks
    using NameChangedCallback = std::function<void()>;
    using ItemCountChangedCallback = std::function<void()>;
    using ItemsChangedCallback = std::function<void()>;

    void setNameChangedCallback(NameChangedCallback cb) { nameChangedCb_ = std::move(cb); }
    void setItemCountChangedCallback(ItemCountChangedCallback cb) { itemCountChangedCb_ = std::move(cb); }
    void setItemsChangedCallback(ItemsChangedCallback cb) { itemsChangedCb_ = std::move(cb); }

    void invokeNameChanged() const;
    void invokeItemCountChanged() const;
    void invokeItemsChanged() const;

    // String operations
    static std::vector<std::string> tokenize(const std::string& text);

private:
    std::string id_;
    std::string name_;
    Date date_;
    std::vector<std::shared_ptr<ChecklistItem>> items_;

    // Callbacks
    NameChangedCallback nameChangedCb_;
    ItemCountChangedCallback itemCountChangedCb_;
    ItemsChangedCallback itemsChangedCb_;

public:
    static std::string generateId();
};

} // namespace checklist
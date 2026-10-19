#pragma once

#include <string>
#include <functional>
#include <memory>
#include "Date.h"

namespace checklist {

class ChecklistItem {
public:
    using Ptr = std::shared_ptr<ChecklistItem>;

    ChecklistItem();
    ChecklistItem(const std::string& text, bool done = false);
    ChecklistItem(const std::string& id, const std::string& text, bool done);
    ~ChecklistItem();

    // Copy/move
    ChecklistItem(const ChecklistItem& other);
    ChecklistItem& operator=(const ChecklistItem& other);
    ChecklistItem(ChecklistItem&& other) noexcept;
    ChecklistItem& operator=(ChecklistItem&& other) noexcept;

    const std::string& id() const { return id_; }
    const std::string& text() const { return text_; }
    bool done() const { return done_; }

    void setText(const std::string& text);
    void setDone(bool done);

    // Event callbacks
    using TextChangedCallback = std::function<void()>;
    using DoneChangedCallback = std::function<void()>;

    void setTextChangedCallback(TextChangedCallback cb) { textChangedCb_ = std::move(cb); }
    void setDoneChangedCallback(DoneChangedCallback cb) { doneChangedCb_ = std::move(cb); }

    void invokeTextChanged() const;
    void invokeDoneChanged() const;

    // Toggle done status
    void toggle();

private:
    std::string id_;
    std::string text_;
    bool done_;

    // Callbacks
    TextChangedCallback textChangedCb_;
    DoneChangedCallback doneChangedCb_;

public:
    static std::string generateId();
};

} // namespace checklist
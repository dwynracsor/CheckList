#include "ChecklistItem.h"
#include <chrono>
#include <functional>
#include <sstream>

namespace checklist {

ChecklistItem::ChecklistItem()
    : id_("item_" + std::to_string(std::hash<std::string>{}("default"))), text_(""), done_(false) {}

ChecklistItem::ChecklistItem(const std::string& text, bool done)
    : ChecklistItem(generateId(), text, done) {}

ChecklistItem::ChecklistItem(const std::string& id, const std::string& text, bool done)
    : id_(id), text_(text), done_(done) {}

ChecklistItem::~ChecklistItem() = default;

ChecklistItem::ChecklistItem(const ChecklistItem& other)
    : id_(other.id_), text_(other.text_), done_(other.done_) {}

ChecklistItem& ChecklistItem::operator=(const ChecklistItem& other) {
    if (this != &other) {
        id_ = other.id_;
        text_ = other.text_;
        done_ = other.done_;
    }
    return *this;
}

ChecklistItem::ChecklistItem(ChecklistItem&& other) noexcept
    : id_(std::move(other.id_)), text_(std::move(other.text_)), done_(other.done_) {}

ChecklistItem& ChecklistItem::operator=(ChecklistItem&& other) noexcept {
    if (this != &other) {
        id_ = std::move(other.id_);
        text_ = std::move(other.text_);
        done_ = other.done_;
    }
    return *this;
}

void ChecklistItem::setText(const std::string& text) {
    if (text_ != text) {
        text_ = text;
        if (textChangedCb_) textChangedCb_();
    }
}

void ChecklistItem::setDone(bool done) {
    if (done_ != done) {
        done_ = done;
        if (doneChangedCb_) doneChangedCb_();
    }
}

void ChecklistItem::toggle() {
    done_ = !done_;
    if (doneChangedCb_) doneChangedCb_();
}

void ChecklistItem::invokeTextChanged() const {
    if (textChangedCb_) textChangedCb_();
}

void ChecklistItem::invokeDoneChanged() const {
    if (doneChangedCb_) doneChangedCb_();
}

std::string ChecklistItem::generateId() {
    auto now = std::chrono::system_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()).count();
    std::ostringstream oss;
    oss << "item_" << ms;
    return oss.str();
}

} // namespace checklist
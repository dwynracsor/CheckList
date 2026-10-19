#include "Correction.h"
#include <algorithm>
#include <sstream>
#include <chrono>

namespace checklist {

Correction::Correction()
    : id_("correction_" + std::to_string(std::hash<std::string>{}("default"))),
      name_("Unnamed"),
      date_(Date::today()) {}

Correction::Correction(const std::string& name, const Date& date)
    : Correction(generateId(), name, date) {}

Correction::Correction(const std::string& id, const std::string& name, const Date& date)
    : id_(id), name_(name), date_(date) {}

Correction::~Correction() = default;

Correction::Correction(const Correction& other)
    : id_(other.id_), name_(other.name_), date_(other.date_) {
    for (const auto& item : other.items_) {
        if (item) {
            items_.push_back(std::make_shared<ChecklistItem>(*item));
        }
    }
}

Correction& Correction::operator=(const Correction& other) {
    if (this != &other) {
        id_ = other.id_;
        name_ = other.name_;
        date_ = other.date_;

        items_.clear();
        for (const auto& item : other.items_) {
            if (item) {
                items_.push_back(std::make_shared<ChecklistItem>(*item));
            }
        }
    }
    return *this;
}

Correction::Correction(Correction&& other) noexcept
    : id_(std::move(other.id_)), name_(std::move(other.name_)),
      date_(std::move(other.date_)) {
    items_ = std::move(other.items_);
}

Correction& Correction::operator=(Correction&& other) noexcept {
    if (this != &other) {
        id_ = std::move(other.id_);
        name_ = std::move(other.name_);
        date_ = std::move(other.date_);
        items_ = std::move(other.items_);
    }
    return *this;
}

ChecklistItem* Correction::getItem(size_t index) {
    if (index < items_.size()) {
        return items_[index].get();
    }
    return nullptr;
}

const ChecklistItem* Correction::getItem(size_t index) const {
    if (index < items_.size()) {
        return items_[index].get();
    }
    return nullptr;
}

void Correction::addItem(const std::shared_ptr<ChecklistItem>& item) {
    if (item) {
        items_.push_back(item);
        invokeItemCountChanged();
        invokeItemsChanged();
    }
}

void Correction::removeItem(size_t index) {
    if (index < items_.size()) {
        items_.erase(items_.begin() + static_cast<long>(index));
        invokeItemCountChanged();
        invokeItemsChanged();
    }
}

void Correction::addNewItem(const std::string& text) {
    auto item = std::make_shared<ChecklistItem>(text, false);
    addItem(item);
}

void Correction::toggleItem(size_t index) {
    if (index < items_.size()) {
        items_[index]->toggle();
        invokeItemCountChanged();
    }
}

void Correction::invokeNameChanged() const {
    if (nameChangedCb_) nameChangedCb_();
}

void Correction::invokeItemCountChanged() const {
    if (itemCountChangedCb_) itemCountChangedCb_();
}

void Correction::invokeItemsChanged() const {
    if (itemsChangedCb_) itemsChangedCb_();
}

std::vector<std::string> Correction::tokenize(const std::string& text) {
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

std::string Correction::generateId() {
    auto now = std::chrono::system_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()).count();
    std::ostringstream oss;
    oss << "correction_" << ms;
    return oss.str();
}

} // namespace checklist
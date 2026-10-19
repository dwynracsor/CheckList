#pragma once

#include <functional>
#include <string>

namespace checklist {

class UndoCommand {
public:
    using UndoFunc = std::function<void()>;
    using RedoFunc = std::function<void()>;

    UndoCommand(const std::string& description, UndoFunc undo, RedoFunc redo);
    ~UndoCommand();

    const std::string& description() const { return description_; }
    void undo();
    void redo();

private:
    std::string description_;
    UndoFunc undo_;
    RedoFunc redo_;
};

} // namespace checklist
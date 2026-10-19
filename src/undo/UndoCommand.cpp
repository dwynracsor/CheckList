#include "undo/UndoCommand.h"

namespace checklist {

UndoCommand::UndoCommand(const std::string& description, UndoFunc undo, RedoFunc redo)
    : description_(description), undo_(std::move(undo)), redo_(std::move(redo)) {}

UndoCommand::~UndoCommand() = default;

void UndoCommand::undo() {
    if (undo_) undo_();
}

void UndoCommand::redo() {
    if (redo_) redo_();
}

} // namespace checklist
#include "undo/UndoCommand.h"

UndoCommand::UndoCommand(const QString &description,
                         std::function<void()> undo,
                         std::function<void()> redo)
    : m_description(description)
    , m_undo(std::move(undo))
    , m_redo(std::move(redo))
{
}

void UndoCommand::undo()
{
    if (m_undo) m_undo();
}

void UndoCommand::redo()
{
    if (m_redo) m_redo();
}

QString UndoCommand::description() const
{
    return m_description;
}

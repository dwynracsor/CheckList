#ifndef UNDOCOMMAND_H
#define UNDOCOMMAND_H

#include <functional>
#include <QString>

class UndoCommand
{
public:
    UndoCommand(const QString &description,
                std::function<void()> undo,
                std::function<void()> redo);

    void undo();
    void redo();
    QString description() const;

private:
    QString m_description;
    std::function<void()> m_undo;
    std::function<void()> m_redo;
};

#endif // UNDOCOMMAND_H

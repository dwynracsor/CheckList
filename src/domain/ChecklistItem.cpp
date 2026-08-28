#include "domain/ChecklistItem.h"

ChecklistItem::ChecklistItem(QObject *parent)
    : QObject(parent)
    , m_id(QUuid::createUuid().toString(QUuid::WithoutBraces))
    , m_text("")
    , m_done(false)
{
}

ChecklistItem::ChecklistItem(const QString &text, bool done, QObject *parent)
    : QObject(parent)
    , m_id(QUuid::createUuid().toString(QUuid::WithoutBraces))
    , m_text(text)
    , m_done(done)
{
}

ChecklistItem::ChecklistItem(const QString &id, const QString &text, bool done, QObject *parent)
    : QObject(parent)
    , m_id(id.isEmpty() ? QUuid::createUuid().toString(QUuid::WithoutBraces) : id)
    , m_text(text)
    , m_done(done)
{
}

QString ChecklistItem::id() const
{
    return m_id;
}

QString ChecklistItem::text() const
{
    return m_text;
}

void ChecklistItem::setText(const QString &text)
{
    if (m_text != text) {
        m_text = text;
        emit textChanged();
    }
}

bool ChecklistItem::done() const
{
    return m_done;
}

void ChecklistItem::setDone(bool done)
{
    if (m_done != done) {
        m_done = done;
        emit doneChanged();
    }
}

void ChecklistItem::toggle()
{
    setDone(!m_done);
}

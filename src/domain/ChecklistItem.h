#ifndef CHECKLISTITEM_H
#define CHECKLISTITEM_H

#include <QObject>
#include <QString>
#include <QUuid>

class ChecklistItem : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString id READ id CONSTANT)
    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
    Q_PROPERTY(bool done READ done WRITE setDone NOTIFY doneChanged)

public:
    explicit ChecklistItem(QObject *parent = nullptr);
    explicit ChecklistItem(const QString &text, bool done = false, QObject *parent = nullptr);
    explicit ChecklistItem(const QString &id, const QString &text, bool done, QObject *parent = nullptr);

    QString id() const;
    QString text() const;
    void setText(const QString &text);
    bool done() const;
    void setDone(bool done);

    Q_INVOKABLE void toggle();

signals:
    void textChanged();
    void doneChanged();

private:
    QString m_id;
    QString m_text;
    bool m_done;
};

#endif // CHECKLISTITEM_H

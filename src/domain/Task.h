#ifndef TASK_H
#define TASK_H

#include <QObject>
#include <QString>
#include <QDate>
#include <QList>
#include <QQmlListProperty>

class ChecklistItem;
class Correction;

class Task : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString id READ id CONSTANT)
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QString date READ date CONSTANT)
    Q_PROPERTY(int itemCount READ itemCount NOTIFY itemCountChanged)
    Q_PROPERTY(int correctionCount READ correctionCount NOTIFY correctionCountChanged)
    Q_PROPERTY(QQmlListProperty<ChecklistItem> items READ items NOTIFY itemsChanged)
    Q_PROPERTY(QQmlListProperty<Correction> corrections READ corrections NOTIFY correctionsChanged)

public:
    explicit Task(QObject *parent = nullptr);
    explicit Task(const QString &name, const QDate &date = QDate::currentDate(), QObject *parent = nullptr);
    explicit Task(const QString &id, const QString &name, const QDate &date, QObject *parent = nullptr);
    ~Task() override;

    QString id() const;
    QString name() const;
    void setName(const QString &name);
    QString date() const;
    int itemCount() const;
    int correctionCount() const;

    QQmlListProperty<ChecklistItem> items();
    QQmlListProperty<Correction> corrections();

    // C++ API for controllers
    Q_INVOKABLE void addItem(ChecklistItem *item);
    Q_INVOKABLE void removeItem(int index);
    Q_INVOKABLE ChecklistItem* getItem(int index) const;

    Q_INVOKABLE void addCorrection(Correction *correction);
    Q_INVOKABLE void removeCorrection(int index);
    Q_INVOKABLE Correction* getCorrection(int index) const;

    // Serialización helpers
    Q_INVOKABLE void addNewItem(const QString &text);
    Q_INVOKABLE Correction* addNewCorrection(const QString &name);

signals:
    void nameChanged();
    void itemCountChanged();
    void correctionCountChanged();
    void itemsChanged();
    void correctionsChanged();

private:
    static void itemsAppend(QQmlListProperty<ChecklistItem> *list, ChecklistItem *item);
    static qsizetype itemsCount(QQmlListProperty<ChecklistItem> *list);
    static ChecklistItem* itemsAt(QQmlListProperty<ChecklistItem> *list, qsizetype index);
    static void itemsClear(QQmlListProperty<ChecklistItem> *list);

    static void correctionsAppend(QQmlListProperty<Correction> *list, Correction *correction);
    static qsizetype correctionsCount(QQmlListProperty<Correction> *list);
    static Correction* correctionsAt(QQmlListProperty<Correction> *list, qsizetype index);
    static void correctionsClear(QQmlListProperty<Correction> *list);

    QString m_id;
    QString m_name;
    QDate m_date;
    QList<ChecklistItem*> m_items;
    QList<Correction*> m_corrections;
};

#endif // TASK_H

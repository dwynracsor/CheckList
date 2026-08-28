#ifndef CORRECTION_H
#define CORRECTION_H

#include <QObject>
#include <QString>
#include <QDate>
#include <QList>
#include <QQmlListProperty>

class ChecklistItem;

class Correction : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString id READ id CONSTANT)
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QString date READ date CONSTANT)
    Q_PROPERTY(int itemCount READ itemCount NOTIFY itemCountChanged)
    Q_PROPERTY(QQmlListProperty<ChecklistItem> items READ items NOTIFY itemsChanged)

public:
    explicit Correction(QObject *parent = nullptr);
    explicit Correction(const QString &name, const QDate &date = QDate::currentDate(), QObject *parent = nullptr);
    explicit Correction(const QString &id, const QString &name, const QDate &date, QObject *parent = nullptr);
    ~Correction() override;

    QString id() const;
    QString name() const;
    void setName(const QString &name);
    QString date() const;
    int itemCount() const;

    QQmlListProperty<ChecklistItem> items();

    // C++ API for controllers
    Q_INVOKABLE void addItem(ChecklistItem *item);
    Q_INVOKABLE void removeItem(int index);
    Q_INVOKABLE ChecklistItem* getItem(int index) const;
    int itemCountRaw() const;

    // Serialización
    Q_INVOKABLE void addNewItem(const QString &text);

signals:
    void nameChanged();
    void itemCountChanged();
    void itemsChanged();

private:
    static void itemsAppend(QQmlListProperty<ChecklistItem> *list, ChecklistItem *item);
    static qsizetype itemsCount(QQmlListProperty<ChecklistItem> *list);
    static ChecklistItem* itemsAt(QQmlListProperty<ChecklistItem> *list, qsizetype index);
    static void itemsClear(QQmlListProperty<ChecklistItem> *list);

    QString m_id;
    QString m_name;
    QDate m_date;
    QList<ChecklistItem*> m_items;
};

#endif // CORRECTION_H

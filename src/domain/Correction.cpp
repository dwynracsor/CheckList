#include "domain/Correction.h"
#include "domain/ChecklistItem.h"

Correction::Correction(QObject *parent)
    : QObject(parent)
    , m_id(QUuid::createUuid().toString(QUuid::WithoutBraces))
    , m_name("Sin nombre")
    , m_date(QDate::currentDate())
{
}

Correction::Correction(const QString &name, const QDate &date, QObject *parent)
    : QObject(parent)
    , m_id(QUuid::createUuid().toString(QUuid::WithoutBraces))
    , m_name(name)
    , m_date(date.isValid() ? date : QDate::currentDate())
{
}

Correction::Correction(const QString &id, const QString &name, const QDate &date, QObject *parent)
    : QObject(parent)
    , m_id(id.isEmpty() ? QUuid::createUuid().toString(QUuid::WithoutBraces) : id)
    , m_name(name)
    , m_date(date.isValid() ? date : QDate::currentDate())
{
}

Correction::~Correction()
{
    qDeleteAll(m_items);
    m_items.clear();
}

QString Correction::id() const
{
    return m_id;
}

QString Correction::name() const
{
    return m_name;
}

void Correction::setName(const QString &name)
{
    if (m_name != name) {
        m_name = name;
        emit nameChanged();
    }
}

QString Correction::date() const
{
    return m_date.toString(Qt::ISODate);
}

int Correction::itemCount() const
{
    return m_items.size();
}

QQmlListProperty<ChecklistItem> Correction::items()
{
    return QQmlListProperty<ChecklistItem>(
        this,
        nullptr,
        &Correction::itemsAppend,
        &Correction::itemsCount,
        &Correction::itemsAt,
        &Correction::itemsClear
    );
}

void Correction::addItem(ChecklistItem *item)
{
    if (!item) return;
    item->setParent(this);
    m_items.append(item);
    emit itemCountChanged();
    emit itemsChanged();
}

void Correction::removeItem(int index)
{
    if (index < 0 || index >= m_items.size()) return;
    ChecklistItem *item = m_items.takeAt(index);
    item->deleteLater();
    emit itemCountChanged();
    emit itemsChanged();
}

ChecklistItem* Correction::getItem(int index) const
{
    if (index < 0 || index >= m_items.size()) return nullptr;
    return m_items.at(index);
}

int Correction::itemCountRaw() const
{
    return m_items.size();
}

void Correction::addNewItem(const QString &text)
{
    ChecklistItem *item = new ChecklistItem(text, false, this);
    addItem(item);
}

// QQmlListProperty callbacks
void Correction::itemsAppend(QQmlListProperty<ChecklistItem> *list, ChecklistItem *item)
{
    Correction *self = qobject_cast<Correction*>(list->object);
    if (self) self->addItem(item);
}

qsizetype Correction::itemsCount(QQmlListProperty<ChecklistItem> *list)
{
    Correction *self = qobject_cast<Correction*>(list->object);
    return self ? self->m_items.size() : 0;
}

ChecklistItem* Correction::itemsAt(QQmlListProperty<ChecklistItem> *list, qsizetype index)
{
    Correction *self = qobject_cast<Correction*>(list->object);
    return self ? self->m_items.at(index) : nullptr;
}

void Correction::itemsClear(QQmlListProperty<ChecklistItem> *list)
{
    Correction *self = qobject_cast<Correction*>(list->object);
    if (self) {
        qDeleteAll(self->m_items);
        self->m_items.clear();
        emit self->itemCountChanged();
        emit self->itemsChanged();
    }
}

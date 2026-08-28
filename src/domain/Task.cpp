#include "domain/Task.h"
#include "domain/ChecklistItem.h"
#include "domain/Correction.h"

Task::Task(QObject *parent)
    : QObject(parent)
    , m_id(QUuid::createUuid().toString(QUuid::WithoutBraces))
    , m_name("Sin nombre")
    , m_date(QDate::currentDate())
{
}

Task::Task(const QString &name, const QDate &date, QObject *parent)
    : QObject(parent)
    , m_id(QUuid::createUuid().toString(QUuid::WithoutBraces))
    , m_name(name)
    , m_date(date.isValid() ? date : QDate::currentDate())
{
}

Task::Task(const QString &id, const QString &name, const QDate &date, QObject *parent)
    : QObject(parent)
    , m_id(id.isEmpty() ? QUuid::createUuid().toString(QUuid::WithoutBraces) : id)
    , m_name(name)
    , m_date(date.isValid() ? date : QDate::currentDate())
{
}

Task::~Task()
{
    qDeleteAll(m_items);
    m_items.clear();
    qDeleteAll(m_corrections);
    m_corrections.clear();
}

QString Task::id() const
{
    return m_id;
}

QString Task::name() const
{
    return m_name;
}

void Task::setName(const QString &name)
{
    if (m_name != name) {
        m_name = name;
        emit nameChanged();
    }
}

QString Task::date() const
{
    return m_date.toString(Qt::ISODate);
}

int Task::itemCount() const
{
    return m_items.size();
}

int Task::correctionCount() const
{
    return m_corrections.size();
}

QQmlListProperty<ChecklistItem> Task::items()
{
    return QQmlListProperty<ChecklistItem>(
        this,
        nullptr,
        &Task::itemsAppend,
        &Task::itemsCount,
        &Task::itemsAt,
        &Task::itemsClear
    );
}

QQmlListProperty<Correction> Task::corrections()
{
    return QQmlListProperty<Correction>(
        this,
        nullptr,
        &Task::correctionsAppend,
        &Task::correctionsCount,
        &Task::correctionsAt,
        &Task::correctionsClear
    );
}

// Items API
void Task::addItem(ChecklistItem *item)
{
    if (!item) return;
    item->setParent(this);
    m_items.append(item);
    emit itemCountChanged();
    emit itemsChanged();
}

void Task::removeItem(int index)
{
    if (index < 0 || index >= m_items.size()) return;
    ChecklistItem *item = m_items.takeAt(index);
    item->deleteLater();
    emit itemCountChanged();
    emit itemsChanged();
}

ChecklistItem* Task::getItem(int index) const
{
    if (index < 0 || index >= m_items.size()) return nullptr;
    return m_items.at(index);
}

// Corrections API
void Task::addCorrection(Correction *correction)
{
    if (!correction) return;
    correction->setParent(this);
    m_corrections.append(correction);
    emit correctionCountChanged();
    emit correctionsChanged();
}

void Task::removeCorrection(int index)
{
    if (index < 0 || index >= m_corrections.size()) return;
    Correction *correction = m_corrections.takeAt(index);
    correction->deleteLater();
    emit correctionCountChanged();
    emit correctionsChanged();
}

Correction* Task::getCorrection(int index) const
{
    if (index < 0 || index >= m_corrections.size()) return nullptr;
    return m_corrections.at(index);
}

// Serialization helpers
void Task::addNewItem(const QString &text)
{
    ChecklistItem *item = new ChecklistItem(text, false, this);
    addItem(item);
}

Correction* Task::addNewCorrection(const QString &name)
{
    Correction *correction = new Correction(name, QDate::currentDate(), this);
    addCorrection(correction);
    return correction;
}

// QQmlListProperty callbacks - Items
void Task::itemsAppend(QQmlListProperty<ChecklistItem> *list, ChecklistItem *item)
{
    Task *self = qobject_cast<Task*>(list->object);
    if (self) self->addItem(item);
}

qsizetype Task::itemsCount(QQmlListProperty<ChecklistItem> *list)
{
    Task *self = qobject_cast<Task*>(list->object);
    return self ? self->m_items.size() : 0;
}

ChecklistItem* Task::itemsAt(QQmlListProperty<ChecklistItem> *list, qsizetype index)
{
    Task *self = qobject_cast<Task*>(list->object);
    return self ? self->m_items.at(index) : nullptr;
}

void Task::itemsClear(QQmlListProperty<ChecklistItem> *list)
{
    Task *self = qobject_cast<Task*>(list->object);
    if (self) {
        qDeleteAll(self->m_items);
        self->m_items.clear();
        emit self->itemCountChanged();
        emit self->itemsChanged();
    }
}

// QQmlListProperty callbacks - Corrections
void Task::correctionsAppend(QQmlListProperty<Correction> *list, Correction *correction)
{
    Task *self = qobject_cast<Task*>(list->object);
    if (self) self->addCorrection(correction);
}

qsizetype Task::correctionsCount(QQmlListProperty<Correction> *list)
{
    Task *self = qobject_cast<Task*>(list->object);
    return self ? self->m_corrections.size() : 0;
}

Correction* Task::correctionsAt(QQmlListProperty<Correction> *list, qsizetype index)
{
    Task *self = qobject_cast<Task*>(list->object);
    return self ? self->m_corrections.at(index) : nullptr;
}

void Task::correctionsClear(QQmlListProperty<Correction> *list)
{
    Task *self = qobject_cast<Task*>(list->object);
    if (self) {
        qDeleteAll(self->m_corrections);
        self->m_corrections.clear();
        emit self->correctionCountChanged();
        emit self->correctionsChanged();
    }
}

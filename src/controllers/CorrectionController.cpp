#include "controllers/CorrectionController.h"
#include "domain/Correction.h"
#include "domain/ChecklistItem.h"

CorrectionController::CorrectionController(QObject *parent)
    : QObject(parent)
    , m_correction(nullptr)
{
}

CorrectionController::~CorrectionController() = default;

QQmlListProperty<ChecklistItem> CorrectionController::items()
{
    return QQmlListProperty<ChecklistItem>(
        this,
        nullptr,
        &CorrectionController::itemsAppend,
        &CorrectionController::itemsCount,
        &CorrectionController::itemsAt,
        &CorrectionController::itemsClear
    );
}

int CorrectionController::itemCount() const
{
    return m_correction ? m_correction->itemCountRaw() : 0;
}

QString CorrectionController::correctionName() const
{
    return m_correction ? m_correction->name() : QString();
}

void CorrectionController::setCorrectionName(const QString &name)
{
    if (m_correction && !name.trimmed().isEmpty()) {
        m_correction->setName(name.trimmed());
        emit nameChanged();
    }
}

void CorrectionController::bindTo(Correction *correction)
{
    if (m_correction == correction) return;

    m_correction = correction;
    emit itemsChanged();
    emit nameChanged();
    emit bound();
}

void CorrectionController::addItem(const QString &text)
{
    if (!m_correction || text.trimmed().isEmpty()) return;

    m_correction->addNewItem(text.trimmed());
    emit itemsChanged();
}

void CorrectionController::removeItem(int index)
{
    if (!m_correction) return;

    m_correction->removeItem(index);
    emit itemsChanged();
}

void CorrectionController::toggleItem(int index, bool done)
{
    if (!m_correction) return;

    ChecklistItem *item = m_correction->getItem(index);
    if (item) {
        item->setDone(done);
    }
}

ChecklistItem* CorrectionController::getItem(int index) const
{
    if (!m_correction) return nullptr;
    return m_correction->getItem(index);
}

// QQmlListProperty callbacks
void CorrectionController::itemsAppend(QQmlListProperty<ChecklistItem> *list, ChecklistItem *item)
{
    CorrectionController *self = qobject_cast<CorrectionController*>(list->object);
    if (self && self->m_correction) {
        self->m_correction->addItem(item);
        emit self->itemsChanged();
    }
}

qsizetype CorrectionController::itemsCount(QQmlListProperty<ChecklistItem> *list)
{
    CorrectionController *self = qobject_cast<CorrectionController*>(list->object);
    return self ? self->itemCount() : 0;
}

ChecklistItem* CorrectionController::itemsAt(QQmlListProperty<ChecklistItem> *list, qsizetype index)
{
    CorrectionController *self = qobject_cast<CorrectionController*>(list->object);
    return self ? self->getItem(index) : nullptr;
}

void CorrectionController::itemsClear(QQmlListProperty<ChecklistItem> *list)
{
    Q_UNUSED(list);
    // Not implemented - use removeItem for each item instead
}

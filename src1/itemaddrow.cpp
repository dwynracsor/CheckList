#include "itemaddrow.h"
#include <QPlainTextEdit>
#include <QPushButton>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QEvent>
#include <QTimer>
#include <QTextDocument>

ItemAddRow::ItemAddRow(QWidget *parent) : QWidget(parent) {
    auto *layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(10);

    m_input = new QPlainTextEdit(this);
    m_input->setPlaceholderText(tr("Item description"));
    m_input->setLineWrapMode(QPlainTextEdit::WidgetWidth);
    m_input->setFixedHeight(44);
    m_input->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_input->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_input->installEventFilter(this);

    m_addBtn = new QPushButton("+", this);
    m_addBtn->setObjectName("PlusButton");
    m_addBtn->setFixedSize(40, 40);
    m_addBtn->setCursor(Qt::PointingHandCursor);

    layout->addWidget(m_input, 1);
    layout->addWidget(m_addBtn, 0);

    connect(m_addBtn, &QPushButton::clicked, this, &ItemAddRow::trySubmit);
    connect(m_input, &QPlainTextEdit::textChanged, this, [this]() {
        QTimer::singleShot(0, this, &ItemAddRow::adjustHeight);
    });
}

void ItemAddRow::adjustHeight() {
    int textWidth = m_input->viewport()->width() - m_input->frameWidth() * 2;
    if (textWidth <= 0) textWidth = m_input->width() - 40;
    QTextDocument probe(m_input->toPlainText());
    probe.setDefaultFont(m_input->font());
    probe.setTextWidth(textWidth);
    int h = static_cast<int>(probe.size().height()) + 24;
    if (h < 44) h = 44;
    m_input->setFixedHeight(h);
}

void ItemAddRow::trySubmit() {
    const QString text = m_input->toPlainText().trimmed();
    if (text.isEmpty()) return;
    emit itemAdded(text);
    m_input->clear();
    adjustHeight();
    m_input->setFocus();
}

void ItemAddRow::clear() {
    m_input->clear();
    adjustHeight();
}

bool ItemAddRow::eventFilter(QObject *obj, QEvent *event) {
    if (obj == m_input && event->type() == QEvent::KeyPress) {
        auto *ke = static_cast<QKeyEvent *>(event);
        if (ke->key() == Qt::Key_Return || ke->key() == Qt::Key_Enter) {
            if (ke->modifiers() & Qt::ShiftModifier)
                return false;
            trySubmit();
            return true;
        }
        if (ke->key() == Qt::Key_Escape) {
            m_input->clear();
            adjustHeight();
            return true;
        }
    }
    return QWidget::eventFilter(obj, event);
}

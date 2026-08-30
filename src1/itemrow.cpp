#include "itemrow.h"
#include <QCheckBox>
#include <QLabel>
#include <QToolButton>
#include <QHBoxLayout>
#include <QStyle>
#include <QMenu>
#include <QContextMenuEvent>
#include <QPlainTextEdit>
#include <QKeyEvent>
#include <QTimer>
#include <QTextDocument>

ItemRow::ItemRow(const QString &text, bool completed, QWidget *parent)
    : QWidget(parent) {
    auto *layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 6, 0, 6);
    layout->setSpacing(10);

    m_checkbox = new QCheckBox(this);
    m_checkbox->setChecked(completed);
    m_checkbox->setFixedSize(20, 20);

    m_label = new QLabel(text, this);
    m_label->setObjectName("ItemText");
    m_label->setWordWrap(true);

    m_deleteBtn = new QToolButton(this);
    m_deleteBtn->setObjectName("DeleteX");
    m_deleteBtn->setText("x");
    m_deleteBtn->setCursor(Qt::PointingHandCursor);
    m_deleteBtn->setAutoRaise(true);

    m_edit = new QPlainTextEdit(text, this);
    m_edit->setObjectName("ItemText");
    m_edit->setLineWrapMode(QPlainTextEdit::WidgetWidth);
    m_edit->setFixedHeight(36);
    m_edit->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_edit->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_edit->setVisible(false);
    m_edit->installEventFilter(this);

    layout->addWidget(m_checkbox, 0, Qt::AlignTop);
    layout->addWidget(m_label, 1);
    layout->addWidget(m_edit, 1);
    layout->addWidget(m_deleteBtn, 0, Qt::AlignTop);

    updateTextStyle(completed);

    connect(m_checkbox, &QCheckBox::toggled, this, [this](bool checked) {
        updateTextStyle(checked);
        emit completedChanged(checked);
    });
    connect(m_deleteBtn, &QToolButton::clicked, this, &ItemRow::deleteRequested);
    connect(m_edit, &QPlainTextEdit::textChanged, this, [this]() {
        QTimer::singleShot(0, this, &ItemRow::adjustHeight);
    });
    connect(this, &ItemRow::editRequested, this, &ItemRow::startInlineEdit);
}

void ItemRow::updateTextStyle(bool completed) {
    m_label->setProperty("completed", completed);
    m_label->style()->unpolish(m_label);
    m_label->style()->polish(m_label);

    QFont f = m_label->font();
    f.setStrikeOut(completed);
    m_label->setFont(f);
}

QString ItemRow::text() const { return m_label->text(); }
bool ItemRow::isCompleted() const { return m_checkbox->isChecked(); }

void ItemRow::adjustHeight() {
    int textWidth = m_edit->viewport()->width() - m_edit->frameWidth() * 2;
    if (textWidth <= 0) textWidth = m_edit->width() - 40;
    QTextDocument probe(m_edit->toPlainText());
    probe.setDefaultFont(m_edit->font());
    probe.setTextWidth(textWidth);
    int h = static_cast<int>(probe.size().height()) + 24;
    if (h < 36) h = 36;
    m_edit->setFixedHeight(h);
}

void ItemRow::startInlineEdit() {
    m_edit->setPlainText(m_label->text());
    adjustHeight();
    m_label->setVisible(false);
    m_checkbox->setVisible(false);
    m_deleteBtn->setVisible(false);
    m_edit->setVisible(true);
    m_edit->setFocus();
    m_edit->selectAll();
}

void ItemRow::commitInlineEdit() {
    QString newText = m_edit->toPlainText().trimmed();
    m_edit->setVisible(false);
    m_label->setVisible(true);
    m_checkbox->setVisible(true);
    m_deleteBtn->setVisible(true);
    if (!newText.isEmpty()) {
        m_label->setText(newText);
        emit textCommitted(newText);
    }
}

void ItemRow::contextMenuEvent(QContextMenuEvent *event) {
    QMenu menu(this);
    auto *edit = menu.addAction(tr("Editar ítem"));
    auto *copyItem = menu.addAction(tr("Copiar ítem"));
    auto *copyList = menu.addAction(tr("Copiar lista"));
    auto *action = menu.exec(event->globalPos());
    if (action == edit) {
        emit editRequested();
    } else if (action == copyItem) {
        emit copyItemRequested(m_label->text());
    } else if (action == copyList) {
        emit copyListRequested();
    }
}

bool ItemRow::eventFilter(QObject *obj, QEvent *event) {
    if (obj == m_edit && event->type() == QEvent::KeyPress) {
        auto *ke = static_cast<QKeyEvent *>(event);
        if (ke->key() == Qt::Key_Return || ke->key() == Qt::Key_Enter) {
            if (ke->modifiers() & Qt::ShiftModifier)
                return false;
            commitInlineEdit();
            return true;
        }
        if (ke->key() == Qt::Key_Escape) {
            m_edit->setVisible(false);
            m_label->setVisible(true);
            m_checkbox->setVisible(true);
            m_deleteBtn->setVisible(true);
            return true;
        }
    }
    return QWidget::eventFilter(obj, event);
}

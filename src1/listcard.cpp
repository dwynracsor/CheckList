#include "listcard.h"
#include "itemrow.h"
#include "itemaddrow.h"
#include "domain/Task.h"
#include "domain/ChecklistItem.h"
#include "controllers/TaskController.h"

#include <QLabel>
#include <QToolButton>
#include <QPushButton>
#include <QProgressBar>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QMessageBox>
#include <QInputDialog>
#include <QDate>
#include <QApplication>
#include <QClipboard>
#include <QStringList>

ListCard::ListCard(TaskController *controller, int taskIndex, QWidget *parent)
    : QWidget(parent)
    , m_controller(controller)
    , m_taskIndex(taskIndex)
    , m_task(controller ? controller->getTask(taskIndex) : nullptr)
{
    setObjectName("ListCard");
    auto *outer = new QVBoxLayout(this);
    outer->setContentsMargins(16, 12, 16, 14);
    outer->setSpacing(10);

    // ---- Header: nombre / lápiz / "-" / "x" ----
    auto *header = new QHBoxLayout();
    header->setSpacing(8);

    m_nameLabel = new QLabel(m_task ? m_task->name() : QString(), this);
    m_nameLabel->setObjectName("ListName");
    m_nameLabel->setWordWrap(true);

    auto *editBtn = new QToolButton(this);
    editBtn->setObjectName("EditPencil");
    editBtn->setText("\u270E"); // ✎ lápiz
    editBtn->setAutoRaise(true);
    editBtn->setCursor(Qt::PointingHandCursor);
    editBtn->setToolTip(tr("Editar nombre de la lista"));

    m_nameEdit = new QLineEdit(m_task ? m_task->name() : QString(), this);
    m_nameEdit->setObjectName("ListNameEdit");
    m_nameEdit->setVisible(false);
    m_nameEdit->installEventFilter(this);

    m_minusBtn = new QToolButton(this);
    m_minusBtn->setObjectName("MinusButton");
    m_minusBtn->setText("\u2212"); // −
    m_minusBtn->setAutoRaise(true);
    m_minusBtn->setCursor(Qt::PointingHandCursor);
    m_minusBtn->setToolTip(tr("Minimizar lista"));

    auto *deleteBtn = new QToolButton(this);
    deleteBtn->setObjectName("MinusButton");
    deleteBtn->setText("\u2715"); // ✕
    deleteBtn->setAutoRaise(true);
    deleteBtn->setCursor(Qt::PointingHandCursor);
    deleteBtn->setToolTip(tr("Eliminar lista"));

    header->addWidget(m_nameLabel);
    header->addWidget(m_nameEdit);
    header->addWidget(editBtn);
    header->addStretch(1);
    header->addWidget(m_minusBtn);
    header->addWidget(deleteBtn);
    outer->addLayout(header);

    // ---- Contenedor de items ----
    m_itemsContainer = new QWidget(this);
    m_itemsLayout = new QVBoxLayout(m_itemsContainer);
    m_itemsLayout->setContentsMargins(0, 0, 0, 0);
    m_itemsLayout->setSpacing(2);
    outer->addWidget(m_itemsContainer);

    // ---- Botón "+ New item" ----
    m_newItemBtn = new QPushButton(tr("+ New item"), this);
    m_newItemBtn->setObjectName("NewItemButton");
    m_newItemBtn->setCursor(Qt::PointingHandCursor);
    outer->addWidget(m_newItemBtn);

    // ---- Progreso + fecha ----
    auto *footer = new QHBoxLayout();
    m_progress = new QProgressBar(this);
    m_progress->setObjectName("ProgressPill");
    m_progress->setTextVisible(false);
    m_progress->setFixedHeight(8);

    m_progressLabel = new QLabel("0%", this);
    m_progressLabel->setObjectName("ProgressLabel");

    QString dateStr;
    if (m_task) {
        QDate d = QDate::fromString(m_task->date(), Qt::ISODate);
        dateStr = d.isValid() ? d.toString("dd/MM/yyyy") : m_task->date();
    }
    auto *dateLabel = new QLabel(dateStr, this);
    dateLabel->setObjectName("DateLabel");

    footer->addWidget(m_progress, 1);
    footer->addWidget(m_progressLabel, 0);
    footer->addWidget(dateLabel, 0);
    outer->addLayout(footer);

    rebuildItems();
    updateProgress();

    // ---- Conexiones ----
    connect(editBtn, &QToolButton::clicked, this, &ListCard::startRename);
    connect(m_nameEdit, &QLineEdit::returnPressed, this, &ListCard::commitRename);
    connect(m_minusBtn, &QToolButton::clicked, this, [this]() {
        m_collapsed = !m_collapsed;
        m_itemsContainer->setVisible(!m_collapsed);
        m_newItemBtn->setVisible(!m_collapsed);
        m_minusBtn->setText(m_collapsed ? QString("+") : QString("\u2212"));
    });
    connect(deleteBtn, &QToolButton::clicked, this, [this]() {
        if (!m_controller) return;
        auto reply = QMessageBox::question(this, tr("Eliminar lista"),
            tr("¿Eliminar la lista \"%1\"? Esta acción no se puede deshacer.").arg(m_nameLabel->text()));
        if (reply == QMessageBox::Yes) m_controller->removeTask(m_taskIndex);
    });
    connect(m_newItemBtn, &QPushButton::clicked, this, [this]() {
        if (m_addRow) {
            m_addRow->deleteLater();
            m_addRow = nullptr;
            return;
        }
        m_addRow = new ItemAddRow(m_itemsContainer);
        m_itemsLayout->addWidget(m_addRow);
        connect(m_addRow, &ItemAddRow::itemAdded, this, [this](const QString &text) {
            if (m_controller) m_controller->addItemToTask(m_taskIndex, text);
            if (m_addRow) {
                m_addRow->deleteLater();
                m_addRow = nullptr;
            }
        });
    });

    if (m_task) {
        connect(m_task, &Task::itemsChanged, this, &ListCard::rebuildItems);
        connect(m_task, &Task::itemCountChanged, this, &ListCard::updateProgress);
        connect(m_task, &Task::nameChanged, this, [this]() {
            if (m_task) m_nameLabel->setText(m_task->name());
        });
    }
}

void ListCard::rebuildItems() {
    QLayoutItem *child;
    while ((child = m_itemsLayout->takeAt(0)) != nullptr) {
        if (child->widget()) child->widget()->deleteLater();
        delete child;
    }
    m_addRow = nullptr;

    if (!m_task) return;
    for (int i = 0; i < m_task->itemCount(); ++i) {
        ChecklistItem *ci = m_task->getItem(i);
        if (!ci) continue;
        auto *row = new ItemRow(ci->text(), ci->done(), m_itemsContainer);
        connect(row, &ItemRow::completedChanged, this, [this, i](bool completed) {
            if (m_controller) m_controller->toggleItemInTask(m_taskIndex, i, completed);
            updateProgress();
        });
        connect(row, &ItemRow::deleteRequested, this, [this, i]() {
            if (m_controller) m_controller->removeItemFromTask(m_taskIndex, i);
        });
        connect(row, &ItemRow::textCommitted, this, [this, i](const QString &text) {
            if (m_controller) m_controller->setItemTextInTask(m_taskIndex, i, text);
        });
        connect(row, &ItemRow::copyItemRequested, this, [this](const QString &text) {
            if (QApplication::clipboard()) QApplication::clipboard()->setText(text);
        });
        connect(row, &ItemRow::copyListRequested, this, [this]() {
            if (!m_task) return;
            QStringList lines;
            lines.append(m_task->name());
            for (int i = 0; i < m_task->itemCount(); ++i) {
                ChecklistItem *ci = m_task->getItem(i);
                if (ci) lines.append((ci->done() ? QStringLiteral("[x] ") : QStringLiteral("[ ] ")) + ci->text());
            }
            if (QApplication::clipboard()) QApplication::clipboard()->setText(lines.join(QStringLiteral("\n")));
        });
        m_itemsLayout->addWidget(row);
    }
}

void ListCard::updateProgress() {
    if (!m_task || m_task->itemCount() == 0) {
        m_progress->setValue(0);
        m_progressLabel->setText("0%");
        return;
    }
    int done = 0;
    for (int i = 0; i < m_task->itemCount(); ++i) {
        ChecklistItem *ci = m_task->getItem(i);
        if (ci && ci->done()) done++;
    }
    int pct = static_cast<int>(static_cast<double>(done) / m_task->itemCount() * 100.0);
    m_progress->setMaximum(m_task->itemCount());
    m_progress->setValue(done);
    m_progressLabel->setText(QString::number(pct) + "%");
}

bool ListCard::eventFilter(QObject *obj, QEvent *event) {
    if (obj == m_nameEdit && event->type() == QEvent::KeyPress) {
        auto *ke = static_cast<QKeyEvent *>(event);
        if (ke->key() == Qt::Key_Escape) {
            m_nameEdit->setVisible(false);
            m_nameLabel->setVisible(true);
            return true;
        }
    }
    return QWidget::eventFilter(obj, event);
}

void ListCard::startRename() {
    if (!m_controller || !m_task) return;
    m_nameEdit->setText(m_task->name());
    m_nameLabel->setVisible(false);
    m_nameEdit->setVisible(true);
    m_nameEdit->setFocus();
    m_nameEdit->selectAll();
}

void ListCard::commitRename() {
    if (m_controller && m_task) {
        QString newName = m_nameEdit->text().trimmed();
        if (!newName.isEmpty()) m_controller->setTaskName(m_taskIndex, newName);
    }
    m_nameEdit->setVisible(false);
    m_nameLabel->setVisible(true);
}

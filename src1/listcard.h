#pragma once
#include <QWidget>

class QLabel;
class QLineEdit;
class QToolButton;
class QVBoxLayout;
class QPushButton;
class QProgressBar;
class ItemAddRow;
class TaskController;
class Task;

// Tarjeta de una lista que muestra sus ítems, el botón "+ New item",
// la barra de progreso y la fecha. Está enlazada a un Task del backend
// (TaskController) para que los cambios persistan.
class ListCard : public QWidget {
    Q_OBJECT
public:
    ListCard(TaskController *controller, int taskIndex, QWidget *parent = nullptr);

private slots:
    void rebuildItems();
    void updateProgress();
    void startRename();
    void commitRename();

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;

private:
    TaskController *m_controller;
    int m_taskIndex;
    Task *m_task;

    QLabel *m_nameLabel;
    QLineEdit *m_nameEdit;
    QWidget *m_itemsContainer;
    QVBoxLayout *m_itemsLayout;
    QProgressBar *m_progress;
    QLabel *m_progressLabel;
    QToolButton *m_minusBtn;
    ItemAddRow *m_addRow = nullptr;
    QPushButton *m_newItemBtn;
    bool m_collapsed = false;
};

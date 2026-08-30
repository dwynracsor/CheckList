#pragma once
#include <QWidget>
#include <QContextMenuEvent>
#include <QEvent>

class QCheckBox;
class QLabel;
class QToolButton;
class QPlainTextEdit;

// Replica cada fila "Name of the first item to complete..." de la imagen:
// checkbox + texto (tachado al completarse) + "x" para eliminar.
class ItemRow : public QWidget {
    Q_OBJECT
public:
    explicit ItemRow(const QString &text, bool completed, QWidget *parent = nullptr);

    QString text() const;
    bool isCompleted() const;

protected:
    void contextMenuEvent(QContextMenuEvent *event) override;
    bool eventFilter(QObject *obj, QEvent *event) override;

signals:
    void completedChanged(bool completed);
    void deleteRequested();
    void copyItemRequested(const QString &text);
    void copyListRequested();
    void editRequested();
    void textCommitted(const QString &text);

private:
    QCheckBox *m_checkbox;
    QLabel *m_label;
    QToolButton *m_deleteBtn;
    QPlainTextEdit *m_edit;

    void updateTextStyle(bool completed);
    void startInlineEdit();
    void commitInlineEdit();
    void adjustHeight();
};

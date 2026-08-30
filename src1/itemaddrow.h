#pragma once
#include <QWidget>
#include <QEvent>

class QPlainTextEdit;
class QPushButton;

// Fila "Item description [+]" reutilizada en Create list y en
// "+ New item" de cada lista ya creada. El campo crece y envuelve
// el texto para poder leer la descripción mientras se escribe.
class ItemAddRow : public QWidget {
    Q_OBJECT
public:
    explicit ItemAddRow(QWidget *parent = nullptr);
    void clear();

signals:
    void itemAdded(const QString &text);

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;

private:
    QPlainTextEdit *m_input;
    QPushButton *m_addBtn;

    void trySubmit();
    void adjustHeight();
};

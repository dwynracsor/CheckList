#pragma once
#include <QWidget>
#include <QPropertyAnimation>

// Toggle tipo iOS, replica el switch junto al título "CheckList"
// en la imagen de referencia. Emite toggled(bool) al cambiar.
class ToggleSwitch : public QWidget {
    Q_OBJECT
    Q_PROPERTY(int handlePos READ handlePos WRITE setHandlePos)

public:
    explicit ToggleSwitch(QWidget *parent = nullptr);

    bool isChecked() const { return m_checked; }
    void setChecked(bool checked, bool animate = true);

    int handlePos() const { return m_handlePos; }
    void setHandlePos(int pos) { m_handlePos = pos; update(); }

    QSize sizeHint() const override { return QSize(40, 22); }

signals:
    void toggled(bool checked);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

private:
    bool m_checked = false;
    int m_handlePos = 2;
    QPropertyAnimation *m_anim;
};

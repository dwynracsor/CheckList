#include "toggleswitch.h"
#include <QPainter>
#include <QMouseEvent>

ToggleSwitch::ToggleSwitch(QWidget *parent) : QWidget(parent) {
    setCursor(Qt::PointingHandCursor);
    setMinimumSize(40, 22);
    m_anim = new QPropertyAnimation(this, "handlePos", this);
    m_anim->setDuration(150);
}

void ToggleSwitch::setChecked(bool checked, bool animate) {
    if (m_checked == checked) return;
    m_checked = checked;
    int target = m_checked ? width() - 20 : 2;
    if (animate) {
        m_anim->stop();
        m_anim->setStartValue(m_handlePos);
        m_anim->setEndValue(target);
        m_anim->start();
    } else {
        setHandlePos(target);
    }
    emit toggled(m_checked);
}

void ToggleSwitch::mousePressEvent(QMouseEvent *event) {
    Q_UNUSED(event);
    setChecked(!m_checked);
}

void ToggleSwitch::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    QRectF track(0, 0, width(), height());
    QColor trackColor = m_checked ? QColor("#111111") : QColor("#D7D7D7");
    p.setPen(Qt::NoPen);
    p.setBrush(trackColor);
    p.drawRoundedRect(track, height() / 2.0, height() / 2.0);

    int handleSize = height() - 4;
    QRectF handle(m_handlePos, 2, handleSize, handleSize);
    p.setBrush(QColor("#FFFFFF"));
    p.drawEllipse(handle);
}

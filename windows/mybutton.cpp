#include "mybutton.h"

#include <QMouseEvent>
#include <QPainter>

MyButton::MyButton(QWidget* parent) : QPushButton{parent}
{
}

void MyButton::setImage(QString normal, QString pressed, QString hover)
{
    m_normal = normal;
    m_hover = hover;
    m_pressed = pressed;
    m_pixmap.load(m_normal);
}

void MyButton::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
    {
        m_pixmap.load(m_pressed);
        this->update();
    }
    QPushButton::mousePressEvent(event);
}

void MyButton::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
    {
        m_pixmap.load(m_normal);
        this->update();
    }
    QPushButton::mouseReleaseEvent(event);
}

void MyButton::enterEvent(QEnterEvent* event)
{
    m_pixmap.load(m_hover);
    this->update();
}

void MyButton::leaveEvent(QEvent* event)
{
    m_pixmap.load(m_normal);
    this->update();
}

void MyButton::paintEvent(QPaintEvent* event)
{
    QPainter p(this);
    p.drawPixmap(this->rect(), m_pixmap);
}

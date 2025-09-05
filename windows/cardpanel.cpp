#include "cardpanel.h"

#include "player.h"
#include <QMouseEvent>
#include <QPaintEvent>
#include <QPainter>

CardPanel::CardPanel(QWidget* parent) : QWidget{parent}
{
    m_isfront = true;
}

void CardPanel::setImage(QPixmap& front, QPixmap& back)
{
    m_front = front;
    m_back = back;
}

QPixmap CardPanel::getImage()
{
    return m_front;
}

void CardPanel::setFrontSide(bool flag)
{
    m_isfront = flag;
}

bool CardPanel::isFrontSide()
{
    return m_isfront;
}

void CardPanel::setSelected(bool flag)
{
    m_selected = flag;
}

bool CardPanel::isSelecetd()
{
    return m_selected;
}

void CardPanel::setCard(Card& card)
{
    m_card = card;
}

Card CardPanel::getCard()
{
    return m_card;
}

void CardPanel::setOwner(Player* player)
{
    m_owner = player;
}

Player* CardPanel::getOwner()
{
    return m_owner;
}

void CardPanel::clicked()
{
    emit CardSelected(Qt::LeftButton);
}

void CardPanel::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    if (m_isfront)
        painter.drawPixmap(this->rect(), m_front);
    else
        painter.drawPixmap(this->rect(), m_back);
    this->resize(m_front.size());
    event->accept();
}

void CardPanel::mousePressEvent(QMouseEvent* event)
{
    // 选中扑克牌
    emit CardSelected(event->button());
}

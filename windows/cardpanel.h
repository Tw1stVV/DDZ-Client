#ifndef CARDPANEL_H
#define CARDPANEL_H

#include "card.h"
#include <QWidget>

class Player;

class CardPanel : public QWidget
{
    Q_OBJECT
public:
    explicit CardPanel(QWidget* parent = nullptr);

    // 设置获取图片函数
    void setImage(QPixmap& front, QPixmap& back);
    QPixmap getImage();

    // 扑克牌显示那一面
    void setFrontSide(bool flag);
    bool isFrontSide();

    // 扑克牌是否被选中
    void setSelected(bool flag);
    bool isSelecetd();

    // 扑克牌花色和点数
    void setCard(Card& card);
    Card getCard();

    // 扑克牌所有者
    void setOwner(Player* player);
    Player* getOwner();

    // 发射被鼠标左键点击的信号
    void clicked();

signals:
    void CardSelected(Qt::MouseButton button);

private:
    QPixmap m_front;
    QPixmap m_back;
    bool m_isfront;
    bool m_selected;
    Card m_card;
    Player* m_owner;

    // QWidget interface
protected:
    virtual void paintEvent(QPaintEvent* event) override;

    // QWidget interface
protected:
    virtual void mousePressEvent(QMouseEvent* event) override;
};

#endif // CARDPANEL_H

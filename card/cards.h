#ifndef CARDS_H
#define CARDS_H
#include "card.h"
#include "qset.h"
class Cards
{
public:
    Cards();
    Cards(Card card);
    enum sortType
    {
        Asc,
        Desc,
        NoSort
    };

    // 添加扑克牌
    void add(const Card& card);
    void add(const Cards& cards);
    void add(const QList<Cards>& cardsList);

    // 一次性添加多张扑克牌
    Cards& operator<<(const Card& card);
    Cards& operator<<(const Cards& cards);

    // 删除扑克牌
    void remove(const Card& card);
    void remove(const Cards& cards);
    void remove(const QList<Cards> list);

    // 扑克牌数量
    int cardCount() const;
    // 是否为空
    bool isEmpty() const;
    // 清空扑克牌
    void clear();

    // 最大点数
    Card::CardPoint maxPoint();
    // 最小点数
    Card::CardPoint minPoint();
    // 指定点数的牌的数量
    int pointCount(Card::CardPoint point);
    int pointCount(Card::CardPoint point, const CardList& list);
    // 某张或某几张牌是否在集合中
    bool contains(const Card& card);
    bool contails(const Cards& cards);

    // 随机取出一张扑克牌
    Card takeRandCard();

    // 将扑克牌按主牌在前，副牌在后的方式排序
    CardList sort();

    // 默认降序
    CardList toCardList(sortType type = Desc) const;

    static Cards toCards(const CardList& list);

private:
    QSet<Card> m_cards;
};

#endif // CARDS_H

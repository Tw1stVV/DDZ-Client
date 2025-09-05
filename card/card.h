#ifndef CARD_H
#define CARD_H

#include "qlist.h"
class Card
{
public:
    // 花色
    enum CardSuit
    {
        Suit_Begin,
        Diamond, // 方块
        Club,    // 梅花
        Heart,   // 红心
        Spade,   // 黑桃
        Suit_End
    };
    // 点数
    enum CardPoint
    {
        Card_Begin = 2,
        Card_3,
        Card_4,
        Card_5,
        Card_6,
        Card_7,
        Card_8,
        Card_9,
        Card_10,
        Card_J,
        Card_Q,
        Card_K,
        Card_A,
        Card_2,
        Card_SJ, // small joker
        Card_BJ, // big joker
        Card_End
    };

    Card();
    Card(CardPoint point, CardSuit suit);

    CardPoint getPoint() const;
    void setPoint(CardPoint newPoint);

    CardSuit getSuit() const;
    void setSuit(CardSuit newSuit);

    QString toString() const;

private:
    CardSuit m_suit;
    CardPoint m_point;
};

using CardList = QList<Card>;

bool lessSort(const Card& c1, const Card& c2);
bool greaterSort(const Card& c1, const Card& c2);

bool operator==(const Card& left, const Card& right);
bool operator<(const Card& left, const Card& right);
uint qHash(const Card& card);

#endif // CARD_H

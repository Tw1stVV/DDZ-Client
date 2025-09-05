#include "card.h"

Card::Card()
{
}

Card::Card(CardPoint point, CardSuit suit) : m_suit(suit), m_point(point)
{
}

Card::CardPoint Card::getPoint() const
{
    return m_point;
}

void Card::setPoint(CardPoint newPoint)
{
    m_point = newPoint;
}

Card::CardSuit Card::getSuit() const
{
    return m_suit;
}

void Card::setSuit(CardSuit newSuit)
{
    m_suit = newSuit;
}

QString Card::toString() const
{
    switch (m_point)
    {
        case Card_3:
            return "3";
        case Card_4:
            return "4";
        case Card_5:
            return "5";
        case Card_6:
            return "6";
        case Card_7:
            return "7";
        case Card_8:
            return "8";
        case Card_9:
            return "9";
        case Card_10:
            return "10";
        case Card_J:
            return "J";
        case Card_Q:
            return "Q";
        case Card_K:
            return "K";
        case Card_A:
            return "A";
        case Card_2:
            return "2";
        case Card_SJ:
            return "SJ";
        case Card_BJ:
            return "BJ";
        default:
            return "";
    }
}
bool lessSort(const Card& c1, const Card& c2)
{
    if (c1.getPoint() == c2.getPoint())
    {
        return c1.getSuit() < c2.getSuit();
    }
    else
    {
        return c1.getPoint() < c2.getPoint();
    }
}

bool greaterSort(const Card& c1, const Card& c2)
{
    return !lessSort(c1, c2);
}

bool operator==(const Card& left, const Card& right)
{
    return (left.getPoint() == right.getPoint()) && (left.getSuit() == right.getSuit());
}

uint qHash(const Card& card)
{
    return card.getPoint() * 100 + card.getSuit();
}

bool operator<(const Card& left, const Card& right)
{
    return lessSort(left, right);
}

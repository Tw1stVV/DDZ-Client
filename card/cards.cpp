#include "cards.h"
#include "qdebug.h"

#include <QRandomGenerator>

Cards::Cards()
{
}

Cards::Cards(Card card)
{
    add(card);
}

void Cards::add(const Card& card)
{
    m_cards.insert(card);
}

void Cards::add(const Cards& cards)
{
    m_cards.unite(cards.m_cards);
}

void Cards::add(const QList<Cards>& cardsList)
{
    for (int i = 0; i < cardsList.size(); ++i)
    {
        this->add(cardsList.at(i));
    }
}

Cards& Cards::operator<<(const Card& card)
{
    add(card);
    return *this;
}

Cards& Cards::operator<<(const Cards& cards)
{
    add(cards);
    return *this;
}

void Cards::remove(const Card& card)
{
    m_cards.remove(card);
}

void Cards::remove(const Cards& cards)
{
    m_cards.subtract(cards.m_cards);
}

void Cards::remove(const QList<Cards> list)
{
    for (int i = 0; i < list.size(); ++i)
    {
        this->remove(list.at(i));
    }
}

int Cards::cardCount() const
{
    return m_cards.size();
}

bool Cards::isEmpty() const
{
    return m_cards.isEmpty();
}

void Cards::clear()
{
    m_cards.clear();
}

Card::CardPoint Cards::maxPoint()
{
    Card::CardPoint max = Card::Card_Begin;
    if (!m_cards.isEmpty())
    {
        for (auto it = m_cards.begin(); it != m_cards.end(); ++it)
        {
            if (it->getPoint() > max)
            {
                max = it->getPoint();
            }
        }
    }
    return max;
}

Card::CardPoint Cards::minPoint()
{
    Card::CardPoint min = Card::Card_End;
    if (!m_cards.isEmpty())
    {
        for (auto it = m_cards.begin(); it != m_cards.end(); ++it)
        {
            if (it->getPoint() < min)
            {
                min = it->getPoint();
            }
        }
    }
    return min;
}

int Cards::pointCount(Card::CardPoint point)
{
    int count = 0;
    for (auto it = m_cards.begin(); it != m_cards.end(); ++it)
    {
        if (it->getPoint() == point)
        {
            count++;
        }
    }
    return count;
}

int Cards::pointCount(Card::CardPoint point, const CardList& list)
{
    int count = 0;
    for (auto it = list.begin(); it != list.end(); ++it)
    {
        if (it->getPoint() == point)
        {
            count++;
        }
    }
    return count;
}

bool Cards::contains(const Card& card)
{
    return m_cards.contains(card);
}

bool Cards::contails(const Cards& cards)
{
    return m_cards.contains(cards.m_cards);
}

Card Cards::takeRandCard()
{
    // 随机取出一张牌
    int num = QRandomGenerator::global()->bounded(m_cards.size());
    QSet<Card>::const_iterator it = m_cards.constBegin();
    for (int i = 0; i < num; ++it, ++i);
    Card card = *it;
    m_cards.erase(it);
    return card;
}

CardList Cards::sort()
{
    // 降序排序
    CardList list = toCardList();
    CardList temp;

    // 将三张或炸弹从牌中取出来
    for (int i = 0; i < list.size(); ++i)
    {
        int count = pointCount(list.at(i).getPoint(), list);
        if (count == 3 || count == 4)
        {
            for (int j = i; j < i + count; ++j)
            {
                temp << list.at(j);
            }
            i += count - 1;
        }
    }

    if (temp.isEmpty())
        return list;

    // 将三张或炸弹从牌中删除
    for (int i = 0; i < temp.size(); ++i)
    {
        list.removeAll(temp.at(i));
    }
    // 将主牌排在亲前，副牌在后
    temp << list;

    return temp;
}

CardList Cards::toCardList(sortType type) const
{
    CardList list;
    for (auto i = m_cards.begin(); i != m_cards.end(); ++i)
    {
        list << *i;
    }
    if (type == Asc)
    {
        std::sort(list.begin(), list.end(), lessSort);
    }
    else if (type == Desc)
    {
        std::sort(list.begin(), list.end(), greaterSort);
    }
    return list;
}

Cards Cards::toCards(const CardList& list)
{
    Cards cards;
    foreach (const Card& card, list)
    {
        cards.m_cards.insert(card);
    }
    return cards;
}

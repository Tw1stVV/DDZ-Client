
#include "strategy.h"
#include <QMap>
#include <functional>

Strategy::Strategy(Player* player, const Cards& cards)
{
    m_player = player;
    m_cards = cards;
}

Cards Strategy::makeStrategy()
{
    // 得到出牌玩家和打出的牌
    Player* pendPlay = m_player->getPendPlayer();
    Cards pendCards = m_player->getPendCards();

    // 判断上次出牌的玩家是不是自己
    if (pendPlay == m_player || pendPlay == nullptr)
    {
        // 上轮出牌的玩家是自己，或自己是本局游戏第一次出牌的玩家，出牌没有限制
        return firstPlay();
    }
    else
    {
        // 如果不是，找出比出牌玩家打出的牌的点数更大的牌
        PlayHand type(pendCards);
        Cards beatCards = getGreaterCards(type);

        // 找到点数更大的牌考虑是否出牌
        bool isBeat = whetherToBeat(beatCards);
        if (isBeat)
            return beatCards;
        else
            return Cards();
    }
    return Cards();
}

Cards Strategy::firstPlay()
{
    // 玩家手中是否只剩单一类型的牌
    PlayHand hand(m_cards);
    if (hand.type() != PlayHand::Hand_UnKnow)
        return m_cards;

    // 多种类型的牌
    // 判断是否有顺子
    QList<Cards> list = pickOptimalSeqSingles();
    if (!list.isEmpty())
    {
        // 对比出顺子前剩下的单牌和出顺子后剩下的单牌来判断要不要出顺子
        int beforePlay = this->findCardsByCount(1).size();
        Cards backup = m_cards;
        backup.remove(list);
        int afterPlay = Strategy(m_player, backup).findCardsByCount(1).size();
        if (beforePlay > afterPlay)
        {
            return list.first();
        }
    }

    bool hasPlane, hasSeqPair, hasTriple;
    hasPlane = hasSeqPair = hasTriple = false;
    Cards backup = m_cards;

    // 将炸弹，飞机，三张，连对从手牌中剔除，删除顺序不能变，否则搜索连对或三张时可能会将炸弹拆掉
    QList<Cards> bombArray =
        Strategy(m_player, backup)
            .findCardType(PlayHand(PlayHand::Hand_Bomb, Card::Card_Begin, 0), false);
    // 先手出牌不考虑出炸弹，直接将炸弹剔除,除非手里只剩下多个炸弹
    backup.remove(bombArray);
    if (backup.isEmpty() && !bombArray.isEmpty())
    {
        Cards temp = bombArray[0];
        return temp;
    }

    // 飞机
    QList<Cards> planeArray =
        Strategy(m_player, backup)
            .findCardType(PlayHand(PlayHand::Hand_Plane, Card::Card_Begin, 0), false);
    if (!planeArray.isEmpty())
    {
        hasPlane = true;
        backup.remove(planeArray);
    }

    // 三张
    QList<Cards> tripleArray =
        Strategy(m_player, backup)
            .findCardType(PlayHand(PlayHand::Hand_Triple, Card::Card_Begin, 0), false);
    if (!tripleArray.isEmpty())
    {
        hasTriple = true;
        backup.remove(tripleArray);
    }

    // 连对
    QList<Cards> pairSeqArray =
        Strategy(m_player, backup)
            .findCardType(PlayHand(PlayHand::Hand_Seq_Pair, Card::Card_Begin, 0), false);
    if (!pairSeqArray.isEmpty())
    {
        hasSeqPair = true;
        backup.remove(pairSeqArray);
    }

    // 将炸弹，飞机，三张，连对剔除之后，手牌中只剩零散的单张和对子

    // 出牌考虑: 连对>飞机>三带一或三带二
    // 连对
    if (hasSeqPair)
    {
        // 打最长的连对
        Cards maxPair;
        for (int i = 0; i < pairSeqArray.size(); ++i)
        {
            if (pairSeqArray.at(i).cardCount() > maxPair.cardCount())
                maxPair = pairSeqArray.at(i);
        }
        return maxPair;
    }

    // 飞机
    if (hasPlane)
    {
        // 飞机带两对
        bool twoPairFound = false;
        QList<Cards> pairArray;
        for (Card::CardPoint point = Card::Card_Begin; point <= Card::Card_K;
             point = (Card::CardPoint)(point + 1))
        {
            Cards pair = Strategy(m_player, backup).findSamePointCards(point, 2);
            if (!pair.isEmpty())
            {
                pairArray << pair;
                // 找到两个对退出
                if (pairArray.size() == 2)
                {
                    twoPairFound = true;
                    break;
                }
            }
        }
        if (twoPairFound)
        {
            Cards temp = planeArray[0];
            temp.add(pairArray);
            return temp;
        }
        // 找不到两个对就找两张
        else
        {
            // 飞机带两张
            bool twoSingleFound = false;
            QList<Cards> singleArray;
            for (Card::CardPoint point = Card::Card_Begin; point <= Card::Card_K;
                 point = (Card::CardPoint)(point + 1))
            {
                Cards single = Strategy(m_player, backup).findSamePointCards(point, 1);
                // 只找单牌，不拆对子
                if (backup.pointCount(point) == 1)
                {
                    singleArray << single;
                    if (singleArray.size() == 2)
                    {
                        twoSingleFound = true;
                        break;
                    }
                }
            }
            if (twoSingleFound)
            {
                Cards temp = planeArray[0];
                temp.add(singleArray);
                return temp;
            }
            else
            {
                // 找不到两张打裸飞机
                return planeArray[0];
            }
        }
    }

    // 三带一或三带二
    if (hasTriple)
    {
        // 三张太大不先手出
        if (PlayHand(tripleArray[0]).point() < Card::Card_K)
        {
            for (Card::CardPoint point = Card::Card_3; point < Card::Card_A;
                 point = (Card::CardPoint)(point + 1))
            {
                // 不将对子拆成单牌，只找单牌
                int pointCount = backup.pointCount(point);
                if (pointCount == 1)
                {
                    Cards single = Strategy(m_player, backup).findSamePointCards(point, 1);
                    Cards temp = tripleArray[0];
                    temp << single;
                    return temp;
                }
                else if (pointCount == 2)
                {
                    Cards pair = Strategy(m_player, backup).findSamePointCards(point, 2);
                    Cards temp = tripleArray[0];
                    temp << pair;
                    return temp;
                }
            }
            // 没有副牌就打三张
            return tripleArray[0];
        }
    }

    // 执行到这说明玩家手中只有单张牌或对牌
    Player* nextPlayer = m_player->next();
    // 下家和玩家不是一个阵营且手中的牌只有一张，玩家需要用大的单牌或对子压住下家
    if (nextPlayer->getCards().cardCount() == 1 && nextPlayer->role() != m_player->role())
    {
        // 从大牌往小牌找
        for (Card::CardPoint point = Card::Card_BJ; point > Card::Card_Begin;
             point = (Card::CardPoint)(point - 1))
        {
            int pointCount = backup.pointCount(point);
            if (pointCount == 1)
            {
                Cards single = Strategy(m_player, backup).findSamePointCards(point, 1);
                return single;
            }
            else if (pointCount == 2)
            {
                Cards pair = Strategy(m_player, backup).findSamePointCards(point, 2);
                return pair;
            }
        }
    }
    else
    {
        // 下家是队友，打出小牌
        for (Card::CardPoint point = Card::Card_Begin; point < Card::Card_End;
             point = (Card::CardPoint)(point + 1))
        {
            int pointCount = backup.pointCount(point);
            if (pointCount == 1)
            {
                Cards single = Strategy(m_player, backup).findSamePointCards(point, 1);
                return single;
            }
            else if (pointCount == 2)
            {
                Cards pair = Strategy(m_player, backup).findSamePointCards(point, 2);
                return pair;
            }
        }
    }

    return Cards();
}

Cards Strategy::getGreaterCards(PlayHand type)
{
    // 1. 自己和出牌方不属于同一阵营
    Player* pendPlayer = m_player->getPendPlayer();
    if (pendPlayer->role() != m_player->role() && pendPlayer->getCards().cardCount() <= 3)
    {
        // 出牌方剩余的牌小于等于四张时，需要拿出最大的炸弹压制他
        QList<Cards> bombs = findCardsByCount(4);
        for (int i = 0; i < bombs.size(); ++i)
        {
            if (PlayHand(bombs[i]).canbeat(type))
                return bombs[i];
        }

        // 搜索王炸
        Cards sj = findSamePointCards(Card::Card_SJ, 1);
        Cards bj = findSamePointCards(Card::Card_BJ, 1);
        if (!sj.isEmpty() && !bj.isEmpty())
        {
            Cards tmp;
            tmp << sj << bj;
            return tmp;
        }
    }

    // 2. 自己和下家不属于同一阵营
    Player* nextPlayer = m_player->next();
    // 剔除自己牌中的顺子在查找可以打的牌
    Cards remain = m_cards;
    remain.remove(Strategy(m_player, remain).pickOptimalSeqSingles());

    auto beatCards = std::bind(
        [=](Cards& cards)
        {
            QList<Cards> beatCardsArray = Strategy(m_player, cards).findCardType(type, true);
            // 在剔除顺子后仍然找的到可以打的牌
            if (!beatCardsArray.isEmpty())
            {
                if (m_player->role() != nextPlayer->role()
                    && nextPlayer->getCards().cardCount() <= 2)
                {
                    // 打大牌
                    return beatCardsArray.back();
                }
                else
                {
                    // 打小牌
                    return beatCardsArray.first();
                }
            }
            return Cards();
        },
        std::placeholders::_1);

    Cards cs;
    if (!(cs = beatCards(remain)).isEmpty())
    { // 在剔除顺子后找的到可以打的牌
        return cs;
    }
    else
    { // 在剔除顺子后找不到可以打的牌，将顺子也纳入搜索范围
        if (!(cs = beatCards(m_cards)).isEmpty())
            return cs;
    }
    return Cards();
}

bool Strategy::whetherToBeat(Cards cards)
{
    if (cards.isEmpty())
        return false;

    Player* pendPlayer = m_player->getPendPlayer();
    // 判断自己和出牌方是否属于同一阵营
    if (m_player->role() == pendPlayer->role())
    {
        // 如果将cards打出去之后，手里的牌还剩一个完整的牌型，下一轮有机会就可以打完，就将cards打出
        Cards left = m_cards;
        left.remove(cards);
        if (PlayHand(left).type() != PlayHand::Hand_UnKnow)
            return true;

        // 如果cards是2，大小王，不压队友，让队友过
        Card::CardPoint point = PlayHand(cards).point();
        if (point == Card::Card_2 || point == Card::Card_SJ || point == Card::Card_BJ)
            return false;
    }
    else
    {
        PlayHand hand(cards);
        // 如果牌型是三带一或三带二并且点数是2，不出牌
        if ((hand.type() == PlayHand::Hand_Triple_Single
             || hand.type() == PlayHand::Hand_Triple_Pair)
            && hand.point() == Card::Card_2)
        {
            return false;
        }

        // 如果cards是对2，并且出牌玩家手中的牌大于等于10张，自己手中的牌大于等于5张，暂时放弃出牌
        if (hand.point() == Card::Card_2 && hand.type() == PlayHand::Hand_Pair
            && pendPlayer->getCards().cardCount() >= 10 && m_player->getCards().cardCount() >= 5)
        {
            return false;
        }
    }
    return true;
}

Cards Strategy::findSamePointCards(Card::CardPoint point, int count)
{
    if (count < 1 || count > 4)
        return Cards();

    // 大小王
    if (point == Card::Card_SJ || point == Card::Card_BJ)
    {
        // 大小王只有一张
        if (count > 1)
        {
            return Cards();
        }

        Card card(point, Card::Suit_Begin);
        if (m_cards.contains(card))
        {
            Cards cards(card);
            return cards;
        }
    }

    // 不是大小王的牌有四个花色，根据point在m_cards遍历四个花色，结果等于count表示找到count个point点数的牌
    int findCount = 0;
    Cards findCards;
    for (int suit = Card::Suit_Begin + 1; suit < Card::Suit_End; ++suit)
    {
        Card card(point, (Card::CardSuit)suit);
        if (m_cards.contains(card))
        {
            findCards.add(card);
            findCount++;
            if (findCount == count)
                return findCards;
        }
    }

    return Cards();
}

QList<Cards> Strategy::findCardsByCount(int count)
{
    if (count < 1 || count > 4)
        return QList<Cards>();

    QList<Cards> cardsArray;
    for (int point = Card::Card_3; point < Card::Card_End; ++point)
    {
        if (m_cards.pointCount((Card::CardPoint)point) == count)
        {
            cardsArray << findSamePointCards((Card::CardPoint)point, count);
        }
    }
    return cardsArray;
}

Cards Strategy::getRangeCards(Card::CardPoint begin, Card::CardPoint end)
{
    Cards cardsRange;
    for (int point = begin; point < end; ++point)
    {
        int count = m_cards.pointCount((Card::CardPoint)point);
        cardsRange << findSamePointCards((Card::CardPoint)point, count);
    }
    return cardsRange;
}

QList<Cards> Strategy::findCardType(PlayHand hand, bool beat)
{
    PlayHand::HandType type = hand.type();
    Card::CardPoint point = hand.point();
    int extra = hand.extra();

    // 确定起始点数
    Card::CardPoint begin = beat ? (Card::CardPoint)(point + 1) : Card::Card_3;

    switch (type)
    {
        case PlayHand::Hand_Single:
            return getCards(begin, 1);
        case PlayHand::Hand_Pair:
            return getCards(begin, 2);
        case PlayHand::Hand_Triple:
            return getCards(begin, 3);
        case PlayHand::Hand_Triple_Single:
            return getTripleSingleOrPair(begin, PlayHand::Hand_Single);
        case PlayHand::Hand_Triple_Pair:
            return getTripleSingleOrPair(begin, PlayHand::Hand_Pair);
        case PlayHand::Hand_Plane:
            return getPlane(begin);
        case PlayHand::Hand_Plane_Two_Single:
            return getPlaneTwoSingleOrTwoPair(begin, PlayHand::Hand_Single);
        case PlayHand::Hand_Plane_Two_pair:
            return getPlaneTwoSingleOrTwoPair(begin, PlayHand::Hand_Pair);
        case PlayHand::Hand_Seq_Pair:
        {
            CardInfo info;
            info.begin = begin;
            info.end = Card::Card_Q;
            info.extra = extra;
            info.beat = beat;
            info.number = 2;
            info.base = 3;
            info.getSeq = &Strategy::getSeqPair;
            return getSeqSingleOrSeqPair(info);
        }
        case PlayHand::Hand_Seq_Single:
        {
            CardInfo info;
            info.begin = begin;
            info.end = Card::Card_10;
            info.extra = extra;
            info.beat = beat;
            info.number = 1;
            info.base = 5;
            info.getSeq = &Strategy::getSeqSingle;
            return getSeqSingleOrSeqPair(info);
        }
        case PlayHand::Hand_Bomb:
            return getBomb(begin);
        default:
            return QList<Cards>();
    }
    return QList<Cards>();
}

// 从cards中挑选满足条件的顺子
/*
 *这个条件是指：在拥有的扑克牌中，可以打出的顺子
 *  这是findCardType选出的顺子组合，它们是不能实际打出来的牌。假如你有3~9这个顺子，你打出了3~8，这时只剩了一张9，不可能在打出一个3~9的顺子
 *  3,4,5,6,7,8,
 *  3,4,5,6,7,8,9
 *  ....
 *  函数的功能是找到一个可以打出的顺子的集合的集合
 *  如: 3,4,5,6,7,8,9,10,J,Q,K,A
 *  3~7 and 10~A是一个可以打出的集合
 *  ....
 *  3~9 and 10~A 是一个可以打出的集合
 *  3~A 是一个可以打出的集合，集合里的牌数=1
 */
void Strategy::pickSeqSingles(
    QList<QList<Cards>>& allSeqRecord, QList<Cards>& seqSingle, const Cards& cards)
{
    // 获取cards里所有可能的顺子组合
    QList<Cards> allSeq =
        Strategy(m_player, cards)
            .findCardType(PlayHand(PlayHand::Hand_Seq_Single, Card::Card_Begin, 0), false);
    if (allSeq.isEmpty())
    {
        // 递归出口，表示参数cards里已经没有顺子
        allSeqRecord << seqSingle;
    }
    else
    {
        Cards saveCards = cards;
        for (int i = 0; i < allSeq.size(); ++i)
        {
            // 取出顺子
            Cards aScheme = allSeq.at(i);

            // 将取出的顺子从牌里删除再搜索牌里其他的顺子
            Cards remain = saveCards;
            remain.remove(aScheme);

            QList<Cards> seqArray = seqSingle;
            // 本次找到的顺子保存到seqArray
            seqArray << aScheme;

            // seqArray作为参数传给pickSeqSingles，递归调用结束时将找到的所有独立的顺子保存到allSeqRecord
            pickSeqSingles(allSeqRecord, seqArray, remain);
        }
    }
}

QList<Cards> Strategy::pickOptimalSeqSingles()
{
    QList<QList<Cards>> seqRecord;
    QList<Cards> seqSingles;
    Cards backup = m_cards;
    // 不从三张和炸弹里拆顺子
    backup.remove(findCardsByCount(4));
    backup.remove(findCardsByCount(3));
    this->pickSeqSingles(seqRecord, seqSingles, backup);

    if (seqRecord.isEmpty())
        return QList<Cards>();

    QMap<int, int> markMap;
    for (int i = 0; i < seqRecord.size(); ++i)
    {
        // 获取剔除掉所有顺子后剩下的单牌
        Cards remain = m_cards;
        QList<Cards> seqArray = seqRecord.at(i);
        remain.remove(seqArray);

        // 剩下的单排数量越少，顺子就越好
        QList<Cards> singleArray = Strategy(m_player, remain).findCardsByCount(1);
        // 将QList<Cards>转换成QList<Card>=CardList方便后续操作
        CardList cardList;
        for (int j = 0; j < singleArray.size(); ++j)
        {
            cardList << singleArray[j].toCardList();
        }

        // 单牌越小->顺子点数越大
        // 单牌越大->顺子点数越小
        // 剔除顺子主要是为了getGreaterCards()函数不拆散顺子之后选出合适的牌
        // 这里选择留点数大的牌，留点数小的顺子

        int mark = 0;
        for (int j = 0; j < cardList.size(); ++j)
        {
            mark += cardList.at(j).getPoint();
        }
        markMap.insert(i, mark);
    }

    // 选出mark最高的QList<Cards>
    // key是seqRecord的索引，value是seqRecord[key]的mark值
    int value = 0;
    int key = 0;
    for (auto it = markMap.constBegin(); it != markMap.constEnd(); ++it)
    {
        if (it.value() > value)
        {
            value = it.value();
            key = it.key();
        }
    }
    return seqRecord[key];
}

QList<Cards> Strategy::getCards(Card::CardPoint point, int number)
{
    QList<Cards> findCardsList;
    for (int pt = point; pt < Card::Card_End; pt++)
    {
        // 尽量不拆分别的牌型
        if (m_cards.pointCount((Card::CardPoint)pt) == number)
        {
            Cards cs = findSamePointCards((Card::CardPoint)pt, number);
            findCardsList << cs;
        }
    }

    return findCardsList;
}

QList<Cards> Strategy::getTripleSingleOrPair(Card::CardPoint begin, PlayHand::HandType type)
{
    // 获取三张点数相同的牌的集合
    QList<Cards> findCardsArray = getCards(begin, 3);
    if (!findCardsArray.isEmpty())
    {
        // 在副本中删除这些集合
        Cards remainCard = m_cards;
        remainCard.remove(findCardsArray);

        // 在剩余的牌中选择后面带的单牌或对牌
        Strategy st(m_player, remainCard);
        // 因为三带一或三带二不对后面带的牌有大小要求，所有PlayHand(type, Card::Card_Begin,
        // 0)从最小的开始找
        QList<Cards> cardArray = st.findCardType(PlayHand(type, Card::Card_Begin, 0), false);
        if (!cardArray.isEmpty())
        {
            // 找到后和前面的三张牌组合起来
            for (int i = 0; i < findCardsArray.size(); ++i)
            {
                findCardsArray[i].add(cardArray.at(i));
            }
        }
        else
        {
            // 找不到返回空的容器
            findCardsArray.clear();
        }
    }
    return findCardsArray;
}

QList<Cards> Strategy::getPlane(Card::CardPoint begin)
{
    QList<Cards> findCardsArray;
    for (Card::CardPoint point = begin; point <= Card::Card_K; point = (Card::CardPoint)(point + 1))
    {
        // 根据点数和数量搜索飞机
        Cards previousCards = findSamePointCards(point, 3);
        Cards nextCards = findSamePointCards((Card::CardPoint)(point + 1), 3);
        if (!previousCards.isEmpty() && !nextCards.isEmpty())
        {
            Cards tmp;
            tmp << previousCards << nextCards;
            findCardsArray << tmp;
        }
    }
    return findCardsArray;
}

QList<Cards> Strategy::getPlaneTwoSingleOrTwoPair(Card::CardPoint begin, PlayHand::HandType type)
{
    // 获取飞机
    QList<Cards> findCarsdArray = getPlane(begin);
    if (!findCarsdArray.isEmpty())
    {
        // 在副本中删除这些集合
        Cards remainCard = m_cards;
        remainCard.remove(findCarsdArray);

        // 在剩余的牌中选择后面带的单牌或对牌
        Strategy st(m_player, remainCard);
        // 因为飞机不对后面带的牌有大小要求，所有PlayHand(type, Card::Card_Begin,
        // 0)从最小的开始找
        QList<Cards> cardArray = st.findCardType(PlayHand(type, Card::Card_Begin, 0), false);
        if (cardArray.size() >= 2)
        {
            // 找到后和前面的飞机组合起来
            for (int i = 0; i < findCarsdArray.size(); ++i)
            {
                Cards temp;
                temp << cardArray[0] << cardArray[1];
                findCarsdArray[i].add(temp);
            }
        }
        else
        {
            // 找不到返回空的容器
            findCarsdArray.clear();
        }
    }
    return findCarsdArray;
}

QList<Cards> Strategy::getSeqSingleOrSeqPair(CardInfo info)
{
    QList<Cards> findCarsdArray;
    // 受到extra这个数值的连对长度限制
    if (info.beat)
    {
        // 连对最小需要三个，最多可以连到A
        // 只搜索满足条件的最小连对
        for (Card::CardPoint point = info.begin; point <= info.end;
             point = (Card::CardPoint)(point + 1))
        {
            bool found = true;
            Cards seqCards;

            // 里层循环搜索point开始往后extra张牌是否是连续的对子
            for (int i = 0; i < info.extra; ++i)
            {
                Cards cards = findSamePointCards((Card::CardPoint)(point + i), info.number);
                if (cards.isEmpty() || (Card::CardPoint)(point + info.extra) >= Card::Card_2)
                {
                    found = false;
                    seqCards.clear();
                    break;
                }
                else
                    seqCards << cards;
            }
            if (found)
            {
                findCarsdArray << seqCards;
                return findCarsdArray;
            }
        }
    }
    // 不受到extra这个数值的连对长度限制
    else
    {
        for (Card::CardPoint point = info.begin; point <= info.end;
             point = (Card::CardPoint)(point + 1))
        {
            // 将这个最基础的连对存储起来
            Cards baseSeq = (this->*info.getSeq)(point);
            if (baseSeq.isEmpty())
                continue;

            findCarsdArray << baseSeq;

            int followed = info.base;
            Cards alreadyFollowedCards; // 累加存储后续找到的连对
            /*
             *while循环找到了满足调整的对,将新的对子添加到alreadyFollowedCards
             *在将baseSeq和alreadyFollowedCards组合起来添加到findCarsdArray
             *findCarsdArray会是33,44,55  33,44,55,66     33,44,55,66,77.....这样的组合
             */
            while (true)
            {
                Card::CardPoint followedPoint = (Card::CardPoint)(point + followed);
                // 是否超出连对上限
                if (followedPoint >= Card::Card_2)
                    break;

                // 判断是否是对子
                Cards followedCards = findSamePointCards(followedPoint, info.number);
                if (followedCards.isEmpty())
                    break;
                else
                {
                    // 与基础连对组合起来加入findCarsdArray
                    alreadyFollowedCards << followedCards;
                    Cards newCards = baseSeq;
                    newCards << alreadyFollowedCards;
                    // findCarsdArray是存储所有连对的数组
                    // 如：33,44,55    33,44,55,66     44,55,66,77
                    findCarsdArray << newCards;
                    followed++;
                }
            }
        }
    }
    return findCarsdArray;
}

QList<Cards> Strategy::getBomb(Card::CardPoint begin)
{
    QList<Cards> findCarsdArray;
    for (Card::CardPoint point = begin; point < Card::Card_End;
         point = (Card::CardPoint)(point + 1))
    {
        Cards cs = findSamePointCards(point, 4);
        if (!cs.isEmpty())
            findCarsdArray << cs;
    }
    return findCarsdArray;
}

Cards Strategy::getSeqSingle(Card::CardPoint point)
{
    Cards c0 = findSamePointCards(point, 1);
    Cards c1 = findSamePointCards((Card::CardPoint)(point + 1), 1);
    Cards c2 = findSamePointCards((Card::CardPoint)(point + 2), 1);
    Cards c3 = findSamePointCards((Card::CardPoint)(point + 3), 1);
    Cards c4 = findSamePointCards((Card::CardPoint)(point + 4), 1);
    Cards seq;
    if (!c0.isEmpty() && !c1.isEmpty() && !c2.isEmpty() && !c3.isEmpty() && !c4.isEmpty())
        seq << c0 << c1 << c2 << c3 << c4;
    return seq;
}

Cards Strategy::getSeqPair(Card::CardPoint point)
{
    Cards c0 = findSamePointCards(point, 2);
    Cards c1 = findSamePointCards((Card::CardPoint)(point + 1), 2);
    Cards c2 = findSamePointCards((Card::CardPoint)(point + 2), 2);
    Cards seq;
    if (!c0.isEmpty() && !c1.isEmpty() && !c2.isEmpty())
        seq << c0 << c1 << c2;
    return seq;
}

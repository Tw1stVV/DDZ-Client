#ifndef STRATEGY_H
#define STRATEGY_H
#include "player.h"
#include "playhand.h"

enum GameStage
{
    Begin,
    Middle,
    End
};

class Strategy
{
public:
    Strategy(Player* player, const Cards& cards);

    // 1. 制定出牌策略
    Cards makeStrategy();

    // 2. 本轮第一个出牌，或本局游戏第一个出牌
    Cards firstPlay();

    // 3. 得到比指定牌型大的牌
    Cards getGreaterCards(PlayHand type);

    // 4. 能大过指定的牌时，判断是出牌还是过 true->出牌 false->过
    bool whetherToBeat(Cards cards);

    // 5. 找出count张点数为point的牌
    Cards findSamePointCards(Card::CardPoint point, int count);

    // 6. 找出所有点数数量为count的牌，得到一个多张扑克牌数组
    // (count == 3找到所有数量是3的牌，不在乎点数)
    QList<Cards> findCardsByCount(int count);

    // 7. 根据点数范围找牌
    Cards getRangeCards(Card::CardPoint begin, Card::CardPoint end);
    // 8. 根据牌型找牌，并指定要找的牌是否要大于指定的牌
    QList<Cards> findCardType(PlayHand hand, bool beat);

    // 9. 从cards中挑选满足条件的顺子
    void
    pickSeqSingles(QList<QList<Cards>>& allSeqRecord, QList<Cards>& seqSingle, const Cards& cards);

    QList<Cards> pickOptimalSeqSingles();

private:
    using function = Cards (Strategy::*)(Card::CardPoint point);
    struct CardInfo
    {
        Card::CardPoint begin;
        Card::CardPoint end; // 顺子或连对第一张牌的最大点数:10 or Q
        int extra;           // 数量
        bool beat;
        int number;      // 搜索同点数牌的数量:顺子1 连对2
        int base;        // 顺子或连对最小数量:5 or 3
        function getSeq; // 函数指针根据不同实例调用对应函数
    };

    QList<Cards> getCards(Card::CardPoint point, int number);

    QList<Cards> getTripleSingleOrPair(Card::CardPoint begin, PlayHand::HandType type);

    QList<Cards> getPlane(Card::CardPoint begin);

    QList<Cards> getPlaneTwoSingleOrTwoPair(Card::CardPoint begin, PlayHand::HandType type);

    // beat为true表示寻找连对需要满足最大长度是extra的限制
    // beat为false表示寻找连对不受extra的长度限制
    QList<Cards> getSeqSingleOrSeqPair(CardInfo info);

    QList<Cards> getBomb(Card::CardPoint begin);

    Cards getSeqSingle(Card::CardPoint point);
    Cards getSeqPair(Card::CardPoint point);

private:
    Player* m_player;
    Cards m_cards;
};

#endif // STRATEGY_H

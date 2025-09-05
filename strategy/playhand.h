#ifndef PLAYHAND_H
#define PLAYHAND_H
#include "cards.h"

struct CardGroup;

class PlayHand
{
public:
    // 出牌组合
    enum HandType
    {
        Hand_UnKnow, // 未知
        Hand_Pass,   // 过

        Hand_Single, // 单
        Hand_Pair,   // 对

        Hand_Triple,        // 三
        Hand_Triple_Single, // 三带一
        Hand_Triple_Pair,   // 三带对

        Hand_Plane,            // 飞机~~
        Hand_Plane_Two_Single, // 飞机带两单 444,555,6,7
        Hand_Plane_Two_pair,   // 飞机带两对 444,555,66,77

        Hand_Seq_Pair,   // 连对
        Hand_Seq_Single, // 顺子

        Hand_Bomb,            // 炸弹
        Hand_Bomb_Single,     // 炸弹带一张
        Hand_Bomb_Pair,       // 炸弹带一对
        Hand_Bomb_Two_Single, // 炸弹带两张

        Hand_Bomb_Jokers,           // 王炸
        Hand_Bomb_Jokers_Single,    // 王炸带一张
        Hand_Bomb_Jokers_Pair,      // 王炸带一对
        Hand_Bomb_Jokers_Two_Single // 王炸带两对
    };

public:
    PlayHand();

    // 传递给类一组牌，分析出牌型，点数，以及相关的附属信息(如连对有多少对，顺子有多少张)
    PlayHand(const Cards& cards);
    PlayHand(HandType type, Card::CardPoint point, int extra);

    Card::CardPoint point() const;
    HandType type() const;
    int extra() const;

    // 比较双方牌型大小
    bool canbeat(const PlayHand& other);

    // 获取牌型权值
    static CardGroup getCardGroup(HandType type, int major, int count);

    Card::CardPoint valueToCardPoint(int value);

    int cardPointToValue(Card::CardPoint point);

    QList<Card::CardPoint> OneCards() const;
    QList<Card::CardPoint> TwoCards() const;
    QList<Card::CardPoint> ThreeCards() const;
    QList<Card::CardPoint> FourCards() const;

    // 对扑克牌进行排序，主牌在前，副牌在后
    // Cards sort();

private:
    // 对扑克牌进行分类：1张的，2张的，3张的，4张的有多少种
    void classify(const Cards& cards);

    // 对牌型进行分类
    void judgeCardType();

    // 判断牌的类型
    bool isPass();                // 过
    bool isSingle();              // 单
    bool isPair();                // 对
    bool isTriple();              // 三张
    bool isTripleSingle();        // 三带一
    bool isTriplePair();          // 三带对
    bool isPlane();               // 飞机
    bool isPlaneTwoSingle();      // 飞机带两张
    bool isPlaneTwoPair();        // 飞机带两对
    bool isSeqPair();             // 连对
    bool isSeqSingle();           // 顺子
    bool isBomb();                // 炸弹
    bool isBombSingle();          // 炸弹带一张
    bool isBombPair();            // 炸弹带一对
    bool isBombTwoSingle();       // 炸弹带两张
    bool isBombJokers();          // 王炸！！！！！！
    bool isBombJokersSingle();    // 王炸带一张
    bool isBombJokersPair();      // 王炸带一对
    bool isBombJokersTwoSingle(); // 王炸带两张

private:
    Card::CardPoint m_point; //
    HandType m_type;
    int m_extra;
    QList<Card::CardPoint> m_oneCard;   // 存储单张牌        如将3，5，7存进来，表示3，5，7是单张
    QList<Card::CardPoint> m_twoCard;   // 存储对牌          如将3，5，7存进来，表示对3，对5，对7
    QList<Card::CardPoint> m_threeCard; // 存储三张的牌 如将3，5，7存进来，表示3个3，三个5，三个7
    QList<Card::CardPoint> m_fourCard;  // 存储炸        如将3，5，7存进来，表示3炸，5炸，7炸
};

#endif // PLAYHAND_H

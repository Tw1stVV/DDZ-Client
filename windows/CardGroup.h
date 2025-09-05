#ifndef CARDGROUP_H
#define CARDGROUP_H
#include "playhand.h"
struct CardGroup
{
    PlayHand::HandType type; // 牌型
    int value;               // 牌的价值
    int count;               // 牌的个数
    int maxCard;             // 最大的牌

    CardGroup(
        PlayHand::HandType ty = PlayHand::Hand_UnKnow, int val = 0, int cnt = 0, int max = 0) :
        type(ty), value(val), count(cnt), maxCard(max) {};
};
#endif // CARDGROUP_H

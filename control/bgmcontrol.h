#ifndef BGMCONTROL_H
#define BGMCONTROL_H

#include <QObject>
#include <QMediaPlayer>
#include "player.h"
#include "musiclist.h"

class BGMControl : public QObject
{
    Q_OBJECT
public:
    enum MediaType
    {
        Man,
        Woman,
        BGM,
        Ending,
        Other
    };
    enum CardType
    {
        // 单
        Three,
        Foue,
        Five,
        Six,
        Seven,
        Eight,
        Nine,
        Ten,
        Jack,
        Queen,
        King,
        Ace,
        Tow,
        SmallJoker,
        BigJoker,
        // 对
        Three_Double,
        Foue_Double,
        Five_Double,
        Six_Double,
        Seven_Double,
        Eight_Double,
        Nine_Double,
        Ten_Double,
        Jack_Double,
        Queen_Double,
        King_Double,
        Ace_Double,
        Tow_Double,
        // 三张
        Three_Triple,
        Foue_Triple,
        Five_Triple,
        Six_Triple,
        Seven_Triple,
        Eight_Triple,
        Nine_Triple,
        Ten_Triple,
        Jack_Triple,
        Queen_Triple,
        King_Triple,
        Ace_Triple,
        Tow_Triple,
        // 组合牌型
        Plane,         // 飞机
        SequencePair,  // 连对
        ThreeBindOne,  // 三带一
        ThreeBindPair, // 三带一对
        Sequence,      // 顺子
        FourBindTwo,   // 四带二(单张)
        FourBind2Pair, // 四带两对
        Bomb,          // 炸弹
        JokerBomb,     // 王炸
        Pass1,         // 过
        Pass2,
        Pass3,
        Pass4,
        MoreBiger1, // 大你
        MoreBiger2,
        Biggest, // 压死
        NoOrder, // 不叫
        NoRob,   // 不抢
        Order,   // 叫地主
        Rob1,    // 抢地主
        Rob2,
        Last1, // 只剩1张牌
        Last2  // 只剩2张牌
    };

    enum AssistMusic
    {
        Dispatch,
        SelectCard,
        PlaneVoice,
        BombVoice,
        Alert
    };

    explicit BGMControl(QObject* parent = nullptr);
    void initList();
    // 播放背景音乐
    void startBGM(int volume);
    void stopBGM();

    // 抢地主时期播放的音乐
    void playerRobLordMusic(int point, Player::Sex sex, bool isFirst);

    // 出牌时音效
    void playCardsMusic(Cards cards, bool isFirst, Player::Sex sex);

    // 剩余1张或2张牌时的音效
    void playLastMusic(CardType type, Player::Sex sex);

    // 不出牌音效
    void playPassMusic(Player::Sex sex);

    // 辅助音效
    void playAssistMusic(AssistMusic type);
    void stopAssistMusic();

    // 结束音乐
    void playEndingMusic(bool isWin);

signals:

private:
    // Man Woman BGM Ending Other
    QList<QMediaPlayer*> m_players;
    QList<MusicList*> m_list;
};

#endif // BGMCONTROL_H

#ifndef GAMECONTROL_H
#define GAMECONTROL_H

#include <QObject>
#include "cards.h"
class UserPlayer;
class Robot;
class Player;

struct BetReord
{
    BetReord()
    {
        reset();
    }
    void reset()
    {
        player = nullptr;
        betPoint = 0;
        grabbingLordTimers = 0;
    }
    Player* player;         // 抢地主玩家
    int betPoint;           // 下注点数
    int grabbingLordTimers; // 本一共有几个玩家抢过地主
};

class GameControl : public QObject
{
    Q_OBJECT
public:
    explicit GameControl(QObject* parent = nullptr);
    // 游戏状态
    enum GameStatus
    {
        DispatchCord,
        CallingLord,
        PlayingHand
    };

    // 玩家状态
    enum PlayerStatus
    {
        ThinkingForCallLord,
        ThinkingForPlayHand,
        Winning
    };

    // 玩家初始化
    void playerInit();

    // 玩家实例对象
    Robot* robotLeft() const;
    Robot* robotRight() const;
    UserPlayer* user() const;
    Player* curPlayer() const;
    void setCurPlayer(Player* newCurPlayer);

    // 出牌玩家和打出的牌
    Cards pendCards() const;
    Player* pendPlayer() const;

    // 扑克牌初始化
    void initCards();

    // 发出一张牌
    Card takeOneCard();

    // 获取剩余的地主牌
    Cards getSurplusCards();

    // 重置卡牌数据
    void resetCardData();

    // 准备叫地主
    void startLordCard(); // 抢地主入口

    // 成为地主
    void becomeLord(Player* player, int bet);

    // 清空玩家得分
    void clearPlayerScore();

    // 获取玩家下注的最大点数
    int getPlayerMaxBetPoint();

    int betScore() const;

private slots:
    // 处理叫地主
    void onGrabBet(Player* player, int point);

    // 玩家出牌
    void onNotifyPlayHand(Player* player, const Cards& cards);
signals:
    // 玩家状态变化
    void playerStatusChanged(Player* player, PlayerStatus status);

    // 通知主界面谁叫地主，下注多少分
    void notifyGrabLordBet(Player* player, int point, bool firstCallLord);

    // 游戏状态切换
    void gameStatusChanged(GameStatus status);

    // 通知主界面玩家出牌
    void notifyPlayHand(Player* player, const Cards& cards);

    //  传递玩家出牌信息
    void pendingInfo(Player* player, const Cards& cards);

    // 通知主界面更新赌注
    void notifyUpdateBeatPoint(int betScore);

private:
    Robot* m_robotLeft;
    Robot* m_robotRight;
    UserPlayer* m_user;
    Player* m_curPlayer;  // 当前的玩家
    Cards m_pendCards;    // 打出的牌
    Player* m_pendPlayer; // 出牌的玩家
    Cards m_allCards;
    BetReord m_betRecord; // 抢地主数据
    int m_betScore;       // 地主下注分数
};

#endif // GAMECONTROL_H

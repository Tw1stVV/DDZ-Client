#ifndef PLAYER_H
#define PLAYER_H

#include "card.h"
#include "cards.h"

#include <QObject>

class Player : public QObject
{
    Q_OBJECT
public:
    explicit Player(QObject* parent = nullptr);
    explicit Player(QString name, QObject* parent = nullptr);

    enum Role // 玩家角色
    {
        Lord,
        Farmer
    };

    enum Type // 玩家类型
    {
        Robot,
        User,
        UnKnow
    };

    enum Sex // 玩家性别
    {
        Man,
        Woman
    };

    enum Direction // 玩家头像方向
    {
        Left,
        Right
    };

    // 名字
    QString name() const;
    void setName(const QString& newName);

    // 玩家角色
    Role role() const;
    void setRole(Role newRole);

    // 玩家类型
    Type type() const;
    void setType(Type newType);

    // 性别
    Sex sex() const;
    void setSex(Sex newSex);

    // 玩家头像方向
    Direction direction() const;
    void setDirection(Direction newDirection);

    // 游戏结果
    int score() const;
    void setScore(int newScore);

    // 是否获胜
    bool getIsWin() const;
    void setIsWin(bool newIsWin);

    // 上家/下家对象
    Player* previous() const;
    void setPrevious(Player* newPrevious);
    Player* next() const;
    void setNext(Player* newNext);

    // 抢地主/叫地主
    void grabLordBet(int point);

    // 存储发牌时得到的扑克牌
    void storeDispatchCard(Card& card);   // 存储一张牌
    void storeDispatchCard(Cards& cards); // 存储多张牌，如发牌时的三张地主牌

    // 得到玩家所有的牌
    Cards getCards();

    // 清空玩家的牌
    void clearCards();

    // 玩家打出某张牌
    void playHand(const Cards& card);

    // 设置出牌的玩家待处理的扑克牌
    void setPendingInfo(Player* player, Cards& cards);

    // 获取出牌的玩家
    Player* getPendPlayer();

    // 获取打出的扑克牌
    Cards getPendCards();

    // 本轮游戏结束后重置出牌玩家和出牌信息
    void clearPendingInfo();

    virtual void prepareCallLord();
    virtual void preparePlayHand();
    virtual void thinkingCallLord();
    virtual void thinkingPlayHand();

public slots:
    // 存储出牌玩家对象和打出的牌
    void storePendingInfo(Player* player, const Cards& cards);

signals:
    // 通知GameControl开始叫地主下注
    void notifyGrabLordBet(Player* player, int bet);

    // 通知GameControl玩家出牌
    void notifyPlayHand(Player* player, const Cards& cards);

    // 通知主界面玩家收到扑克牌
    void notifyPickCards(Player* player, const Cards& cards);

protected:
    QString m_name;
    Role m_role;
    Type m_type;
    Sex m_sex;
    Direction m_direction;
    int m_score;
    bool isWin;
    Player* m_previous;
    Player* m_next;
    Cards m_cards;
    Cards m_pendCards;
    Player* m_pendPlayer;
};

#endif // PLAYER_H

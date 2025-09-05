#ifndef ROBOT_H
#define ROBOT_H

#include <QObject>
#include "player.h"

class Robot : public Player
{
    Q_OBJECT
public:
    explicit Robot(QObject* parent = nullptr);

    // 使用父类构造函数
    using Player::Player;

    // Player interface
public:
    virtual void prepareCallLord() override;
    virtual void preparePlayHand() override;

    virtual void thinkingCallLord() override;
    virtual void thinkingPlayHand() override;
};

#endif // ROBOT_H

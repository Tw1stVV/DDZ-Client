#include "robotplayhand.h"
#include "robot.h"

RobotPlayHand::RobotPlayHand(Player* player, QObject* parent) :
    QObject(parent), QRunnable(), m_player(player)
{
    setAutoDelete(true);
}

void RobotPlayHand::run()
{
    QThread::msleep(1500);
    m_player->thinkingPlayHand();
}

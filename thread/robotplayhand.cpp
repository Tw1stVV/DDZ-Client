#include "robotplayhand.h"
#include "robot.h"
RobotPlayHand::RobotPlayHand(Player* player, QObject* parent) : QThread(parent), m_player(player)
{
}

void RobotPlayHand::run()
{
    msleep(1500);
    m_player->thinkingPlayHand();
}

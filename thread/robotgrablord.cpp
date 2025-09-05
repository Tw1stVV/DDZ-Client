#include "robotgrablord.h"
#include "robot.h"
RobotGrabLord::RobotGrabLord(Robot* robot, QObject* parent) : QThread(parent), m_robot(robot)
{
}

void RobotGrabLord::run()
{
    msleep(2000);
    m_robot->thinkingCallLord();
}

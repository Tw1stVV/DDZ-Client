#include "robotgrablord.h"
#include "robot.h"
#include <QThread>
RobotGrabLord::RobotGrabLord(Robot* robot, QObject* parent) :
    QObject(parent), QRunnable(), m_robot(robot)
{
    setAutoDelete(true);
}

void RobotGrabLord::run()
{
    QThread::msleep(2000);
    m_robot->thinkingCallLord();
}

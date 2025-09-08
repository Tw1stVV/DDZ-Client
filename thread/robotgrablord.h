#ifndef ROBOTGRABLORD_H
#define ROBOTGRABLORD_H
#include <QRunnable>
#include <QThread>
class Robot;

class RobotGrabLord : public QObject, public QRunnable
{
    Q_OBJECT
public:
    RobotGrabLord(Robot* robot, QObject* parent = nullptr);

private:
    Robot* m_robot;

    // QThread interface
protected:
    virtual void run() override;
};

#endif // ROBOTGRABLORD_H

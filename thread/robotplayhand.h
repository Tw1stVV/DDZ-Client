#ifndef ROBOTPLAYHAND_H
#define ROBOTPLAYHAND_H
#include <QRunnable>
#include <QThread>

class Player;
class RobotPlayHand : public QObject, public QRunnable
{
    Q_OBJECT
public:
    explicit RobotPlayHand(Player* player, QObject* parent = nullptr);
    Player* m_player;
    RobotPlayHand(QObject* parent);

    // QThread interface
protected:
    virtual void run() override;
};

#endif // ROBOTPLAYHAND_H

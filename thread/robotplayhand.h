#ifndef ROBOTPLAYHAND_H
#define ROBOTPLAYHAND_H

#include <QThread>

class Player;
class RobotPlayHand : public QThread
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

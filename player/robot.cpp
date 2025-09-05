#include "robot.h"
#include "qdebug.h"
#include "strategy.h"
#include "robotgrablord.h"
#include "robotplayhand.h"
Robot::Robot(QObject* parent) : Player{parent}
{
    this->m_type = Player::Robot;
}

void Robot::prepareCallLord()
{
    RobotGrabLord* subThread = new RobotGrabLord(this);
    connect(
        subThread,
        &QThread::finished,
        this,
        [=]()
        {
            qDebug() << "RobotGrabLord子线程对象析构...." << ", Robot name: " << this->name();
            subThread->deleteLater();
        });
    subThread->start();
}

void Robot::preparePlayHand()
{
    RobotPlayHand* subThread = new RobotPlayHand(this);
    connect(
        subThread,
        &QThread::finished,
        this,
        [=]()
        {
            qDebug() << "RobotPlayHand子线程对象析构...." << ", Robot name: " << this->name();
            subThread->deleteLater();
        });
    subThread->start();
}

void Robot::thinkingCallLord()
{
    /*
     *基于手中的牌计算全重
     *大王/小王: 6
     *顺子/炸弹: 5
     *三张: 4
     *单张2: 3
     *对牌: 1
     */

    int weight = 0;
    Strategy st(this, m_cards);
    // 大小王权重
    weight += st.getRangeCards(Card::Card_SJ, Card::Card_BJ).cardCount() * 6;

    // 顺子权重
    //  pickOptimalSeqSingles()已经将炸弹从手牌里剔除
    QList<Cards> optimalSeq = st.pickOptimalSeqSingles();
    weight += optimalSeq.size() * 5;

    // 炸弹权重
    QList<Cards> bombs = st.findCardsByCount(4);
    weight += bombs.size() + 5;

    // 2的权重
    //  每张2都是两分，这里先计算2的权重，防止后面的三张将可能的三张2计算为4分
    weight += m_cards.pointCount(Card::Card_2) * 3;

    // 三张的权重
    Cards temp = m_cards;
    // 剔除顺子和炸弹和2(防止三张2)
    Cards card2 = st.getRangeCards(Card::Card_2, Card::Card_2);
    temp.remove(optimalSeq);
    temp.remove(bombs);
    temp.remove(card2);
    QList<Cards> triples = Strategy(this, temp).findCardsByCount(3);
    weight += triples.size() * 4;

    // 对牌的权重
    temp.remove(triples);
    QList<Cards> pair = Strategy(this, temp).findCardsByCount(2);
    weight += pair.size();

    if (weight >= 22)
    {
        grabLordBet(3);
    }
    else if (weight < 22 && weight >= 18)
    {
        grabLordBet(2);
    }
    else if (weight < 18 && weight >= 10)
    {
        grabLordBet(1);
    }
    else
    {
        grabLordBet(0);
    }
}

void Robot::thinkingPlayHand()
{
    Strategy st(this, m_cards);
    Cards cs = st.makeStrategy();
    this->playHand(cs);
}

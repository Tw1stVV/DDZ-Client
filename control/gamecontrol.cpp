#include "gamecontrol.h"
#include "playhand.h"
#include "userplayer.h"
#include "robot.h"
#include "player.h"
#include <qDebug>
#include <QRandomGenerator>
#include <QTimer>

GameControl::GameControl(QObject* parent) : QObject{parent}
{
    m_betScore = 0;
}

void GameControl::playerInit()
{
    m_user = new UserPlayer("自己", this);
    m_robotLeft = new Robot("机器人A", this);
    m_robotRight = new Robot("机器人B", this);

    m_pendPlayer = nullptr;

    // 头像显示方位
    m_robotLeft->setDirection(Player::Left);
    m_robotRight->setDirection(Player::Right);
    m_user->setDirection(Player::Right);

    // 性别
    Player::Sex sex;
    sex = (Player::Sex)QRandomGenerator::global()->bounded(2);
    m_robotLeft->setSex(sex);
    sex = (Player::Sex)QRandomGenerator::global()->bounded(2);
    m_robotRight->setSex(sex);
    sex = (Player::Sex)QRandomGenerator::global()->bounded(2);
    m_user->setSex(sex);

    // 出牌顺序 逆时针
    //    机器人A        机器人B
    //            自己

    m_robotLeft->setPrevious(m_robotRight);
    m_robotLeft->setNext(m_user);

    m_robotRight->setPrevious(m_user);
    m_robotRight->setNext(m_robotLeft);

    m_user->setPrevious(m_robotLeft);
    m_user->setNext(m_robotRight);

    // 指定当前玩家为user,游戏初始化时玩家可以先抢地主
    m_curPlayer = m_user;

    // 处理玩家发送的下注信号
    connect(m_user, &Player::notifyGrabLordBet, this, &GameControl::onGrabBet);
    connect(m_robotLeft, &Player::notifyGrabLordBet, this, &GameControl::onGrabBet);
    connect(m_robotRight, &Player::notifyGrabLordBet, this, &GameControl::onGrabBet);

    // 传递出牌玩家和打出的牌
    connect(this, &GameControl::pendingInfo, m_robotLeft, &Player::storePendingInfo);
    connect(this, &GameControl::pendingInfo, m_robotRight, &Player::storePendingInfo);
    connect(this, &GameControl::pendingInfo, m_user, &Player::storePendingInfo);

    // 处理玩家出牌
    connect(m_robotLeft, &Robot::notifyPlayHand, this, &GameControl::onNotifyPlayHand);
    connect(m_robotRight, &Robot::notifyPlayHand, this, &GameControl::onNotifyPlayHand);
    connect(m_user, &Robot::notifyPlayHand, this, &GameControl::onNotifyPlayHand);
}

Robot* GameControl::robotLeft() const
{
    return m_robotLeft;
}

Robot* GameControl::robotRight() const
{
    return m_robotRight;
}

UserPlayer* GameControl::user() const
{
    return m_user;
}

Player* GameControl::curPlayer() const
{
    return m_curPlayer;
}

void GameControl::setCurPlayer(Player* newCurPlayer)
{
    m_curPlayer = newCurPlayer;
}

Cards GameControl::pendCards() const
{
    return m_pendCards;
}

Player* GameControl::pendPlayer() const
{
    return m_pendPlayer;
}

void GameControl::initCards()
{
    m_allCards.clear();
    for (int p = Card::Card_Begin + 1; p < Card::Card_SJ; ++p)
    {
        for (int s = Card::Suit_Begin + 1; s < Card::Suit_End; ++s)
        {
            Card c((Card::CardPoint)p, (Card::CardSuit)s);
            m_allCards.add(c);
        }
    }
    m_allCards.add(Card(Card::Card_SJ, Card::Suit_Begin));
    m_allCards.add(Card(Card::Card_BJ, Card::Suit_Begin));
}

Card GameControl::takeOneCard()
{
    return m_allCards.takeRandCard();
}

Cards GameControl::getSurplusCards()
{
    return m_allCards;
}

void GameControl::resetCardData()
{
    // 洗牌
    initCards();
    // 清空玩家手中的牌
    m_user->clearCards();
    m_robotLeft->clearCards();
    m_robotRight->clearCards();
    // 初始化出牌玩家和牌
    m_pendPlayer = nullptr;
    m_pendCards.clear();
}

void GameControl::startLordCard()
{
    m_curPlayer->prepareCallLord();
    // 告知主界面，当前玩家叫地主
    emit playerStatusChanged(m_curPlayer, ThinkingForCallLord);
}

void GameControl::becomeLord(Player* player, int bet)
{
    // 设置下注分数
    m_betScore = bet;

    // 设置身份
    player->setRole(Player::Lord);
    player->previous()->setRole(Player::Farmer);
    player->next()->setRole(Player::Farmer);

    // 获取地主牌
    m_curPlayer = player;
    player->storeDispatchCard(m_allCards);

    // 切换游戏状态和玩家状态，并准备出牌
    QTimer::singleShot(
        1000,
        this,
        [=]()
        {
            emit playerStatusChanged(player, ThinkingForPlayHand);
            emit gameStatusChanged(PlayingHand);
            m_curPlayer->preparePlayHand();
        });
}

void GameControl::clearPlayerScore()
{
    m_user->setScore(0);
    m_robotLeft->setScore(0);
    m_robotRight->setScore(0);
}

int GameControl::getPlayerMaxBetPoint()
{
    return m_betRecord.betPoint;
}

void GameControl::onGrabBet(Player* player, int point)
{
    // 1. 通知主界面玩家叫地主了（更新信息提示）
    // 下注为0表示不抢地主，或下注小于上个玩家下的注表示放弃抢地主
    if (point == 0 || point <= m_betRecord.betPoint)
    {
        emit notifyGrabLordBet(player, 0, false);
    }
    // 下注大于0并且，m_betRecord.betPoint等于0表示这是第一个抢地主的玩家
    else if (point > 0 && m_betRecord.betPoint == 0)
    {
        emit notifyGrabLordBet(player, point, true);
    }
    else
    // 第二、第三个抢地主的玩家
    {
        emit notifyGrabLordBet(player, point, false);
    }

    // 更新主界面赌注点数
    emit notifyUpdateBeatPoint(point);

    // 2. 判断玩家下注是不是三分，如果是，抢地主结束
    if (point == 3)
    {
        this->becomeLord(player, point);
        // 清空数据
        m_betRecord.reset();
        // 结束抢地主
        return;
    }

    // 3. 下注不够三分，对所有玩家下的注进行比较，分数高的是地主
    // 记录本次下注高的玩家和分数
    if (m_betRecord.betPoint < point)
    {
        m_betRecord.betPoint = point;
        m_betRecord.player = player;
    }
    m_betRecord.grabbingLordTimers++;
    // 如果每个玩家都抢过一次地主，分数高的成为地主，结束抢地主
    if (m_betRecord.grabbingLordTimers == 3)
    {
        // 没有玩家抢地主，重新发牌
        if (m_betRecord.grabbingLordTimers == 0)
            emit gameStatusChanged(DispatchCord);
        else
            // 运行到这说明每个玩家都参与了抢地主，m_betRecord.player只保留下注最高的玩家
            this->becomeLord(m_betRecord.player, point);
        // 重置数据，防止下一轮游戏抢地主时有数据残留
        m_betRecord.reset();
        return;
    }

    // 4. 切换玩家，通知下一个玩家继续抢地主
    m_curPlayer = player->next();
    // 告知主界面，轮到下一个玩家抢地主(只做主界面更新操作)
    emit playerStatusChanged(m_curPlayer, ThinkingForCallLord);
    // 切换下个玩家抢地主
    m_curPlayer->prepareCallLord();
}

void GameControl::onNotifyPlayHand(Player* player, const Cards& cards)
{
    // 1. 将玩家出牌信号发送给主界面
    emit notifyPlayHand(player, cards);
    // 2.如果不是空牌，保存出牌玩家和打出的牌，并给其他玩家发送信号
    if (!cards.isEmpty())
    {
        m_pendCards = cards;
        m_pendPlayer = player;
        emit pendingInfo(player, cards);
    }
    // 3.出牌是炸弹或者王炸将下注分数*2
    PlayHand::HandType type = PlayHand(cards).type();
    if (type == PlayHand::Hand_Bomb || type == PlayHand::Hand_Bomb_Jokers)
    {
        m_betScore *= 2;
        emit notifyUpdateBeatPoint(m_betScore);
    }

    // 3.玩家出完牌，计算本局游戏总分
    if (player->getCards().isEmpty())
    {
        // 地主获胜地主玩家加上游戏总分*2，农民获胜农民加上游戏总分*1
        Player* previous = player->previous();
        Player* next = player->next();
        if (player->role() == Player::Lord)
        {
            // 出牌玩家是地主
            player->setScore(player->score() + m_betScore * 2);
            previous->setScore(previous->score() - m_betScore);
            next->setScore(next->score() - m_betScore);
            player->setIsWin(true);
            previous->setIsWin(false);
            next->setIsWin(false);
        }
        else
        {
            // 出牌玩家是农民
            player->setScore(player->score() + m_betScore);
            player->setIsWin(true);
            if (previous->role() == Player::Lord)
            {
                // 上家是地主
                next->setScore(next->score() + m_betScore);
                previous->setScore(previous->score() - m_betScore * 2);
                next->setIsWin(true);
                previous->setIsWin(false);
            }
            else
            {
                // 下家是地主
                next->setScore(next->score() - m_betScore * 2);
                previous->setScore(previous->score() + m_betScore);
                next->setIsWin(false);
                previous->setIsWin(true);
            }
        }
        // 本轮游戏结束重置玩家存储的上轮出牌玩家和出牌信息
        next->clearPendingInfo();
        player->clearPendingInfo();
        previous->clearPendingInfo();

        // 通知主界面玩家获胜
        emit playerStatusChanged(player, GameControl::Winning);
        // 重置赌注
        m_betScore = 0;
        emit notifyUpdateBeatPoint(m_betScore);
        return;
    }
    // 4.玩家没有出完牌，切换下个玩家出牌
    m_curPlayer = player->next();
    m_curPlayer->preparePlayHand();
    emit playerStatusChanged(m_curPlayer, ThinkingForPlayHand);
}

int GameControl::betScore() const
{
    return m_betScore;
}

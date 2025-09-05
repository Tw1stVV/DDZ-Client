#include "player.h"

Player::Player(QObject* parent) : QObject{parent}
{
    m_score = 0;
    isWin = false;
    m_pendPlayer = nullptr;
    m_role = Lord;
}

Player::Player(QString name, QObject* parent) : Player(parent)
{
    m_name = name;
}

QString Player::name() const
{
    return m_name;
}

void Player::setName(const QString& newName)
{
    m_name = newName;
}

Player::Role Player::role() const
{
    return m_role;
}

void Player::setRole(Role newRole)
{
    m_role = newRole;
}

Player::Type Player::type() const
{
    return m_type;
}

void Player::setType(Type newType)
{
    m_type = newType;
}

Player::Sex Player::sex() const
{
    return m_sex;
}

void Player::setSex(Sex newSex)
{
    m_sex = newSex;
}

Player::Direction Player::direction() const
{
    return m_direction;
}

void Player::setDirection(Direction newDirection)
{
    m_direction = newDirection;
}

int Player::score() const
{
    return m_score;
}

void Player::setScore(int newScore)
{
    m_score = newScore;
}

bool Player::getIsWin() const
{
    return isWin;
}

void Player::setIsWin(bool newIsWin)
{
    isWin = newIsWin;
}

Player* Player::previous() const
{
    return m_previous;
}

void Player::setPrevious(Player* newPrevious)
{
    m_previous = newPrevious;
}

Player* Player::next() const
{
    return m_next;
}

void Player::setNext(Player* newNext)
{
    m_next = newNext;
}

void Player::grabLordBet(int point)
{
    emit notifyGrabLordBet(this, point);
}

void Player::storeDispatchCard(Card& card)
{
    m_cards.add(card);
    Cards cs(card);
    emit notifyPickCards(this, cs);
}

void Player::storeDispatchCard(Cards& cards)
{
    m_cards.add(cards);
    emit notifyPickCards(this, cards);
}

Cards Player::getCards()
{
    return m_cards;
}

void Player::clearCards()
{
    m_cards.clear();
}

void Player::playHand(const Cards& cards)
{
    // 删除玩家打出的这张牌
    m_cards.remove(cards);

    // 发出信号
    emit notifyPlayHand(this, cards);
}

void Player::setPendingInfo(Player* player, Cards& cards)
{
    m_pendCards = cards;
    m_pendPlayer = player;
}

Player* Player::getPendPlayer()
{
    return m_pendPlayer;
}

Cards Player::getPendCards()
{
    return m_pendCards;
}

void Player::clearPendingInfo()
{
    m_pendPlayer = nullptr;
    m_pendCards = Cards();
}

void Player::prepareCallLord()
{
    // 基类不做实现，留给子类处理
}

void Player::preparePlayHand()
{
    // 基类不做实现，留给子类处理
}

void Player::thinkingCallLord()
{
    // 基类不做实现，留给子类处理
}

void Player::thinkingPlayHand()
{
    // 基类不做实现，留给子类处理
}

void Player::storePendingInfo(Player* player, const Cards& cards)
{
    m_pendPlayer = player;
    m_pendCards = cards;
}

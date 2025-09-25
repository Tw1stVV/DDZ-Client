#ifndef GAMEPANEL_H
#define GAMEPANEL_H

#define TEST

#include "animationwindow.h"
#include "card.h"
#include "cards.h"
#include "countdown.h"
#include "gamecontrol.h"
#include "player.h"
#include <QMainWindow>

QT_BEGIN_NAMESPACE
class CardPanel;
// class Player;
class QLabel;
class QTimer;
class BGMControl;
namespace Ui {
class Gamepanel;
}
QT_END_NAMESPACE

class Gamepanel : public QMainWindow
{
    Q_OBJECT

public:
    Gamepanel(QWidget* parent = nullptr);
    ~Gamepanel();

private:
    // 初始化游戏控制类
    void gameControlInit();

    // 更新分数面板分数
    void updatePlayerScore();

    // 初始化图片
    void initCardMap();
    // 将对应位置的牌切割并保存到卡牌窗口并添加到m_cardMap
    void cropImage(QPixmap& pix, int x, int y, Card& c);

    // 初始化按钮组
    void initButtonGroup();

    // 初始化玩家在窗口中的上下文环境
    void initPlayerContext();

    // 初始化游戏场景
    void initGameScene();

    // 处理游戏状态
    void gameStartPrecess(GameControl::GameStatus status);

    // 发牌前的设置
    void startDispatchCard();

    // 发牌定时器处理
    void onDispatchCard();

    // 移动扑克牌
    void cardMoveStep(Player* curPlayer, int curPos, int floatPos = 0);

    // 处理分发得到的扑克牌
    void disposeCard(Player* player, const Cards& cards);

    // 更新扑克牌在窗口中的窗口
    void updatePlayerCards(Player* player);

    // 显示动画
    void showAnimation(AnimationWindow::AnimationType type, int bet = 0);

    // 隐藏玩家上一轮打出的牌或提示信息
    void hidePlayerDropCardsOrInfo(Player* player);

    // 加载玩家头像
    QPixmap loadRoleImage(Player::Sex sex, Player::Role roe, Player::Direction dirc);

    // 显示游戏结算窗口
    void showEndingScorePanel();

    // 初始化倒计时窗口
    void initCountDown();

    // 修改玩家头像
    void changePlayerImg(Player* player, Player::Role);

private slots:
    // 处理玩家状态变化
    void onPlayerStatusChanged(Player* player, GameControl::PlayerStatus status);

    // 处理玩家抢地主
    void onNotifyGrabLordBet(Player* player, int point, bool firstCallLord);

    // 处理玩家出牌
    void onNotifyPlayHand(Player* player, const Cards& cards);

    // 玩家选中扑克牌
    void onCardSelected(Qt::MouseButton button);

    // 处理用户玩家出牌
    void onUserPlayHand();

    // 处理用户玩家不出牌
    void onUserPass();

    // 更新赌注
    void updateBeat(int beat);

public:
signals:
    void ClosePanel();

private:
    enum CardAlign { Horizontal, Vertical };
    struct PlayContext
    {
        // 1. 玩家扑克牌显示的区域
        QRect cardRect;

        // 2. 出牌的区域
        QRect playHandRect;

        // 3. 扑克牌对齐方式
        CardAlign align;

        // 4. 扑克牌显示正面or背面
        bool isFrontSide;

        // 5. 游戏过程中的提升信息，如：不要
        QLabel* info;

        // 6. 玩家头像
        QLabel* roleImg;

        // 7. 玩家刚打出的牌
        Cards lastCards;
    };

private:
    Ui::Gamepanel* ui;
    QPixmap m_backgroundImage;
    GameControl* m_gameCtl;
    QList<Player*> m_playerList;             // 保存玩家类
    QMap<Card, CardPanel*> m_cardMap;        // 保存卡牌和对应的卡牌窗口
    QSize m_cardSize;                        // 每张扑克牌的大小 w:80 h:105
    QPixmap m_cardBackgroundImage;           // 扑克牌背面图片
    QMap<Player*, PlayContext> m_contextMap; // 存储玩家上下文信息
    CardPanel* m_baseCards;                  // 发牌区的扑克牌
    CardPanel* m_moveCard;                   // 发牌过程中移动的扑克牌
    QList<CardPanel*> m_lastThreeCard;       // 最后三张地主牌的窗口
    QPoint baseCardPos;                      // 发牌区的扑克牌的位置
    GameControl::GameStatus m_gameStatus;    // 游戏状态
    QTimer* m_timer;                         // 定时器
    AnimationWindow* m_animation;
    CardPanel* m_curSelectedCardPanel;    // 当前选中的扑克牌
    QSet<CardPanel*> m_selectedCards;     // 选中的扑克牌集合
    QRect m_cardsRect;                    // 非机器人玩家剩余的扑克牌显示的区域
    QHash<CardPanel*, QRect> m_userCards; // 存储非机器人玩家手中的扑克牌窗口在m_cardsRect中的位置
    CountDown* m_countDown;
    BGMControl* m_bgm;

    // QWidget interface
protected:
    virtual void paintEvent(QPaintEvent* event) override;

    // QWidget interface
protected:
    virtual void mouseMoveEvent(QMouseEvent* event) override;

    // QWidget interface
protected:
    virtual void closeEvent(QCloseEvent *event) override;
};
#endif // GAMEPANEL_H

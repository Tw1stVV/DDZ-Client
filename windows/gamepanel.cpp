#include "cardpanel.h"
#include "gamecontrol.h"
#include "gamepanel.h"
#include "qforeach.h"
#include "robot.h"
#include "ui_gamepanel.h"
#include "userplayer.h"
#include <QLabel>
#include <QPaintEvent>
#include <QPainter>
#include <QDebug>
#include <QRandomGenerator>
#include <QPainter>
#include <QPropertyAnimation>
#include <QTimer>
#include "animationwindow.h"
#include "endingpanel.h"
#include "playhand.h"
#include "bgmcontrol.h"

Gamepanel::Gamepanel(QWidget* parent) : QMainWindow(parent), ui(new Ui::Gamepanel)
{
    ui->setupUi(this);
    this->setAutoFillBackground(true);

    // 1. 背景图
    int num = QRandomGenerator::global()->bounded(1, 11); //[1,10)
    QString path = QString(":/images/background-%1.png").arg(num);
    m_backgroundImage.load(path);

    // 2. 设置窗口标题，大小，图标
    this->setFixedSize(1200, 750); // 不允许改变窗口大小
    this->setWindowTitle("欢乐斗地主");
    this->setWindowIcon(QIcon(":/images/logo.ico"));

    // 3. 实例化游戏控制类对象GameControl
    gameControlInit();

    // 4. 更新玩家得分
    updatePlayerScore();

    // 初始化赌注
    this->updateBeat(0);

    // 5. 切割扑克牌图片
    initCardMap();

    // 6. 初始化游戏中的按钮组
    initButtonGroup();

    // 7. 初始化玩家在窗口中的上下文化境
    initPlayerContext();

    // 8. 初始化游戏场景
    initGameScene();

    // 初始化定时器
    initCountDown();

    // 定时器实例化
    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &Gamepanel::onDispatchCard);

    m_animation = new AnimationWindow(this);

    m_bgm = new BGMControl(this);
}

Gamepanel::~Gamepanel()
{
    delete ui;
    deleteLater();
}

void Gamepanel::gameControlInit()
{
    m_gameCtl = new GameControl(this);
    // 初始化玩家对象
    m_gameCtl->playerInit();
    Robot* left = m_gameCtl->robotLeft();
    Robot* right = m_gameCtl->robotRight();
    UserPlayer* user = m_gameCtl->user();
    // 存储顺序 leftRobot rightRobot User
    m_playerList << left << right << user;

    connect(m_gameCtl, &GameControl::playerStatusChanged, this, &Gamepanel::onPlayerStatusChanged);
    connect(m_gameCtl, &GameControl::notifyGrabLordBet, this, &Gamepanel::onNotifyGrabLordBet);
    connect(m_gameCtl, &GameControl::gameStatusChanged, this, &Gamepanel::gameStartPrecess);
    connect(m_gameCtl, &GameControl::notifyPlayHand, this, &Gamepanel::onNotifyPlayHand);

    connect(left, &Player::notifyPickCards, this, &Gamepanel::disposeCard);
    connect(right, &Player::notifyPickCards, this, &Gamepanel::disposeCard);
    connect(user, &Player::notifyPickCards, this, &Gamepanel::disposeCard);

    connect(m_gameCtl, &GameControl::notifyUpdateBeatPoint, this, &Gamepanel::updateBeat);
}

void Gamepanel::updatePlayerScore()
{
    ui->scorePanel->setScore(
        m_playerList[0]->score(), m_playerList[1]->score(), m_playerList[2]->score());
}

void Gamepanel::initCardMap()
{
    // 1. 加载大图
    QPixmap pixmap(":/images/card.png"); // 图片按每张扑克牌来切割共有13列5行

    // 2. 计算扑克牌大小
    m_cardSize.setWidth(pixmap.width() / 13);
    m_cardSize.setHeight(pixmap.height() / 5);

    // 3. 扑克牌背面，正常花色牌，大小王
    /*
        ————————x
        |
        |
        |
        y
    */
    // 扑克牌背面
    m_cardBackgroundImage = pixmap.copy(
        m_cardSize.width() * 2, m_cardSize.height() * 4, m_cardSize.width(), m_cardSize.height());
    // 正常花色牌
    for (int y = 0, suit = Card::Suit_Begin + 1; suit < Card::Suit_End; ++y, ++suit)
    {
        for (int x = 0, pt = Card::Card_Begin + 1; pt < Card::Card_End; ++x, ++pt)
        {
            Card c((Card::CardPoint)pt, (Card::CardSuit)suit);
            // 裁剪图片
            cropImage(pixmap, m_cardSize.width() * x, m_cardSize.height() * y, c);
        }
    }

    // 大小王
    Card c(Card::Card_SJ, Card::Suit_Begin); // 小王
    cropImage(pixmap, 0 * m_cardSize.width(), m_cardSize.height() * 4, c);
    c.setPoint(Card::Card_BJ); // 大王
    cropImage(pixmap, 1 * m_cardSize.width(), m_cardSize.height() * 4, c);
}

void Gamepanel::cropImage(QPixmap& pix, int x, int y, Card& c)
{
    // 裁剪出对应位置的牌
    QPixmap sub = pix.copy(x, y, m_cardSize.width(), m_cardSize.height());

    // 创建CardPenal对象
    CardPanel* panel = new CardPanel(this);

    // 设置penal对象正面和背面图片
    panel->setImage(sub, m_cardBackgroundImage);

    // 将牌设置到penal对象中
    panel->setCard(c);
    panel->hide();
    m_cardMap.insert(c, panel);

    connect(panel, &CardPanel::CardSelected, this, &Gamepanel::onCardSelected);
}

void Gamepanel::initButtonGroup()
{
    ui->btnGroup->initButtons();
    ui->btnGroup->selectPanel(ButtonGroup::Start);
    connect(
        ui->btnGroup,
        &ButtonGroup::startGame,
        this,
        [=]()
        {
            // 界面初始化
            ui->btnGroup->selectPanel(ButtonGroup::Empty);
            // 修改游戏状态->发牌
            gameStartPrecess(GameControl::DispatchCord);

            // 播放背景音乐
            m_bgm->startBGM(80);
        });

    connect(ui->btnGroup, &ButtonGroup::playHand, this, &Gamepanel::onUserPlayHand);
    connect(ui->btnGroup, &ButtonGroup::pass, this, &Gamepanel::onUserPass);
    connect(
        ui->btnGroup,
        &ButtonGroup::betPoint,
        this,
        [=](int bet)
        {
            m_gameCtl->user()->grabLordBet(bet);
            ui->btnGroup->hide();
        });
}

void Gamepanel::initPlayerContext()
{
    // 1. 放置玩家扑克牌的区域
    const QRect cardsRect[] = {
        QRect(90, 130, 100, height() - 200),                    // 左侧机器人
        QRect(rect().right() - 190, 130, 100, height() - 200),  // 右侧机器人
        QRect(250, rect().bottom() - 120, width() - 500, 100)}; // 玩家

    // 2. 玩家出牌的区域
    const QRect playHandRect[] = {
        QRect(260, 150, 100, 100),                              // 左侧机器人
        QRect(rect().right() - 360, 150, 100, 100),             // 右侧机器人
        QRect(150, rect().bottom() - 290, width() - 300, 105)}; // 玩家

    // 3. 玩家头像位置
    const QPoint roleImgPos[] = {
        QPoint(cardsRect[0].left() - 80, cardsRect[0].height() / 2 + 20),  // 左侧机器人
        QPoint(cardsRect[1].right() + 10, cardsRect[1].height() / 2 + 20), // 右侧机器人
        QPoint(cardsRect[2].right() - 10, cardsRect[2].top() - 10)};       // 玩家

    // 4. 逐个遍历m_playerList将其添加到m_contextMap
    int index = m_playerList.indexOf(m_gameCtl->user()); // 记录非机器人玩家在list里的索引
    for (int i = 0; i < m_playerList.size(); ++i)
    {
        PlayContext context;
        context.cardRect = cardsRect[i];
        context.playHandRect = playHandRect[i];
        context.align = i == index ? Horizontal : Vertical;
#ifndef TEST
        context.isFrontSide = i == index ? true : false;
#else
        context.isFrontSide = true;
#endif

        // 提示信息
        context.info = new QLabel(this);
        context.info->resize(160, 98);
        context.info->hide();
        // 提示信息显示到出牌区域的中间
        QRect rect = playHandRect[i];
        float x = rect.left() + (rect.width() - context.info->width()) / 2;
        float y = rect.top() + (rect.height() - context.info->height()) / 2;
        QPoint pt(x, y);
        context.info->move(pt);

        // 玩家头像
        context.roleImg = new QLabel(this);
        context.roleImg->resize(89, 120);
        context.roleImg->move(roleImgPos[i]);
        context.roleImg->show();

        m_contextMap.insert(m_playerList[i], context);
    }
}

void Gamepanel::initGameScene()
{
    // 1. 发牌区的扑克牌
    m_baseCards = new CardPanel(this);
    m_baseCards->setImage(m_cardBackgroundImage, m_cardBackgroundImage);

    // 2. 发牌过程中移动的扑克牌
    m_moveCard = new CardPanel(this);
    m_moveCard->setImage(m_cardBackgroundImage, m_cardBackgroundImage);

    // 3. 最后三张底牌的显示
    for (int i = 0; i < 3; ++i)
    {
        CardPanel* panel = new CardPanel(this);
        panel->setImage(m_cardBackgroundImage, m_cardBackgroundImage);
        panel->hide();
        m_lastThreeCard.push_back(panel);
    }

    // 发牌区扑克牌位置
    float x = (this->width() - m_cardSize.width()) / 2; // 在窗口水平居中位置
    float y = (this->height()) / 2 - 100;               // 在窗口垂直居中向上100个像素
    baseCardPos = QPoint(x, y);
    m_baseCards->move(baseCardPos);
    m_moveCard->move(baseCardPos);

    // 地主牌位置
    // 每张牌y轴位置不变，x轴位置是base+(牌的宽度+每张牌的间隔)*第i张牌
    float base = (this->width() - 3 * m_cardSize.width() - 10 * 2) / 2; // 每张牌间隔10个像素
    for (int i = 0; i < m_lastThreeCard.size(); ++i)
    {
        m_lastThreeCard[i]->move(base + (m_cardSize.width() + 10) * i, 20);
    }
}

void Gamepanel::gameStartPrecess(GameControl::GameStatus status)
{
    m_gameStatus = status;
    switch (status)
    {
        // 发牌状态
        case GameControl::DispatchCord:
            startDispatchCard();
            break;
        // 叫地主状态
        case GameControl::CallingLord:
        {
            // 取出地主牌数据
            CardList list = m_gameCtl->getSurplusCards().toCardList();
            // 给地主牌窗口设置图片
            for (int i = 0; i < list.size(); ++i)
            {
                QPixmap p = m_cardMap[list.at(i)]->getImage();
                m_lastThreeCard.at(i)->setImage(p, m_cardBackgroundImage);
                m_lastThreeCard.at(i)->hide();
            }
            // 开始叫地主
            m_gameCtl->startLordCard();
            // 显示玩家头像，默认全部是农民
            for (int j = 0; j < m_playerList.size(); ++j)
            {
                // PlayContext& context = m_contextMap[m_playerList.at(j)];
                Player* player = m_playerList[j];
                changePlayerImg(player, Player::Farmer);
            }
            break;
        }
        case GameControl::PlayingHand:
        {
            // 隐藏发牌区的牌
            m_baseCards->hide();
            // 显示三张地主牌
            for (int i = 0; i < m_lastThreeCard.size(); ++i)
            {
                m_lastThreeCard.at(i)->show();
            }
            for (int j = 0; j < m_playerList.size(); ++j)
            {
                PlayContext& context = m_contextMap[m_playerList.at(j)];
                // 隐藏抢地主过程中的提示信息
                context.info->hide();
                // 修改地主头像
                if (m_playerList.at(j)->role() == Player::Lord)
                {
                    changePlayerImg(m_playerList.at(j), Player::Lord);
                }
            }
            break;
        }
        default:
            break;
    }
}

void Gamepanel::startDispatchCard()
{
    // 重置每张牌的属性
    foreach (auto& iter, m_cardMap)
    {
        iter->setSelected(false);
        iter->setFrontSide(true);
        iter->hide();
    }

    // 隐藏三张地主牌
    foreach (auto& iter, m_lastThreeCard)
    {
        iter->hide();
    }

    // 重置玩家的上下文窗口信息
    int index = m_playerList.indexOf(m_gameCtl->user());
    for (int i = 0; i < m_playerList.size(); ++i)
    {
        m_contextMap[m_playerList.at(i)].lastCards.clear();
        m_contextMap[m_playerList.at(i)].info->hide();
        // m_contextMap[m_playerList.at(i)].roleImg->hide();
        // 重置地主头像为农民
        if (m_playerList[i]->role() == Player::Lord)
        {
            changePlayerImg(m_playerList.at(i), Player::Farmer);
        }
        m_contextMap[m_playerList.at(i)].isFrontSide = i == index ? true : false;
    }
    // 重置所有玩家的扑克牌数据
    m_gameCtl->resetCardData();

    // 显示发牌的牌堆
    m_baseCards->show();

    // 隐藏按钮面板
    ui->btnGroup->selectPanel(ButtonGroup::Empty);

    // 启动定时器
    m_timer->start(20);

    // 播放背景音乐
    m_bgm->playAssistMusic(BGMControl::Dispatch);
}

void Gamepanel::onDispatchCard()
{
    // 记录移动中的扑克牌位置
    static int curMoveCard = 0;

    // 当前玩家
    Player* curPlayer = m_gameCtl->curPlayer();

    // 当扑克牌移动到玩家扑克牌区域时
    if (curMoveCard >= 100)
    {
        // 给当前的玩家发一张牌
        Card card = m_gameCtl->takeOneCard();
        curPlayer->storeDispatchCard(card);

        // 在玩家扑克牌显示区域更新扑克牌
        Cards cards(card);

        // 由storeDispatchCard()函数里发送的信号调用disposeCard()
        // disposeCard(curPlayer, cards);

        // 重置扑克牌位置，重置扑克牌移动的浮动位置，切换下一个玩家
        curMoveCard = 0;
        m_gameCtl->setCurPlayer(curPlayer->next());
        curPlayer = m_gameCtl->curPlayer();

        // 判断扑克牌是否发完
        if (m_gameCtl->getSurplusCards().cardCount() == 3)
        {
            // 停止定时器
            m_timer->stop();

            // 游戏状态切换 发牌->叫地主
            gameStartPrecess(GameControl::CallingLord);

            // 停止播放发牌音效
            m_bgm->stopAssistMusic();

            m_moveCard->hide();
            return;
        }
    }

    // 移动扑克牌
    cardMoveStep(curPlayer, curMoveCard);
    curMoveCard += 30;
}

// cardMoveStep只做发牌动画，不参与实际发牌操作
void Gamepanel::cardMoveStep(Player* curPlayer, int curPos, int floatPos)
{
    Q_UNUSED(floatPos);

    // 在函数多次调用中得到每个玩家扑克牌展示区域
    QRect curRect = m_contextMap[curPlayer].cardRect;

    // 每个玩家扑克牌展示区域和牌堆之间的单位步长
    int unit[] = {
        (baseCardPos.x() - curRect.left()) / 100, // 左侧玩家
        (curRect.left() - baseCardPos.x()) / 100, // 右侧玩家
        (curRect.top() - baseCardPos.y()) / 100,  // 底部玩家
    };

    // 每次扑克牌移动时每个玩家对应的扑克牌窗口实时坐标
    QPoint pos[] = {
        QPoint(baseCardPos.x() - (unit[0] * curPos), baseCardPos.y()),
        QPoint(baseCardPos.x() + (unit[1] * curPos), baseCardPos.y()),
        QPoint(baseCardPos.x(), baseCardPos.y() + (unit[2] * curPos)),
    };

    // 移动扑克牌窗口
    int index = m_playerList.indexOf(curPlayer);
    m_moveCard->move(pos[index]);

    // 临界状态转换
    if (curPos == 0)
        m_moveCard->show();
    else if (curPos >= 100)
        m_moveCard->hide();
}

void Gamepanel::disposeCard(Player* player, const Cards& cards)
{
    CardList cardList = cards.toCardList();

    // 设置每张扑克牌窗口的所有者
    for (auto& iter : cardList)
    {
        CardPanel* panel = m_cardMap[iter];
        panel->setOwner(player);
    }

    // 更新扑克牌窗口
    updatePlayerCards(player);
}

void Gamepanel::updatePlayerCards(Player* player)
{
    // 拿到玩家拥有的牌作为key从m_cardMap中取出对应的cardPanel
    Cards cards = player->getCards();
    CardList list = cards.toCardList();

    // 拿到玩家对应的扑克牌展示区域
    QRect rect = m_contextMap[player].cardRect;

    // 计算本次更新的扑克牌在窗口中显示的位置
    // 前size-1张牌显示的长度或宽度
    int space = 25;
    // 水平显示
    int HorX = rect.left() + (rect.width() - (list.size() - 1) * space - m_baseCards->width()) / 2;
    int HorY = rect.top() + (rect.height() - m_baseCards->height()) / 2;

    // 垂直显示
    int VerX = rect.left() + (rect.width() - m_baseCards->width()) / 2;
    int VerY = rect.top() + (rect.height() - (list.size() - 1) * space - m_baseCards->height()) / 2;

    // 玩家框选扑克牌功能
    m_cardsRect = QRect();
    // 清空玩家手中剩余扑克牌的位置
    m_userCards.clear();

    for (int i = 0; i < list.size(); ++i)
    {
        // 拿到扑克牌对应的窗口
        CardPanel* panel = m_cardMap[list.at(i)];
        panel->show();
        panel->raise();
        // panel->setFrontSide(m_contextMap[player].isFrontSide);
        panel->setFrontSide(true);

        // 水平或垂直显示
        if (m_contextMap[player].align == Horizontal)
        {
            // 扑克牌被选中的和比其他牌高一点
            int temp = HorY;
            if (panel->isSelecetd())
            {
                temp -= 5;
                panel->move(HorX + space * i, temp);
                temp -= 5;
            }
            panel->move(HorX + space * i, temp);

            // 玩家框选扑克牌功能
            // 非机器人玩家剩余的扑克牌显示的区域
            m_cardsRect =
                QRect(HorX, HorY, space * i + m_baseCards->width(), m_baseCards->height());
            // 除了最后一张牌，其他牌的宽度都只有space
            int curWidth = 0;
            if (i != list.size() - 1)
            {
                curWidth = space;
            }
            else
            {
                curWidth = m_baseCards->width();
            }
            // panel这张牌显示的区域
            QRect curCardRect = QRect(HorX + i * space, HorY, curWidth, m_baseCards->height());
            m_userCards.insert(panel, curCardRect);
        }
        else
        {
            panel->move(VerX, VerY + space * i);
        }
    }

    // 显示玩家打出的牌
    // 玩家当前出牌区域以及本轮打出的牌
    QRect playHandrect = m_contextMap[player].playHandRect;
    Cards playHandCards = m_contextMap[player].lastCards;

    if (!playHandCards.isEmpty())
    {
        int spacing = 30;
        CardList lastCardsList = playHandCards.sort();
        CardList::ConstIterator iter = lastCardsList.constBegin();
        for (int i = 0; iter != lastCardsList.constEnd(); ++iter, ++i)
        {
            CardPanel* panel = m_cardMap[*iter];
            panel->setFrontSide(true);
            panel->raise();
            if (m_contextMap[player].align == Horizontal)
            {
                int leftX = playHandrect.left()
                            + (playHandrect.width() - (lastCardsList.size() - 1) * spacing
                               - m_baseCards->width())
                                  / 2;
                int topY = playHandrect.top() + (playHandrect.height() - m_baseCards->height()) / 2;
                panel->move(leftX + i * spacing, topY);
            }
            else
            {
                int leftX = playHandrect.left() + (playHandrect.width() - m_baseCards->width()) / 2;
                int topY = playHandrect.top()
                           + (playHandrect.height() - (lastCardsList.size() - 1) * spacing
                              + m_baseCards->height())
                                 / 2;
                panel->move(leftX, topY + i * spacing);
                // qDebug() << "X:" << leftX << " Y:" << topY + i * spacing;
            }
            panel->show();
        }
    }
}

void Gamepanel::showAnimation(AnimationWindow::AnimationType type, int bet)
{
    switch (type)
    {
        case AnimationWindow::Pair_Seq:
        case AnimationWindow::Sequent:
            m_animation->setFixedSize(250, 150);
            m_animation->move((this->width() - m_animation->width()) / 2, 200);
            m_animation->showSuquenceOrPair((AnimationWindow::Type)type);
            break;
        case AnimationWindow::Plane:
            m_animation->setFixedSize(800, 75);
            m_animation->move((this->width() - m_animation->width()) / 2, 200);
            m_animation->showPlane();
            break;
        case AnimationWindow::Bomb:
            m_animation->setFixedSize(180, 200);
            m_animation->move(
                (this->width() - m_animation->width()) / 2,
                (this->height() - m_animation->height()) / 2 - 70);
            m_animation->showBomb();
            break;
        case AnimationWindow::Bomb_Jokers:
            m_animation->setFixedSize(250, 200);
            m_animation->move(
                (this->width() - m_animation->width()) / 2,
                (this->height() - m_animation->height()) / 2 - 70);
            m_animation->showJokersBomb();
            break;
        case AnimationWindow::Bet:
            m_animation->setFixedSize(160, 98);
            m_animation->move(
                (this->width() - m_animation->width()) / 2,
                (this->height() - m_animation->height()) / 2 - 180);
            m_animation->showBetScore(bet);
            break;
        default:
            break;
    }
    m_animation->show();
}

void Gamepanel::hidePlayerDropCardsOrInfo(Player* player)
{
    // 获取玩家在游戏窗口中的信息
    auto it = m_contextMap.find(player);
    if (it != m_contextMap.end())
    {
        // 玩家上一轮没有出牌，隐藏提示信息
        if (it->lastCards.isEmpty())
        {
            it->info->hide();
        }
        else
        {
            // 获取玩家上一轮出的牌，并将这些牌的窗口隐藏
            CardList list = it->lastCards.toCardList();
            for (auto last = list.begin(); last != list.end(); ++last)
            {
                m_cardMap[*last]->hide();
            }
        }
        it->lastCards.clear();
    }
}

QPixmap Gamepanel::loadRoleImage(Player::Sex sex, Player::Role role, Player::Direction dirc)
{
    QList<QString> lordMan;
    QList<QString> lordWoman;
    QList<QString> farmerMan;
    QList<QString> farmerWoman;
    lordMan << ":/images/lord_man_1.png" << ":/images/lord_man_2.png";
    lordWoman << ":/images/lord_woman_1.png" << ":/images/lord_woman_2.png";
    farmerMan << ":/images/farmer_man_1.png" << ":/images/farmer_man_2.png";
    farmerWoman << ":/images/farmer_woman_1.png" << ":/images/farmer_woman_2.png";

    QImage image;
    int random = QRandomGenerator::global()->bounded(2);
    if (sex == Player::Man && role == Player::Lord)
    {
        image.load(lordMan.at(random));
    }
    else if (sex == Player::Woman && role == Player::Lord)
    {
        image.load(lordWoman.at(random));
    }
    else if (sex == Player::Man && role == Player::Farmer)
    {
        image.load(farmerMan.at(random));
    }
    else if (sex == Player::Woman && role == Player::Farmer)
    {
        image.load(farmerWoman.at(random));
    }

    QPixmap pix;
    if (dirc == Player::Left)
    {
        pix = QPixmap::fromImage(image);
    }
    else if (dirc == Player::Right)
    {
        pix = QPixmap::fromImage(image.mirrored(true, false));
    }
    return pix;
}

void Gamepanel::showEndingScorePanel()
{
    bool isLord = m_gameCtl->user()->role() == Player::Lord ? true : false;
    bool isWin = m_gameCtl->user()->getIsWin();
    m_bgm->playEndingMusic(isWin);
    EndingPanel* panel = new EndingPanel(isLord, isWin, this);
    panel->show();
    // 移动到主窗口上方隐藏起来
    int x = (this->width() - panel->width()) / 2;
    int y = -panel->height();
    panel->move(x, y);

    panel->setPlayerScore(
        m_gameCtl->robotLeft()->score(),
        m_gameCtl->robotRight()->score(),
        m_gameCtl->user()->score(),
        m_gameCtl->betScore());

    QPropertyAnimation* animation = new QPropertyAnimation(panel, "geometry", this);
    // 设置动画持续时间
    animation->setDuration(1500);
    // 设置动画起始位置和终止位置
    animation->setStartValue(QRect(panel->x(), panel->y(), panel->width(), panel->height()));
    animation->setEndValue(
        QRect(x, ((this->height() - panel->height()) / 2), panel->width(), panel->height()));
    // 设置动画曲线
    animation->setEasingCurve(QEasingCurve(QEasingCurve::OutBounce));
    // 播放动画
    animation->start();

    // 处理窗口信号
    connect(
        panel,
        &EndingPanel::continueGame,
        this,
        [=]()
        {
            // 关闭窗口
            panel->close();
            // 删除对象
            panel->deleteLater();
            animation->deleteLater();
            // 隐藏按钮组
            ui->btnGroup->selectPanel(ButtonGroup::Empty);
            // 切换游戏状态到发牌状态
            gameStartPrecess(GameControl::DispatchCord);

            // 播放背景音乐
            m_bgm->startBGM(80);
        });
}

void Gamepanel::initCountDown()
{
    m_countDown = new CountDown(this);
    m_countDown->move(
        (this->width() - m_countDown->width()) / 2,
        (this->height() - m_countDown->height()) / 2 + 30);

    connect(
        m_countDown,
        &CountDown::notTimeMuch,
        this,
        [=]()
        {
            // 播放提示音乐
            m_bgm->playAssistMusic(BGMControl::Alert);
        });

    connect(m_countDown, &CountDown::timeout, this, &Gamepanel::onUserPass);

    connect(
        m_gameCtl->user(),
        &UserPlayer::startCountDown,
        this,
        [=]() { m_countDown->showCountDown(); });
}

void Gamepanel::changePlayerImg(Player* player, Player::Role role)
{
    QPixmap pix = loadRoleImage(player->sex(), role, player->direction());
    m_contextMap[player].roleImg->setPixmap(pix);
    m_contextMap[player].roleImg->show();
}

void Gamepanel::onPlayerStatusChanged(Player* player, GameControl::PlayerStatus status)
{
    switch (status)
    {
        case GameControl::ThinkingForCallLord:
        {
            // 只有玩家抢地主状态才显示按钮，机器人玩家不显示
            if (player == m_gameCtl->user())
                ui->btnGroup->selectPanel(ButtonGroup::CallLord, m_gameCtl->getPlayerMaxBetPoint());

            break;
        }
        case GameControl::ThinkingForPlayHand:
        {
            // 隐藏上一轮玩家打出的牌
            hidePlayerDropCardsOrInfo(player);
            if (player == m_gameCtl->user())
            {
                // 出牌玩家对象，若上一轮出牌玩家不是自己，则可以选择出牌或不出，若上一轮出牌玩家是自己，只能选择出牌
                if (player == m_gameCtl->pendPlayer() || m_gameCtl->pendPlayer() == nullptr)
                {
                    ui->btnGroup->selectPanel(ButtonGroup::PlayCard);
                }
                else
                {
                    ui->btnGroup->selectPanel(ButtonGroup::PassOrPlay);
                }
            }
            else
            {
                ui->btnGroup->selectPanel(ButtonGroup::Empty);
            }
            ui->btnGroup->show();
            break;
        }
        case GameControl::Winning:
        {
            // 停止播放背景音乐
            m_bgm->stopBGM();

            // 本轮游戏结束，显示所有玩家的牌
            m_contextMap[m_gameCtl->robotLeft()].isFrontSide = true;
            m_contextMap[m_gameCtl->robotRight()].isFrontSide = true;
            updatePlayerCards(m_gameCtl->robotLeft());
            updatePlayerCards(m_gameCtl->robotRight());

            // 更新玩家得分
            updatePlayerScore();

            // 显示游戏结束结算窗口
            showEndingScorePanel();

            // 本局游戏获胜的玩家下一句可以优先抢地主
            m_gameCtl->setCurPlayer(player);

            break;
        }
        default:
            break;
    }
}

void Gamepanel::onNotifyGrabLordBet(Player* player, int point, bool firstCallLord)
{
    // 显示玩家抢地主信息
    PlayContext context = m_contextMap[player];
    if (point == 0)
    {
        context.info->setPixmap(QPixmap(":/images/buqinag.png"));
    }
    else
    { // 第一个抢地主的玩家显示“叫地主”，其余的显示“抢地主”
        if (firstCallLord)
        {
            context.info->setPixmap(QPixmap(":/images/jiaodizhu.png"));
        }
        else
        {
            context.info->setPixmap(QPixmap(":/images/qiangdizhu.png"));
        }
        context.info->show();
    }
    // 显示叫地主的得分图片
    showAnimation(AnimationWindow::Bet, point);
    // 播放分数的音乐
    m_bgm->playerRobLordMusic(point, player->sex(), firstCallLord);
}

void Gamepanel::onNotifyPlayHand(Player* player, const Cards& cards)
{
    // 1. 存储玩家打出的牌
    auto it = m_contextMap.find(player);
    it->lastCards = cards;

    // 2. 根据牌型显示特效
    PlayHand::HandType type = PlayHand(cards).type();
    if (type == PlayHand::Hand_Plane || type == PlayHand::Hand_Plane_Two_Single
        || type == PlayHand::Hand_Plane_Two_pair)
    {
        // 飞机特效
        showAnimation(AnimationWindow::Plane);
    }
    else if (type == PlayHand::Hand_Seq_Pair)
    {
        // 连对特效
        showAnimation(AnimationWindow::Pair_Seq);
    }
    else if (type == PlayHand::Hand_Seq_Single)
    {
        // 顺子特效
        showAnimation(AnimationWindow::Sequent);
    }
    else if (type == PlayHand::Hand_Bomb)
    {
        // 炸弹特效
        showAnimation(AnimationWindow::Bomb);
    }
    else if (type == PlayHand::Hand_Bomb_Jokers)
    {
        // 王炸特效
        showAnimation(AnimationWindow::Bomb_Jokers);
    }

    // 3. cards为空(玩家不出牌)显示提示信息
    if (cards.isEmpty())
    {
        it->info->setPixmap(QPixmap(":/images/pass.png"));
        it->info->show();
        // 播放不出牌对应的音效
        m_bgm->playPassMusic(player->sex());
    }
    else
    {
        // 本局游戏第一个出牌或本轮第一个出牌
        if (player == m_gameCtl->pendPlayer() || m_gameCtl->pendPlayer() == nullptr)
        {
            m_bgm->playCardsMusic(cards, true, player->sex());
        }
        else
        {
            m_bgm->playCardsMusic(cards, false, player->sex());
        }
    }

    // 4. 将打出的牌显示到界面更新玩家剩余的牌
    updatePlayerCards(player);

    // 5. 播放提示音效
    // 判断玩家剩余手牌数量
    if (player->getCards().cardCount() == 1)
    {
        m_bgm->playLastMusic(BGMControl::Last1, player->sex());
    }
    else if (player->getCards().cardCount() == 2)
    {
        m_bgm->playLastMusic(BGMControl::Last2, player->sex());
    }
}

void Gamepanel::onCardSelected(Qt::MouseButton button)
{
    // 1.判断是不是出牌状态
    if (m_gameStatus == GameControl::CallingLord || GameControl::DispatchCord)
    {
        return;
    }

    // 2.判断这种扑克牌的所有者是不是用户玩家
    CardPanel* panel = static_cast<CardPanel*>(sender());
    if (panel->getOwner() != m_gameCtl->user())
    {
        // 用户只能选自己的牌
        return;
    }

    // 3.保存当前选中的牌的对象
    m_curSelectedCardPanel = panel;

    // 4.参数是鼠标左键还是右键
    if (button == Qt::LeftButton)
    {
        // 设置扑克牌选中状态
        panel->setSelected(!panel->isSelecetd());
        // 更新扑克牌在窗口的位置
        updatePlayerCards(panel->getOwner());
        // 保存或删除扑克牌窗口
        QSet<CardPanel*>::ConstIterator iter = m_selectedCards.constFind(panel);
        if (iter != m_selectedCards.end())
        {
            // 这张扑克牌已经被选中，将其移除出选中集合
            m_selectedCards.erase(iter);
        }
        else
        {
            // 未被选中，添加进集合
            m_selectedCards.insert(panel);
        }
        // 播放选中扑克牌音效
        m_bgm->playAssistMusic(BGMControl::SelectCard);
    }
    else if (button == Qt::RightButton)
    {
        // 调用出牌按钮槽函数
        onUserPlayHand();
    }
}

void Gamepanel::onUserPlayHand()
{
    // 判断游戏状态
    if (m_gameStatus != GameControl::PlayingHand)
    {
        return;
    }

    // 判断出牌玩家是不是用户
    if (m_gameCtl->curPlayer() != m_gameCtl->user())
    {
        return;
    }

    // 判断出牌是否为空
    if (m_selectedCards.isEmpty())
    {
        return;
    }

    // 获取出牌的牌型
    Cards cs;
    for (auto iter = m_selectedCards.begin(); iter != m_selectedCards.end(); ++iter)
    {
        Card card = (*iter)->getCard();
        cs.add(card);
    }
    PlayHand hand(cs);
    PlayHand::HandType type = hand.type();
    if (type == PlayHand::Hand_UnKnow)
    {
        return;
    }

    // 用户玩家不是上一轮出牌的玩家，需要比较牌的大小
    // 判断出牌能不能压住上一轮打出的牌
    if (m_gameCtl->pendPlayer() != m_gameCtl->user() || m_gameCtl->pendPlayer() == nullptr)
    {
        if (!hand.canbeat(PlayHand(m_gameCtl->pendCards())))
        {
            return;
        }
    }

    // 通过用户玩家对象出牌
    m_gameCtl->user()->playHand(cs);

    // 结束倒计时
    m_countDown->stopCountDown();

    // 清空当前选中的牌的容器
    m_selectedCards.clear();
}

void Gamepanel::onUserPass()
{
    // 判断是不是用户玩家
    if (m_gameCtl->curPlayer() != m_gameCtl->user())
    {
        return;
    }

    // 判断当前用户是不是上一次出牌的玩家，或本局游戏刚刚开始，用户是第一个出牌的人，这种情况用户必须出牌不能跳过
    if (m_gameCtl->pendPlayer() == m_gameCtl->user() || m_gameCtl->pendPlayer() == nullptr)
    {
        // 如果倒计时结束后没出牌，打出一张最小的牌
        if (m_countDown->getTimeout())
        {
            CardList list = m_gameCtl->user()->getCards().toCardList();
            std::sort(list.begin(), list.end());
            Cards cards = list[0];
            m_gameCtl->user()->playHand(cards);
        }
        return;
    }

    // 打出一个空cards
    m_gameCtl->user()->playHand(Cards());

    // 用户选择不出，结束倒计时
    m_countDown->stopCountDown();

    // 清空用户选择的牌，让这些牌归位
    for (auto iter = m_selectedCards.begin(); iter != m_selectedCards.end(); ++iter)
    {
        (*iter)->setSelected(false);
    }
    m_selectedCards.clear();

    // 更新
    updatePlayerCards(m_gameCtl->user());
}

void Gamepanel::updateBeat(int beat)
{
    ui->scorePanel->setBeat(beat);
}

void Gamepanel::paintEvent(QPaintEvent* event)
{
    QPainter p(this);
    p.drawPixmap(this->rect(), m_backgroundImage);
    event->accept();
}

void Gamepanel::mouseMoveEvent(QMouseEvent* event)
{
    // 按住鼠标左键移动
    if (event->buttons() & Qt::LeftButton)
    {
        QPoint pos = event->pos();
        // 鼠标不在玩家剩余的扑克牌区域内，设置当前选中扑克牌窗口为空
        if (!m_cardsRect.contains(pos))
        {
            m_curSelectedCardPanel = nullptr;
        }
        else
        {
            QList<CardPanel*> list = m_userCards.keys();
            for (int i = 0; i < list.size(); ++i)
            {
                CardPanel* panel = list.at(i);
                // 鼠标在扑克牌窗口内，并且鼠标上一次经过的扑克牌不是这张扑克牌
                if (m_userCards[panel].contains(pos) && m_curSelectedCardPanel != panel)
                {
                    // 调用clicked发送CardSelected信号
                    panel->clicked();
                    m_curSelectedCardPanel = panel;
                }
            }
        }
    }
}

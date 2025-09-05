#include "buttongroup.h"
#include "ui_buttongroup.h"

ButtonGroup::ButtonGroup(QWidget* parent) : QWidget(parent), ui(new Ui::ButtonGroup)
{
    ui->setupUi(this);
}

ButtonGroup::~ButtonGroup()
{
    delete ui;
}

void ButtonGroup::initButtons()
{
    // 开始游戏
    ui->btnStart->setImage(":/images/start-1.png", ":/images/start-2.png", ":/images/start-3.png");

    // 出牌
    ui->btnPlayCard->setImage(
        ":/images/chupai_btn-1.png", ":/images/chupai_btn-2.png", ":/images/chupai_btn-3.png");
    ui->btnPlayCard_2->setImage(
        ":/images/chupai_btn-1.png", ":/images/chupai_btn-2.png", ":/images/chupai_btn-3.png");

    // 不要
    ui->btnPass->setImage(
        ":/images/pass_btn-1.png", ":/images/pass_btn-2.png", ":/images/pass_btn-3.png");

    // 不抢地主
    ui->btnGiveUp->setImage(
        ":/images/buqiang-1.png", ":/images/buqiang-2.png", ":/images/buqiang-3.png");

    // 抢地主1分，2分，3分
    ui->btnOneScore->setImage(":/images/1fen-1.png", ":/images/1fen-2.png", ":/images/1fen-3.png");
    ui->btnTwoScore->setImage(":/images/2fen-1.png", ":/images/2fen-2.png", ":/images/2fen-3.png");
    ui->btnThreeScore->setImage(
        ":/images/3fen-1.png", ":/images/3fen-2.png", ":/images/3fen-3.png");

    // 设置按钮大小
    QList<MyButton*> list;
    list << ui->btnStart << ui->btnPlayCard << ui->btnPlayCard_2 << ui->btnPass << ui->btnGiveUp
         << ui->btnOneScore << ui->btnTwoScore << ui->btnThreeScore;
    for (int i = 0; i < list.size(); ++i) list[i]->setFixedSize(90, 45);

    // 连接按钮信号，游戏逻辑在这里不在处理，交给其他类
    connect(ui->btnStart, &MyButton::clicked, this, &ButtonGroup::startGame);

    connect(ui->btnPlayCard, &MyButton::clicked, this, &ButtonGroup::playHand);

    connect(ui->btnPlayCard_2, &MyButton::clicked, this, &ButtonGroup::playHand);

    connect(ui->btnPass, &MyButton::clicked, this, &ButtonGroup::pass);

    connect(ui->btnGiveUp, &MyButton::clicked, this, [=]() { emit betPoint(0); });

    connect(ui->btnOneScore, &MyButton::clicked, this, [=]() { emit betPoint(1); });

    connect(ui->btnTwoScore, &MyButton::clicked, this, [=]() { emit betPoint(2); });

    connect(ui->btnThreeScore, &MyButton::clicked, this, [=]() { emit betPoint(3); });
}

void ButtonGroup::selectPanel(Panel type, int point)
{
    ui->stackedWidget->setCurrentIndex(type);
    if (type != CallLord)
        return;

    // 上个抢地主玩家下注1分的话，当前玩家只能下注2或3分
    if (point == 0)
    {
        ui->btnOneScore->setVisible(true);
        ui->btnTwoScore->setVisible(true);
        ui->btnThreeScore->setVisible(true);
    }
    else if (point == 1)
    {
        ui->btnOneScore->setVisible(false);
        ui->btnTwoScore->setVisible(true);
        ui->btnThreeScore->setVisible(true);
    }
    else if (point == 2)
    {
        ui->btnOneScore->setVisible(false);
        ui->btnTwoScore->setVisible(false);
        ui->btnThreeScore->setVisible(true);
    }
}

#include "endingpanel.h"

#include <QPaintEvent>
#include <QPainter>

EndingPanel::EndingPanel(bool isLord, bool isWin, QWidget* parent) : QWidget{parent}
{
    m_background.load(":/images/gameover.png");
    this->setFixedSize(m_background.size());

    // 显示用户角色以及游戏状态
    m_title = new QLabel(this);
    if (isLord && isWin)
    {
        m_title->setPixmap(QPixmap(":/images/lord_win.png"));
    }
    else if (isLord && !isWin)
    {
        m_title->setPixmap(QPixmap(":/images/lord_fail.png"));
    }
    else if (!isLord && isWin)
    {
        m_title->setPixmap(QPixmap(":/images/farmer_win.png"));
    }
    else if (!isLord && !isWin)
    {
        m_title->setPixmap(QPixmap(":/images/farmer_fail.png"));
    }
    m_title->move(125, 125);

    // 分数面板
    m_score = new ScorePanel(this);
    m_score->move(75, 230);
    m_score->setFixedSize(260, 168);
    m_score->setMyFontColor(ScorePanel::Red);
    m_score->setMyFontSize(18);

    // 继续游戏按钮
    m_continue = new QPushButton(this);
    m_continue->move(84, 429);
    // QString style = R"(
    //     QPushButton{border-image: url(":/images/button_normal.png"")}
    //     QPushButton:hover{border-image: url(":/images/button_hover.png"")}
    //     QPushButton:pressed{border-image: url(":/images/button_pressed.png"")}
    // )";
    QString style = R"(
    QPushButton {
        border-image: url(":/images/button_normal.png");
        border-radius: 4px;
        min-width: 80px;
        min-height: 30px;
        background: transparent;
    }
    QPushButton:hover {
        border-image: url(":/images/button_hover.png");
    }
    QPushButton:pressed {
        border-image: url(":/images/button_pressed.png");
    }
)";
    m_continue->setStyleSheet(style);
    m_continue->setFixedSize(231, 48);
    connect(m_continue, &QPushButton::clicked, this, &EndingPanel::continueGame);
}

void EndingPanel::setPlayerScore(int left, int right, int me, int beat)
{
    m_score->setScore(left, right, me);
    m_score->setBeat(beat);
}

void EndingPanel::paintEvent(QPaintEvent* event)
{
    QPainter p(this);
    p.drawPixmap(this->rect(), m_background);
    event->accept();
}

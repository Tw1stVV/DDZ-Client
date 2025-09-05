#include "animationwindow.h"

#include <QPainter>
#include <QTimer>
#include <QPaintEvent>

AnimationWindow::AnimationWindow(QWidget* parent) : QWidget{parent}
{
    m_x = 0;
}

void AnimationWindow::showBetScore(int bet)
{
    m_x = 0;
    if (bet == 1)
    {
        m_image.load(":/images/score1.png");
    }
    else if (bet == 2)
    {
        m_image.load(":/images/score2.png");
    }
    else if (bet == 3)
    {
        m_image.load(":/images/score3.png");
    }
    else
    {
        m_image = QPixmap();
    }
    this->update();
    QTimer::singleShot(1500, this, &AnimationWindow::hide);
}

void AnimationWindow::showSuquenceOrPair(Type type)
{
    m_x = 0;
    QString name = type == Sequence ? ":/images/shunzi.png" : ":/images/liandui.png";
    m_image.load(name);
    this->update();
    QTimer::singleShot(1500, this, &AnimationWindow::hide);
}

void AnimationWindow::showJokersBomb()
{
    m_x = 0;
    m_index = 0;
    QTimer* timer = new QTimer(this);
    connect(
        timer,
        &QTimer::timeout,
        this,
        [=]()
        {
            m_index++;
            if (m_index > 8)
            {
                timer->stop();
                timer->deleteLater();
                m_index = 8;
                this->hide();
            }
            QString name = QString(":/images/joker_bomb_%1.png").arg(m_index);
            m_image.load(name);
            this->update();
        });
    timer->start(60);
}

void AnimationWindow::showBomb()
{
    m_x = 0;
    m_index = 0;
    QTimer* timer = new QTimer(this);
    connect(
        timer,
        &QTimer::timeout,
        this,
        [=]()
        {
            m_index++;
            if (m_index > 12)
            {
                timer->stop();
                timer->deleteLater();
                m_index = 12;
                this->hide();
            }
            QString name = QString(":/images/bomb_%1.png").arg(m_index);
            m_image.load(name);
            this->update();
        });
    timer->start(60);
}

void AnimationWindow::showPlane()
{
    m_x = this->width();
    m_image.load(":/images/plane_1.png");
    this->setFixedHeight(m_image.height());
    this->update();

    int step = this->width() / 5;
    QTimer* timer = new QTimer(this);
    connect(
        timer,
        &QTimer::timeout,
        this,
        [=]()
        {
            static int dist = 0;
            static int times = 0;
            dist += 5;
            if (dist > step)
            {
                dist = 0;
                times++;
                QString name = QString(":/images/plane_%1.png").arg(times);
                m_image.load(name);
            }
            if (m_x <= -210)
            {
                timer->stop();
                timer->deleteLater();
                dist = times = 0;
                this->hide();
            }
            m_x -= 5;
            this->update();
        });
    timer->start(15);
}

void AnimationWindow::paintEvent(QPaintEvent* event)
{
    QPainter p(this);
    p.drawPixmap(m_x, 0, m_image.width(), m_image.height(), m_image);
    event->accept();
}

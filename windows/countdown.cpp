#include "countdown.h"

#include <QPainter>

CountDown::CountDown(QWidget* parent) : QWidget{parent}
{
    setFixedSize(70, 70);
    m_timer = new QTimer(this);
    connect(
        m_timer,
        &QTimer::timeout,
        this,
        [=]()
        {
            m_count--;
            m_timeout = false;
            if (m_count < 10 && m_count > 0)
            {
                // 加载图片
                m_clock.load(":/images/clock.png");
                m_number = QPixmap(":/images/number.png")
                               .copy(m_count * (30 + 10), 0, 30, 42)
                               .scaled(20, 30);
                // 时间快结束向主窗口发送信号
                if (m_count == 5)
                {
                    emit notTimeMuch();
                }
            }
            else if (m_count <= 0)
            {
                // 倒计时结束后， 停止倒计时，清空图片，向主窗口发送信号
                m_clock = QPixmap();
                m_number = QPixmap();
                m_timer->stop();
                m_timeout = true;
                emit timeout();
            }
            this->update();
        });
}

void CountDown::showCountDown()
{
    m_count = 15;
    m_timer->start(1000);
}

// 倒计时结束前出牌
void CountDown::stopCountDown()
{
    m_timer->stop();
    m_clock = QPixmap();
    m_number = QPixmap();
    m_timeout = false;
    this->update();
}

bool CountDown::getTimeout()
{
    return m_timeout;
}

void CountDown::paintEvent(QPaintEvent* event)
{
    QPainter p(this);
    p.drawPixmap(this->rect(), m_clock);
    p.drawPixmap(24, 24, m_number.width(), m_number.height(), m_number);
}

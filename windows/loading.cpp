#include "loading.h"

#include "gamepanel.h"

#include <QPaintEvent>
#include <QPainter>
#include <QTimer>

Loading::Loading(QWidget* parent) : QWidget{parent}
{
    bk.load(":/images/loading.png");
    this->setFixedSize(bk.size());
    dist = 0;

    // 去边框
    this->setWindowFlag(Qt::FramelessWindowHint);
    // 背景透明
    this->setAttribute(Qt::WA_TranslucentBackground);

    QPixmap pix(":/images/progress.png");
    QTimer* timer = new QTimer(this);
    connect(
        timer,
        &QTimer::timeout,
        this,
        [=]()
        {
            progress = pix.copy(0, 0, dist, pix.height());
            this->update();
            if (dist >= pix.width())
            {
                timer->stop();
                timer->deleteLater();
                Gamepanel* panel = new Gamepanel;
                panel->show();
                this->close();
            }
            dist += 5;
        });
    timer->start(15);
}

void Loading::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);
    QPainter p(this);
    p.drawPixmap(this->rect(), bk);
    p.drawPixmap(62, 417, progress.width(), progress.height(), progress);
}

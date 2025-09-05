#ifndef COUNTDOWN_H
#define COUNTDOWN_H

#include <QTimer>
#include <QWidget>

class CountDown : public QWidget
{
    Q_OBJECT
public:
    explicit CountDown(QWidget* parent = nullptr);
    void showCountDown();
    void stopCountDown();
    bool getTimeout();

signals:
    void notTimeMuch();
    void timeout();

private:
    QPixmap m_clock;
    QPixmap m_number;
    QTimer* m_timer;
    int m_count = 0;
    bool m_timeout;

    // QWidget interface
protected:
    virtual void paintEvent(QPaintEvent* event) override;
};

#endif // COUNTDOWN_H

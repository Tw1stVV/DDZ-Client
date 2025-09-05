#ifndef LOADING_H
#define LOADING_H

#include <QWidget>
class Gamepanel;
class Loading : public QWidget
{
    Q_OBJECT
public:
    explicit Loading(QWidget* parent = nullptr);

signals:
private:
    QPixmap bk;
    QPixmap progress;
    int dist;

    // QWidget interface
protected:
    virtual void paintEvent(QPaintEvent* event) override;
};

#endif // LOADING_H

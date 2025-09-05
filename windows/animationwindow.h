#ifndef ANIMATIONWINDOW_H
#define ANIMATIONWINDOW_H

#include <QWidget>

class AnimationWindow : public QWidget
{
    Q_OBJECT
public:
    enum Type
    {
        Sequence,
        Pair
    };
    explicit AnimationWindow(QWidget* parent = nullptr);
    void showBetScore(int bet);
    void showSuquenceOrPair(Type type);
    void showJokersBomb();
    void showBomb();
    void showPlane();

    enum AnimationType
    {
        Sequent,     // 顺子
        Pair_Seq,    // 连对
        Plane,       // 飞机
        Bomb,        // 炸弹
        Bomb_Jokers, // 王炸
        Bet,         // 下注展示点数
    };

signals:

    // QWidget interface
protected:
    virtual void paintEvent(QPaintEvent* event) override;

private:
    QPixmap m_image;
    int m_index;
    int m_x;
};

#endif // ANIMATIONWINDOW_H

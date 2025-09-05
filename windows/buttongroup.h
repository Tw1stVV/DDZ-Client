#ifndef BUTTONGROUP_H
#define BUTTONGROUP_H

#include <QWidget>

namespace Ui
{
    class ButtonGroup;
}

class ButtonGroup : public QWidget
{
    Q_OBJECT

public:
    enum Panel
    {
        Start,
        PlayCard,
        PassOrPlay,
        CallLord,
        Empty
    };
    explicit ButtonGroup(QWidget* parent = nullptr);
    ~ButtonGroup();

    // 初始化按钮
    void initButtons();

    // page页切换处理
    void selectPanel(Panel type, int point = 0);

signals:
    // 开始游戏
    void startGame();

    // 出牌
    void playHand();

    // 不出牌
    void pass();

    // 叫地主
    void betPoint(int bet);

private:
    Ui::ButtonGroup* ui;
};

#endif // BUTTONGROUP_H

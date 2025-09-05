#ifndef SCOREPANEL_H
#define SCOREPANEL_H

#include "qlabel.h"
#include <QWidget>

namespace Ui
{
    class ScorePanel;
}

class ScorePanel : public QWidget
{
    Q_OBJECT

public:
    enum FontColor
    {
        Black,
        White,
        Red,
        Green,
        Blue
    };
    explicit ScorePanel(QWidget* parent = nullptr);
    ~ScorePanel();
    void setScore(int left, int right, int user);
    void setMyFontSize(int point);
    void setMyFontColor(FontColor color);
    void setBeat(int beat);

private:
    QList<QLabel*> list;
    Ui::ScorePanel* ui;
};

#endif // SCOREPANEL_H

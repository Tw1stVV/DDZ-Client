#ifndef ENDINGPANEL_H
#define ENDINGPANEL_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include "scorepanel.h"

class EndingPanel : public QWidget
{
    Q_OBJECT
public:
    explicit EndingPanel(bool isLord, bool isWin, QWidget* parent = nullptr);
    void setPlayerScore(int left, int right, int me, int beat);

signals:
    void continueGame();

private:
    QPixmap m_background;
    QLabel* m_title;
    QPushButton* m_continue;
    ScorePanel* m_score;

    // QWidget interface
protected:
    virtual void paintEvent(QPaintEvent* event) override;
};

#endif // ENDINGPANEL_H

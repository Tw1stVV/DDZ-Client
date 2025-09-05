#include "scorepanel.h"
#include "ui_scorepanel.h"
const QString MYCOLOR[] = {"black", "white", "red", "green", "blue"};

ScorePanel::ScorePanel(QWidget* parent) : QWidget(parent), ui(new Ui::ScorePanel)
{
    ui->setupUi(this);
    list << ui->label_3 << ui->label_4 << ui->label_8 << ui->myTitle << ui->leftTitle
         << ui->rightTitle << ui->myScore << ui->leftScore << ui->rightScore << ui->beatLebel
         << ui->beatScore << ui->label_9;
}

ScorePanel::~ScorePanel()
{
    delete ui;
}

void ScorePanel::setScore(int left, int right, int user)
{
    ui->leftScore->setText(QString::number(left));
    ui->rightScore->setText(QString::number(right));
    ui->myScore->setText(QString::number(user));
}

void ScorePanel::setMyFontSize(int point)
{
    QFont font("微软雅黑", point, QFont::Bold);
    for (int i = 0; i < list.size(); ++i)
    {
        list[i]->setFont(font);
    }
}

void ScorePanel::setMyFontColor(FontColor color)
{
    QString style = QString("QLabel{color:%1}").arg(MYCOLOR[color]);
    for (int i = 0; i < list.size(); ++i)
    {
        list[i]->setStyleSheet(style);
    }
}

void ScorePanel::setBeat(int beat)
{
    ui->beatScore->setText(QString::number(beat));
}

#include "gamemode.h"
#include "gamepanel.h"
#include "ui_gamemode.h"
#include <qevent.h>

GameMode::GameMode(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::GameMode)
{
    ui->setupUi(this);

    ui->stackedWidget->setCurrentWidget(ui->mode_page);

    connect(ui->single_btn, &QPushButton::clicked, this, [=]() {
        Gamepanel *panel = new Gamepanel;
        connect(panel, &Gamepanel::ClosePanel, this, &GameMode::show);
        panel->show();
        this->hide();
    });

    connect(ui->net_btn, &QPushButton::clicked, this, [=]() {
        ui->stackedWidget->setCurrentWidget(ui->room_page);
    });
}

GameMode::~GameMode()
{
    delete ui;
}

void GameMode::closeEvent(QCloseEvent *event)
{
    if (ui->stackedWidget->currentWidget() == ui->room_page)
    {
        ui->stackedWidget->setCurrentWidget(ui->mode_page);
        event->ignore();
    }
    else
    {
        event->accept();
    }
}

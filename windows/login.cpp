#include "login.h"
#include "datamanager.h"
#include "ui_login.h"
#include "gamepanel.h"

#include <QLineEdit>
#include <QRegularExpression>
#include <QRegularExpressionValidator>

Login::Login(QWidget* parent) : QDialog(parent), ui(new Ui::Login)
{
    ui->setupUi(this);

    ui->stackedWidget->setCurrentWidget(ui->login_page);

    connect(
        ui->back_home_btn,
        &QPushButton::clicked,
        this,
        [&]() { ui->stackedWidget->setCurrentWidget(ui->login_page); });

    connect(
        ui->show_network_btn,
        &QPushButton::clicked,
        this,
        [&]() { ui->stackedWidget->setCurrentWidget(ui->network_page); });

    connect(
        ui->login_register_btn,
        &QPushButton::clicked,
        this,
        [&]() { ui->stackedWidget->setCurrentWidget(ui->register_page); });

    // 数据校验
    /* 验证登录用户名和注册用户名
     * 长度3-16位
     * 由大写字母，小写字母，数组，和下划线组成
     */
    QRegularExpression expreg("^[a-zA-Z0-9_]{3,16}$");
    QRegularExpressionValidator* valiUsername = new QRegularExpressionValidator(expreg, this);
    ui->login_username_ed->setValidator(valiUsername);
    ui->register_username_ed->setValidator(valiUsername);

    /* 验证登录密码和注册密码
     * 长度4-12位
     * 至少包含一个大写字母，一个小写字母，不允许包含特殊字符
     */
    expreg.setPattern("^(?=.*[A-Z])(?=.*[a-z])(?=.*\\d)[A-Za-z\\d]{4,12}$");
    QRegularExpressionValidator* valiPwd = new QRegularExpressionValidator(expreg, this);
    ui->login_password_ed->setValidator(valiPwd);
    ui->register_password_ed->setValidator(valiPwd);

    /* 验证注册手机号
     * 长度11位
     * 由1开头，第二位是3-9，最后匹配9个数字
     */
    expreg.setPattern("^1[3456789]\\d{9}$");
    QRegularExpressionValidator* valiPhone = new QRegularExpressionValidator(expreg, this);
    ui->register_phone_ed->setValidator(valiPhone);

    /* 验证IP地址
     *
     */
    expreg.setPattern(
        "^(((\\d|[1-9]\\d|1\\d{2}|2[0-4]\\d|25[0-5])\\.){3}(\\d|[1-9]\\d|1\\d{2}|2[0-4]\\d|25[0-5]))$");
    QRegularExpressionValidator* valiIP = new QRegularExpressionValidator(expreg, this);
    ui->net_ipaddress_ed->setValidator(valiIP);

    /* 验证端口
     *0-65535
     */
    expreg.setPattern(
        "^(0|[1-9]\\d{0,3}|[1-5]\\d{4}|6[0-4]\\d{3}|65[0-4]\\d{2}|655[0-2]\\d|6553[0-5])$");
    QRegularExpressionValidator* valiPort = new QRegularExpressionValidator(expreg, this);
    ui->net_port_ed->setValidator(valiPort);

    connect(ui->login_login_btn, &QPushButton::clicked, this, &Login::onLogin);
    connect(ui->register_register_btn, &QPushButton::clicked, this, &Login::onRegister);
    connect(ui->net_on_btn, &QPushButton::clicked, this, &Login::onNetOK);
}

bool Login::verifyData(QLineEdit* edit)
{
    if (edit->hasAcceptableInput() == false)
    {
        // 设置红色边框提醒检验失败
        edit->setStyleSheet("border: 2px solid red;");
        return false;
    }
    else
    {
        edit->setStyleSheet("none");
    }
    return true;
}

Login::~Login()
{
    delete ui;
}

void Login::onLogin()
{
    bool flagName, flagPwd;
    flagName = verifyData(ui->login_username_ed);
    flagPwd = verifyData(ui->login_password_ed);
    if (flagName && flagPwd)
    {
        // 显示游戏窗口
        Gamepanel* pannel = new Gamepanel();
        pannel->show();
    }
}

void Login::onRegister()
{
    bool flagName, flagPwd, flagPhone;
    flagName = verifyData(ui->register_username_ed);
    flagPwd = verifyData(ui->register_password_ed);
    flagPhone = verifyData(ui->register_phone_ed);
    if (flagName && flagPwd && flagPhone)
    {
        ui->stackedWidget->setCurrentWidget(ui->login_page);
    }
}

void Login::onNetOK()
{
    bool flagIpaddr, flagPort;
    flagIpaddr = verifyData(ui->net_ipaddress_ed);
    flagPort = verifyData(ui->net_port_ed);
    if (flagIpaddr && flagPort)
    {
        DataManager* instance = DataManager::getInstance();
        instance->setIpaddr(ui->net_ipaddress_ed->text().toUtf8());
        instance->setPort(ui->net_port_ed->text().toUtf8());
    }
}

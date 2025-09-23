#include "login.h"
#include "aescrypto.h"
#include "communication.h"
#include "datamanager.h"
#include "gamepanel.h"
#include "ui_login.h"

#include <qjsondocument.h>
#include <qjsonobject.h>
#include <QCryptographicHash>
#include <QFile>
#include <QLineEdit>
#include <QMessageBox>
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include <QThreadPool>

Login::Login(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Login)
{
    ui->setupUi(this);

    ui->stackedWidget->setCurrentWidget(ui->login_page);

    connect(ui->back_home_btn, &QPushButton::clicked, this, [&]() {
        //登录界面index 0 注册 1 网络设置 2
        ui->stackedWidget->setCurrentIndex(0);
        update();
    });

    connect(ui->show_network_btn, &QPushButton::clicked, this, [&]() {
        //登录界面index 0 注册 1 网络设置 2
        ui->stackedWidget->setCurrentIndex(2);
        update();
    });

    connect(ui->login_register_btn, &QPushButton::clicked, this, [&]() {
        //登录界面index 0 注册 1 网络设置 2
        ui->stackedWidget->setCurrentIndex(1);
        update();
    });

    // 数据校验
    /* 验证登录用户名和注册用户名
     * 长度3-16位
     * 由大写字母，小写字母，数组，和下划线组成
     */
    QRegularExpression expreg("^[a-zA-Z0-9_]{3,16}$");
    QRegularExpressionValidator *valiUsername = new QRegularExpressionValidator(expreg, this);
    ui->login_username_ed->setValidator(valiUsername);
    ui->register_username_ed->setValidator(valiUsername);

    /* 验证登录密码和注册密码
     * 长度4-12位
     * 至少包含一个大写字母，一个小写字母，不允许包含特殊字符
     */
    expreg.setPattern("^(?=.*[A-Z])(?=.*[a-z])(?=.*\\d)[A-Za-z\\d]{4,12}$");
    QRegularExpressionValidator *valiPwd = new QRegularExpressionValidator(expreg, this);
    ui->login_password_ed->setValidator(valiPwd);
    ui->register_password_ed->setValidator(valiPwd);

    /* 验证注册手机号
     * 长度11位
     * 由1开头，第二位是3-9，最后匹配9个数字
     */
    expreg.setPattern("^1[3456789]\\d{9}$");
    QRegularExpressionValidator *valiPhone = new QRegularExpressionValidator(expreg, this);
    ui->register_phone_ed->setValidator(valiPhone);

    /* 验证IP地址
     *
     */
    expreg.setPattern("^(((\\d|[1-9]\\d|1\\d{2}|2[0-4]\\d|25[0-5])\\.){3}(\\d|[1-9]\\d|1\\d{2}|2[0-"
                      "4]\\d|25[0-5]))$");
    QRegularExpressionValidator *valiIP = new QRegularExpressionValidator(expreg, this);
    ui->net_ipaddress_ed->setValidator(valiIP);

    /* 验证端口
     *0-65535
     */
    expreg.setPattern(
        "^(0|[1-9]\\d{0,3}|[1-5]\\d{4}|6[0-4]\\d{3}|65[0-4]\\d{2}|655[0-2]\\d|6553[0-5])$");
    QRegularExpressionValidator *valiPort = new QRegularExpressionValidator(expreg, this);
    ui->net_port_ed->setValidator(valiPort);

    connect(ui->login_login_btn, &QPushButton::clicked, this, &Login::onLogin);
    connect(ui->register_register_btn, &QPushButton::clicked, this, &Login::onRegister);
    connect(ui->net_on_btn, &QPushButton::clicked, this, &Login::onNetOK);

    // test
    // ui->login_username_ed->setText("test");
    // ui->login_password_ed->setText("95184726Ww");

    //加载密码
    loadUserInfo();
}

bool Login::verifyData(QLineEdit *edit)
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

void Login::startConnect(Message *msg)
{
    if (m_isConnected)
    {
        // 已经和服务器建立连接
        // 和服务器通信
        Communication *task = DataManager::getInstance()->communication();
        task->sendMessage(msg);
    }
    else
    {
        // 尚未和服务器建立连接
        Communication *task = new Communication(*msg);
        connect(task, &Communication::connectFailed, this, [&]() {
            QMessageBox::critical(this, "连接服务器", "连接服务器失败");
            m_isConnected = false;
        });

        // 选择游戏模式-单机或联机
        connect(task, &Communication::LoginOK, this, [&]() {
            //保存用户名
            DataManager::getInstance()->setUsername(ui->login_username_ed->text().toUtf8());
            QMessageBox::information(this, "登录", "登录成功");
            //保存用户名和密码
            saveUserInfo();
        });

        // 注册成功切换到登录界面
        connect(task, &Communication::RegisterOK, this, [&]() {
            QMessageBox::information(this, "注册", "注册成功");
            //登录界面index 0 注册 1 网络设置 2
            ui->stackedWidget->setCurrentIndex(0);
            update();
        });

        connect(task, &Communication::FailedMsg, this, [&](QByteArray msg) {
            QMessageBox::critical(this, "错误", msg);
        });

        DataManager::getInstance()->setCommunication(task);
        QThreadPool::globalInstance()->start(task);
        m_isConnected = true;
    }
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
        Message msg;
        msg.userName = ui->login_username_ed->text().toUtf8();
        msg.reqCode = RequestCode::UserLogin;

        QByteArray pwd = ui->login_password_ed->text().toUtf8();

        msg.data1 = QCryptographicHash::hash(pwd, QCryptographicHash::Sha224).toHex();

        // 连接服务器
        startConnect(&msg);
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
        // ui->stackedWidget->setCurrentWidget(ui->login_page);

        Message msg;
        msg.userName = ui->register_username_ed->text().toUtf8();
        msg.reqCode = RequestCode::Register;

        QByteArray pwd = ui->register_password_ed->text().toUtf8();

        msg.data1 = QCryptographicHash::hash(pwd, QCryptographicHash::Sha224).toHex();
        msg.data2 = ui->register_phone_ed->text().toUtf8();

        // 连接服务器
        startConnect(&msg);
    }
}

void Login::onNetOK()
{
    bool flagIpaddr, flagPort;
    flagIpaddr = verifyData(ui->net_ipaddress_ed);
    flagPort = verifyData(ui->net_port_ed);
    if (flagIpaddr && flagPort)
    {
        DataManager *instance = DataManager::getInstance();
        instance->setIpaddr(ui->net_ipaddress_ed->text().toUtf8());
        instance->setPort(ui->net_port_ed->text().toUtf8());
    }
}

void Login::saveUserInfo()
{
    if (ui->login_save_pwd->isChecked())
    {
        //json保存用户名和密码
        QJsonObject obj;
        obj.insert("username", ui->login_username_ed->text());
        obj.insert("passwd", ui->login_password_ed->text());
        QJsonDocument doc(obj);
        QByteArray json = doc.toJson();
        //aes加密
        AesCrypto aes(AesCrypto::AES_CBC_128, KEY.left(16));
        json = aes.enCrypto(json);
        //写入文件
        QFile file("userinfo.bin");
        file.open(QIODevice::WriteOnly);
        file.write(json);
        file.close();
    }
    else
    {
        QFile file("userinfo.bin");
        file.remove();
    }
}

void Login::loadUserInfo()
{
    QFile file("userinfo.bin");
    bool isSave = file.open(QIODevice::ReadOnly);
    if (isSave)
    {
        ui->login_save_pwd->setChecked(true);
        QByteArray data = file.readAll();
        AesCrypto aes(AesCrypto::AES_CBC_128, KEY.left(16));
        data = aes.deCrypto(data);
        QJsonDocument doc = QJsonDocument::fromJson(data);
        QJsonObject json = doc.object();
        ui->login_username_ed->setText(json.value("username").toString());
        ui->login_password_ed->setText(json.value("passwd").toString());
    }
    else
    {
        ui->login_save_pwd->setChecked(false);
    }
}

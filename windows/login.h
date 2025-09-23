#ifndef LOGIN_H
#define LOGIN_H

#include "codec.h"
#include <QDialog>

class QLineEdit;
namespace Ui {
class Login;
}

class Login : public QDialog
{
    Q_OBJECT

public:
    explicit Login(QWidget* parent = nullptr);
    bool verifyData(QLineEdit* edit);
    void startConnect(Message* msg);
    ~Login();

private slots:
    void onLogin();
    void onRegister();
    void onNetOK();

private:
    Ui::Login* ui;
    bool m_isConnected = false;
};

#endif // LOGIN_H

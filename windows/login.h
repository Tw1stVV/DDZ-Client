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

    void saveUserInfo();
    void loadUserInfo();

private:
    Ui::Login* ui;
    bool m_isConnected = false;
    const QByteArray KEY = "王师北定中原日，家祭勿忘告乃翁";
};

#endif // LOGIN_H

#ifndef LOGIN_H
#define LOGIN_H

#include <QDialog>

class QLineEdit;
namespace Ui
{
    class Login;
}

class Login : public QDialog
{
    Q_OBJECT

public:
    explicit Login(QWidget* parent = nullptr);
    bool verifyData(QLineEdit* edit);
    ~Login();

private slots:
    void onLogin();
    void onRegister();
    void onNetOK();

private:
    Ui::Login* ui;
};

#endif // LOGIN_H

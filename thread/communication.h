#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include "codec.h"
#include "tcpsocket.h"
#include <QObject>
#include <QRunnable>

class AesCrypto;

// 服务器通信子线程
class Communication : public QObject, public QRunnable
{
    Q_OBJECT
public:
    enum KeyLen { L16 = 16, L24 = 24, L32 = 32 };
    explicit Communication(const Message& msg, QObject* parent = nullptr);
    ~Communication();
    inline void stopLoop() { m_stop = true; }

    void sendMessage(Message* msg, bool aescrpto = true);

    // 解析服务器发送数据
    void recvMessage();

signals:
    void connectFailed();
    void LoginOK();
    void RegisterOK();
    void FailedMsg(QByteArray msg);

    // QRunnable interface
public:
    virtual void run() override;

private:
    // 处理密钥分发
    void handleRsaDistribute(QSharedPointer<Message> ptr);
    // 生成aes密钥
    QByteArray generateAesKey(KeyLen len);

private:
    QByteArray m_aesKey;
    AesCrypto* m_aes = nullptr;
    TcpSocket* m_socket;
    Message m_msgInfo;
    bool m_stop = false;
};

#endif // COMMUNICATION_H

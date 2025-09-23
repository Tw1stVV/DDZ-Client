#include "communication.h"
#include "aescrypto.h"
#include "datamanager.h"
#include "rsacrypto.h"
#include <QDateTime>
#include <QDebug>
#include <QThread>
#include <QtWidgets/QMessageBox>

Communication::Communication(const Message& msg, QObject* parent)
    : QObject{parent}
    , m_msgInfo(msg)
{
    setAutoDelete(true);
}

Communication::~Communication()
{
    if (m_aes != nullptr)
    {
        delete m_aes;
    }
}

void Communication::sendMessage(Message* msg, bool aescrpto)
{
    qDebug() << "request: " << msg->reqCode;
    Codec c(msg);
    QByteArray data = c.encodeMsg();

    qDebug("----加密前begin------");
    qDebug() << data.toHex();
    qDebug("----加密前end------");

    // 加密数据
    if (aescrpto && m_aes != nullptr)
    {
        data = m_aes->enCrypto(data);
    }

    qDebug("----aes key begin------");
    qDebug() << m_aesKey.toHex();
    qDebug("----aes key end------");

    qDebug("----加密后begin------");
    qDebug() << data.toHex();
    qDebug("----加密后end------");
    m_socket->sendMsg(data);
}

void Communication::recvMessage()
{
    // 读取数据最多阻塞两秒
    QByteArray data = m_socket->recvMsg(2);
    if (data.isEmpty())
    {
        // qDebug() << "recv message is empty";
        return;
    }
    // 反序列化数据
    Codec c(data);
    QSharedPointer<Message> ptr = c.decodeMsg();
    switch (ptr->resCode)
    {
    case ResponseCode::LoginOK:
        emit LoginOK();
        break;
    case ResponseCode::RegisterOK:
        emit RegisterOK();
        break;
    case ResponseCode::RsaDistribute:
        handleRsaDistribute(ptr);
        break;
    case ResponseCode::AesVerifyOK:
        // 密钥分发成功创建aes对象
        m_aes = new AesCrypto(AesCrypto::AES_CBC_256, m_aesKey);
        qDebug() << "aes校验成功";
        sendMessage(&m_msgInfo);
        break;
    case ResponseCode::Failed:
        emit FailedMsg(ptr->data1);
        break;
    default:
        break;
    }
}

void Communication::run()
{
    DataManager* obj = DataManager::getInstance();
    m_socket = new TcpSocket();
    bool flag = m_socket->connectHost(obj->ipaddr(), obj->port().toUShort());
    qDebug() << "---------通信线程ID: " << QThread::currentThreadId() << " connected: " << flag;
    if (!flag)
    {
        // 连接失败，通知主线程
        emit connectFailed();
        delete m_socket;
        return;
    }

    while (!m_stop)
    {
        // 处理服务器回复数据
        // 获取非对称加密公钥，生成对称加密密钥，加密密钥发送给服务器，等待服务器回复，再发送登录或注册请求
        recvMessage();
    }
    m_socket->disConnect();
    delete m_socket;
}

void Communication::handleRsaDistribute(QSharedPointer<Message> ptr)
{
    RsaCrypto rsa;
    rsa.StringToKey(ptr->data1, RsaCrypto::PublicKey);
    // 校验签名
    bool flag = rsa.verify(ptr->data2, ptr->data1);
    if (!flag)
    {
        QMessageBox::critical(nullptr, "签名验证", "签名验证错误（不安全的连接）");
        return;
    }
    // 生成对称加密密钥
    m_aesKey = generateAesKey(L32);
    Message resMsg;
    // 加密私钥
    resMsg.data1 = rsa.pubKeyEncrypt(m_aesKey);
    // 哈希校验码
    resMsg.data2 = QCryptographicHash::hash(m_aesKey, QCryptographicHash::Sha224).toHex();
    // 请求码
    resMsg.reqCode = RequestCode::AesDistribute;
    // 向服务器发送aes密钥
    sendMessage(&resMsg, false);
}

QByteArray Communication::generateAesKey(KeyLen len)
{
    QByteArray date = QDateTime::currentDateTime().toString("yyyy.MM.dd-hh:mm:ss.zzz").toUtf8();
    // 计算哈希值
    QCryptographicHash hash(QCryptographicHash::Sha384);
    hash.addData(date);
    QByteArray result = hash.result();
    result = result.left(len);
    return result;
}

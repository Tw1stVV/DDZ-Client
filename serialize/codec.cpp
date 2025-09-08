#include "codec.h"
#include <QDebug>

Codec::Codec(Message* msg)
{
    reLoad(msg);
}

Codec::Codec(QByteArray msg)
{
    reLoad(msg);
}

QByteArray Codec::encodeMsg()
{
    std::string out;
    m_obj.SerializeToString(&out);
    return QByteArray::fromStdString(out);
}

QSharedPointer<Message> Codec::uncodeMsg()
{
    std::string data = m_msg.toStdString();
    m_obj.ParseFromString(data);
    Message* msg = new Message();
    msg->userName = QByteArray::fromStdString(m_obj.username());
    msg->data1 = QByteArray::fromStdString(m_obj.data1());
    msg->data2 = QByteArray::fromStdString(m_obj.data2());
    msg->data3 = QByteArray::fromStdString(m_obj.data3());
    msg->reqCode = m_obj.reqcode();
    msg->resCode = m_obj.rescode();
    QSharedPointer<Message> ptr(
        msg,
        [this](Message* p)
        {
            delete p;
            qDebug() << "Message被释放";
        });
    return ptr;
}

void Codec::reLoad(Message* msg)
{
    m_obj.set_username(msg->userName.toStdString());
    m_obj.set_data1(msg->data1.toStdString());
    m_obj.set_data2(msg->data2.toStdString());
    m_obj.set_data3(msg->data3.toStdString());
    m_obj.set_reqcode(msg->reqCode);
    m_obj.set_rescode(msg->resCode);
}

void Codec::reLoad(QByteArray msg)
{
    m_msg = msg;
}

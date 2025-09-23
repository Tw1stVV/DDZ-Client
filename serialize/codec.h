#ifndef CODEC_H
#define CODEC_H
#include "information.pb.h"
#include <QByteArray>
#include <QSharedPointer>

struct Message
{
    QByteArray userName;
    QByteArray data1;
    QByteArray data2;
    QByteArray data3;
    RequestCode reqCode;
    ResponseCode resCode;
};

class Codec
{
public:
    // 序列化
    Codec(Message* msg);

    // 反序列化
    Codec(QByteArray msg);

    // 数据编码
    QByteArray encodeMsg();

    // 数据解码
    QSharedPointer<Message> decodeMsg();

    // 重新加载数据
    void reLoad(Message* msg);
    void reLoad(QByteArray msg);

private:
    QByteArray m_msg;
    Informaion m_obj; // 用于数据的序列号和反序列化
};

#endif // CODEC_H

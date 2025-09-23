#ifndef BASE64_H
#define BASE64_H

#include <QObject>
#include <QByteArray>

class Base64 : public QObject
{
    Q_OBJECT
public:
    explicit Base64(QObject* parent = nullptr);

    // 编码
    static QByteArray encode(QByteArray data);
    static QByteArray encode(const char* data, int len);
    // 解码
    static QByteArray decode(QByteArray data);
    static QByteArray decode(const char* data, int len);

signals:
};

#endif // BASE64_H

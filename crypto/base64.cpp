#include "base64.h"
#include "openssl/bio.h"
#include "openssl/evp.h"
#include "openssl/buffer.h"

Base64::Base64(QObject* parent) : QObject{parent}
{
}

QByteArray Base64::encode(QByteArray data)
{
    return encode(data.data(), data.size());
}

QByteArray Base64::encode(const char* data, int len)
{
    BIO* b64 = BIO_new(BIO_f_base64());
    BIO* mem = BIO_new(BIO_s_mem());

    // 组织bio链
    BIO_push(b64, mem);

    // 数据编码
    BIO_write(b64, data, len);
    BIO_flush(b64);

    // 读取编码后的数据
    BUF_MEM* buf;
    BIO_get_mem_ptr(b64, &buf);

    QByteArray str(buf->data, buf->length);
    BIO_free_all(b64);
    return str;
}

QByteArray Base64::decode(QByteArray data)
{
    return decode(data.data(), data.size());
}

QByteArray Base64::decode(const char* data, int len)
{
    BIO* b64 = BIO_new(BIO_f_base64());
    BIO* mem = BIO_new(BIO_s_mem());

    // 组织bio链
    BIO_push(b64, mem);

    // 写入编码的数据
    BIO_write(mem, data, len);

    // 解码
    char* buf = new char[len];
    int ret = BIO_read(b64, buf, len);

    QByteArray str(buf, ret);
    BIO_free_all(b64);
    delete[] buf;
    return str;
}

#include "aescrypto.h"
#include "cards.h"
#include "login.h"
#include "rsacrypto.h"
#include <QApplication>
#include <QResource>
#include <QThreadPool>

void text()
{
    AesCrypto aes(AesCrypto::AES_CBC_128, "1234567812345678");
    QByteArray text = aes.enCrypto(
        "EVP_CipherInit_ex 是OpenSSL库中的一个函数，"
        "它用于初始化加密或解密操作的密码上下文。这个函数是对称加密操作中非常关键的一部分，"
        "它允许开发者指定加密算法、密钥、初始化向量（IV）以及加密或解密的方向。");
    qDebug() << "密文:\n" << text.data();
    text = aes.deCrypto(text);
    qDebug() << "明文:\n" << text.data();

    RsaCrypto rsa;
    rsa.generateRsaKey(RsaCrypto::BITS_2K);

    QByteArray temp("王师北定中原日，家祭勿忘告乃翁");
    RsaCrypto rsaPub("public_key.pem", RsaCrypto::PublicKey);
    temp = rsaPub.pubKeyEncrypt(temp);
    qDebug() << "密文:\n" << temp.data();

    RsaCrypto rsaPri("private_key.pem", RsaCrypto::PrivateKey);
    temp = rsaPri.priKeyDecrypt(temp);
    qDebug() << "明文:\n" << temp.data();

    temp = rsaPri.sign(text);
    qDebug() << "签名:\n" << temp.data();
    qDebug() << "验证: " << rsaPub.verify(temp, text);
}

int main(int argc, char* argv[])
{
    // 设置线程池最大线程数
    QThreadPool::globalInstance()->setMaxThreadCount(8);
    QApplication a(argc, argv);
    qRegisterMetaType<Cards>("Cards&");
    QResource::registerResource("./resource.rcc");
    Login w;
    int ret = w.exec();
    if (ret == QDialog::Accepted)
    {
        return a.exec();
    }
    return 0;
}

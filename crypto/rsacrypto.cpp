#include "rsacrypto.h"
#include "openssl/evp.h"
#include "openssl/rsa.h"
#include "openssl/pem.h"
#include "base64.h"

RsaCrypto::RsaCrypto(QObject* parent) : QObject{parent}
{
}

RsaCrypto::RsaCrypto(QByteArray fileName, KeyType type, QObject* parent) : QObject{parent}
{
    BIO* bio = BIO_new_file(fileName.data(), "rb");
    assert(bio != NULL);
    if (type == KeyType::PublicKey)
    {
        PEM_read_bio_PUBKEY(bio, &m_pubKey, NULL, NULL);
    }
    else
    {
        PEM_read_bio_PrivateKey(bio, &m_priKey, NULL, NULL);
    }
    BIO_free(bio);
}

RsaCrypto::~RsaCrypto()
{
    if (m_pubKey)
    {
        EVP_PKEY_free(m_pubKey);
    }
    if (m_priKey)
    {
        EVP_PKEY_free(m_priKey);
    }
}

void RsaCrypto::StringToKey(QByteArray data, KeyType type)
{
    BIO* bio = BIO_new_mem_buf(data.data(), data.size());
    assert(bio != NULL);
    if (type == KeyType::PublicKey)
    {
        PEM_read_bio_PUBKEY(bio, &m_pubKey, NULL, NULL);
    }
    else
    {
        PEM_read_bio_PrivateKey(bio, &m_priKey, NULL, NULL);
    }
    BIO_free(bio);
}

void RsaCrypto::generateRsaKey(KeyLength bits, QByteArray pub, QByteArray pri)
{
    // 创建密钥上下文
    EVP_PKEY_CTX* ctx = EVP_PKEY_CTX_new_id(EVP_PKEY_RSA, NULL);
    // 初始化ctx
    int ret = EVP_PKEY_keygen_init(ctx);
    assert(ret == 1);

    // 指定密钥长度
    ret = EVP_PKEY_CTX_set_rsa_keygen_bits(ctx, bits);
    assert(ret == 1);

    // 生成密钥对
    ret = EVP_PKEY_generate(ctx, &m_priKey);
    assert(ret);

    // 释放密钥上下文
    EVP_PKEY_CTX_free(ctx);

    // 公钥写入磁盘
    BIO* bio = BIO_new_file(pub.data(), "wb");
    ret = PEM_write_bio_PUBKEY(bio, m_priKey);
    assert(ret == 1);
    BIO_flush(bio);
    BIO_free(bio);

    // 私钥写入磁盘
    bio = BIO_new_file(pri.data(), "wb");
    ret = PEM_write_bio_PrivateKey(bio, m_priKey, NULL, NULL, 0, NULL, NULL); // 未设置私钥加密
    assert(ret == 1);
    BIO_flush(bio);
    BIO_free(bio);
}

QByteArray RsaCrypto::pubKeyEncrypt(QByteArray data)
{
    // 创建上下文
    EVP_PKEY_CTX* ctx = EVP_PKEY_CTX_new(m_pubKey, NULL);
    assert(ctx);

    // 初始化上下文
    int ret = EVP_PKEY_encrypt_init(ctx);
    assert(ret == 1);

    // 设置填充
    ret = EVP_PKEY_CTX_set_rsa_padding(ctx, RSA_PKCS1_OAEP_PADDING);
    assert(ret == 1);

    // 使用公钥加密
    size_t outLen = 0;
    // 获取加密后的数据长度
    ret = EVP_PKEY_encrypt(
        ctx, NULL, &outLen, reinterpret_cast<const unsigned char*>(data.data()), data.size());
    assert(ret == 1);

    // 根据加密后的数据长度申请内存
    unsigned char* out = new unsigned char[outLen];
    ret = EVP_PKEY_encrypt(
        ctx, out, &outLen, reinterpret_cast<const unsigned char*>(data.data()), data.size());
    assert(ret == 1);

    // base64编码
    QByteArray retData = Base64::encode(reinterpret_cast<char*>(out), outLen);

    // 释放内存
    delete[] out;
    EVP_PKEY_CTX_free(ctx);

    return retData;
}

QByteArray RsaCrypto::priKeyDecrypt(QByteArray data)
{
    // base64解码
    data = Base64::decode(data);

    // 创建上下文
    EVP_PKEY_CTX* ctx = EVP_PKEY_CTX_new(m_priKey, NULL);
    assert(ctx);

    // 初始化上下文
    int ret = EVP_PKEY_decrypt_init(ctx);
    assert(ret == 1);

    // 设置填充
    ret = EVP_PKEY_CTX_set_rsa_padding(ctx, RSA_PKCS1_OAEP_PADDING);
    assert(ret == 1);

    // 使用私钥解密
    size_t outLen = 0;
    // 获取解密后的数据长度
    ret = EVP_PKEY_decrypt(
        ctx, NULL, &outLen, reinterpret_cast<const unsigned char*>(data.data()), data.size());
    assert(ret == 1);

    // 根据解密后的数据长度申请内存
    unsigned char* out = new unsigned char[outLen];
    ret = EVP_PKEY_decrypt(
        ctx, out, &outLen, reinterpret_cast<const unsigned char*>(data.data()), data.size());
    assert(ret == 1);

    QByteArray retData(reinterpret_cast<char*>(out), outLen);

    // 释放内存
    delete[] out;
    EVP_PKEY_CTX_free(ctx);

    return retData;
}

QByteArray RsaCrypto::sign(QByteArray data, QCryptographicHash::Algorithm hash)
{
    // 计算哈希值
    QCryptographicHash h(hash);
    h.addData(data);
    QByteArray md = h.result();

    // 创建上下文
    EVP_PKEY_CTX* ctx = EVP_PKEY_CTX_new(m_priKey, NULL);
    assert(ctx);

    // 初始化
    int ret = EVP_PKEY_sign_init(ctx);
    assert(ret == 1);

    // 设置填充
    ret = EVP_PKEY_CTX_set_rsa_padding(
        ctx, RSA_PKCS1_PADDING); // 签名不能用RSA_PKCS1_OAEP_PADDING进行填充
    assert(ret == 1);

    // 设置签名使用的哈希算法
    ret = EVP_PKEY_CTX_set_signature_md(ctx, m_hashMethods.value(hash)());
    assert(ret == 1);

    // 设置签名
    size_t outLen = 0;
    ret = EVP_PKEY_sign(
        ctx, NULL, &outLen, reinterpret_cast<const unsigned char*>(md.data()), md.size());
    assert(ret == 1);

    unsigned char* out = new unsigned char[outLen];
    ret = EVP_PKEY_sign(
        ctx, out, &outLen, reinterpret_cast<const unsigned char*>(md.data()), md.size());
    assert(ret == 1);

    // base64编码
    QByteArray retData = Base64::encode(reinterpret_cast<char*>(out), outLen);

    // 释放内存
    delete[] out;
    EVP_PKEY_CTX_free(ctx);

    return retData;
}

bool RsaCrypto::verify(QByteArray sign, QByteArray data, QCryptographicHash::Algorithm hash)
{
    // base64解码
    sign = Base64::decode(sign);

    // 计算哈希值
    QCryptographicHash h(hash);
    h.addData(data);
    QByteArray md = h.result();

    // 创建上下文
    EVP_PKEY_CTX* ctx = EVP_PKEY_CTX_new(m_pubKey, NULL);
    assert(ctx);

    // 初始化
    int ret = EVP_PKEY_verify_init(ctx);
    assert(ret == 1);

    // 设置填充
    ret = EVP_PKEY_CTX_set_rsa_padding(ctx, RSA_PKCS1_PADDING);
    assert(ret == 1);

    // 设置签名使用的哈希算法
    ret = EVP_PKEY_CTX_set_signature_md(ctx, m_hashMethods.value(hash)());
    assert(ret == 1);

    // 验证签名
    size_t outLen = 0;
    ret = EVP_PKEY_verify(
        ctx,
        reinterpret_cast<const unsigned char*>(sign.data()),
        sign.size(),
        reinterpret_cast<const unsigned char*>(md.data()),
        md.size());

    // 释放内存
    EVP_PKEY_CTX_free(ctx);

    if (ret == 1)
    {
        return true;
    }

    return false;
}

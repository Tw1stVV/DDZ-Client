#ifndef DATAMANAGER_H
#define DATAMANAGER_H
#include <QByteArray>

class DataManager
{
public:
    // 禁用拷贝构造和赋值运算
    DataManager(const DataManager&) = delete;
    DataManager& operator=(const DataManager&) = delete;
    static DataManager* getInstance();

    QByteArray username() const;
    void setUsername(const QByteArray &newUsername);

    QByteArray ipaddr() const;
    void setIpaddr(const QByteArray &newIpaddr);

    QByteArray port() const;
    void setPort(const QByteArray &newPort);

private:
    DataManager() = default;
    static DataManager* m_data;

    QByteArray m_username;
    QByteArray m_ipaddr;
    QByteArray m_port;
};
#endif // DATAMANAGER_H

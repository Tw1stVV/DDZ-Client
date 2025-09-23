#ifndef DATAMANAGER_H
#define DATAMANAGER_H
#include "communication.h"
#include <QByteArray>

class DataManager
{
public:
    // 禁用拷贝构造和赋值运算
    DataManager(const DataManager&) = delete;
    DataManager& operator=(const DataManager&) = delete;
    static DataManager* getInstance();

    QByteArray username() const;
    void setUsername(const QByteArray& newUsername);

    QByteArray ipaddr() const;
    void setIpaddr(const QByteArray& newIpaddr);

    QByteArray port() const;
    void setPort(const QByteArray& newPort);

    Communication* communication() const;
    void setCommunication(Communication* newCommunication);

private:
    DataManager() = default;
    static DataManager* m_data;

    QByteArray m_username;
    QByteArray m_ipaddr = "192.168.2.129";
    QByteArray m_port = "10000";
    Communication* m_communication = nullptr;
};
#endif // DATAMANAGER_H

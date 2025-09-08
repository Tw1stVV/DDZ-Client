#include "datamanager.h"

DataManager* DataManager::m_data = new DataManager();

DataManager* DataManager::getInstance()
{
    return m_data;
}

QByteArray DataManager::username() const
{
    return m_username;
}

void DataManager::setUsername(const QByteArray &newUsername)
{
    m_username = newUsername;
}

QByteArray DataManager::ipaddr() const
{
    return m_ipaddr;
}

void DataManager::setIpaddr(const QByteArray &newIpaddr)
{
    m_ipaddr = newIpaddr;
}

QByteArray DataManager::port() const
{
    return m_port;
}

void DataManager::setPort(const QByteArray &newPort)
{
    m_port = newPort;
}

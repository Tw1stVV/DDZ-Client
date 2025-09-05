#include "musiclist.h"
#include "qurl.h"

MusicList::MusicList(QStringList list, QObject* parent) : QObject{parent}
{
    currentIndex = 0;
    m_playMode = Once;
    m_player = nullptr;
    for (int i = 0; i < list.size(); ++i)
    {
        QUrl url(list.at(i));
        m_list.push_back(url);
    }
    // 设置播放源
    // m_player->setSource(m_list[currentIndex]);
}

MusicList::MusicList(QObject* parent) : QObject{parent}
{
    currentIndex = 0;
    m_playMode = Once;
    m_player = nullptr;
}

void MusicList::setPlayMode(PlayMode mode)
{
    m_playMode = mode;
}

void MusicList::setCurrentIndex(int newCurrentIndex)
{
    currentIndex = newCurrentIndex;
    // 设置播放源
    m_player->setSource(m_list[currentIndex]);
}

int MusicList::getCurrentIndex() const
{
    return currentIndex;
}

void MusicList::handleEndOfMedia(QMediaPlayer::MediaStatus status)
{
    if (status == QMediaPlayer::EndOfMedia)
    {
        if (m_playMode == Loop)
        {
            // 当前列表循环播放
            //  currentIndex = currentIndex;
            currentIndex = (currentIndex + 1) % m_list.size();
            m_player->setSource(m_list[currentIndex]);
            m_player->play();
        }
        else if (m_playMode == Once)
        {
            // 当前曲目播放一次
            m_player->stop();
        }
        else
        {
            m_player->play();
        }
    }
}

QMediaPlayer* MusicList::player() const
{
    return m_player;
}

void MusicList::setPlayer(QMediaPlayer* newPlayer)
{
    m_player = newPlayer;
}

void MusicList::setList(QStringList list)
{
    for (int i = 0; i < list.size(); ++i)
    {
        QUrl url(list.at(i));
        m_list.push_back(url);
    }
    // 设置播放源
    m_player->setSource(m_list[currentIndex]);
}

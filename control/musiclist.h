#ifndef MUSICLIST_H
#define MUSICLIST_H

#include <QObject>
#include <QMediaPlayer>

class QMediaPlayer;
class MusicList : public QObject
{
    Q_OBJECT
public:
    enum PlayMode
    {
        Once,      // 当前曲目播放一次
        Loop,      // 列表循环播放
        CurInLoop, // 单曲循环
    };
    explicit MusicList(QStringList list, QObject* parent = nullptr);
    explicit MusicList(QObject* parent = nullptr);
    void setPlayMode(PlayMode mode);

    void setCurrentIndex(int newCurrentIndex);

    int getCurrentIndex() const;

    QMediaPlayer* player() const;
    void setPlayer(QMediaPlayer* newPlayer);

    void setList(QStringList list);

signals:

public slots:
    void handleEndOfMedia(QMediaPlayer::MediaStatus status);

private:
    int currentIndex = 0;
    QMediaPlayer* m_player;
    PlayMode m_playMode;
    QList<QUrl> m_list;
};

#endif // MUSICLIST_H

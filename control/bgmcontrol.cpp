#include "bgmcontrol.h"
#include "musiclist.h"
#include "playhand.h"

#include <QAudioOutput>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QRandomGenerator>
#include <QTimer>
BGMControl::BGMControl(QObject* parent) : QObject{parent}
{
    for (int i = 0; i < 5; ++i)
    {
        QMediaPlayer* player = new QMediaPlayer(this);
        MusicList* list = new MusicList(this);
        // 设置默认输出设备
        player->setAudioOutput(new QAudioOutput(this));
        player->audioOutput()->setVolume(100);
        // 播放列表绑定播放器
        list->setPlayer(player);
        if (i == 2)
        {
            // 2是bgm循环播放其他是各种音效
            list->setPlayMode(MusicList::Loop);
        }
        else
        {
            list->setPlayMode(MusicList::Once);
        }
        m_list.push_back(list);
        m_players.push_back(player);
        connect(player, &QMediaPlayer::mediaStatusChanged, list, &MusicList::handleEndOfMedia);
    }
    initList();
}

void BGMControl::initList()
{
    QStringList list;
    list << "Man" << "Woman" << "BGM" << "Ending" << "Other";
    // 读取配置json配置文件
    QFile file(":/conf/playList.json");
    file.open(QFile::ReadOnly);
    QByteArray json = file.readAll();
    file.close();

    // 解析数据
    QJsonDocument doc = QJsonDocument::fromJson(json);
    QJsonObject obj = doc.object();

    for (int i = 0; i < list.size(); ++i)
    {
        QString prefix = list.at(i);
        QJsonArray arr = obj.value(prefix).toArray();
        QStringList urls;
        for (int j = 0; j < arr.size(); ++j)
        {
            urls.push_back(arr.at(j).toString());
        }
        m_list[i]->setList(urls);
    }
}

void BGMControl::startBGM(int volume)
{
    m_list[BGM]->setCurrentIndex(0);
    m_list[BGM]->player()->audioOutput()->setVolume(volume);
    m_list[BGM]->player()->play();
}

void BGMControl::stopBGM()
{
    m_list[BGM]->player()->stop();
}

void BGMControl::playerRobLordMusic(int point, Player::Sex sex, bool isFirst)
{
    MediaType index = sex == Player::Man ? BGMControl::Man : BGMControl::Woman;
    // 叫地主
    if (point > 0 && isFirst)
    {
        m_list[index]->setCurrentIndex(Order);
    }
    else if (point == 0)
    {
        // 不抢地主or不叫地主
        if (isFirst)
        {
            m_list[index]->setCurrentIndex(NoOrder);
        }
        else
        {
            m_list[index]->setCurrentIndex(NoRob);
        }
    }
    else if (point == 2)
    {
        m_list[index]->setCurrentIndex(Rob1);
    }
    else if (point == 3)
    {
        m_list[index]->setCurrentIndex(Rob2);
    }
    m_players[index]->play();
}

void BGMControl::playCardsMusic(Cards cards, bool isFirst, Player::Sex sex)
{
    MediaType index = sex == Player::Man ? BGMControl::Man : BGMControl::Woman;
    MusicList* list = m_list[index];
    Card::CardPoint pt = Card::Card_Begin;

    // 判断牌型
    PlayHand hand(cards);
    PlayHand::HandType type = hand.type();
    if (type == PlayHand::Hand_Single || type == PlayHand::Hand_Pair
        || type == PlayHand::Hand_Triple)
    {
        pt = cards.takeRandCard().getPoint();
    }

    int number = 0;
    switch (type)
    {
        case PlayHand::Hand_Single:
            number = pt - 1;
            break;
        case PlayHand::Hand_Pair:
            number = pt - 1 + 15;
            break;
        case PlayHand::Hand_Triple:
            number = pt - 1 + 15 + 13;
            break;
        case PlayHand::Hand_Triple_Single:
            number = ThreeBindOne;
            break;
        case PlayHand::Hand_Triple_Pair:
            number = ThreeBindPair;
            break;
        case PlayHand::Hand_Plane_Two_Single:
        case PlayHand::Hand_Plane_Two_pair:
        case PlayHand::Hand_Plane:
            number = Plane;
            break;
        case PlayHand::Hand_Seq_Pair:
            number = SequencePair;
            break;
        case PlayHand::Hand_Seq_Single:
            number = Sequence;
            break;
        case PlayHand::Hand_Bomb:
            number = Bomb;
            break;
        case PlayHand::Hand_Bomb_Jokers:
            number = JokerBomb;
            break;
        case PlayHand::Hand_Bomb_Single:        // 没有对应的音频文件
        case PlayHand::Hand_Bomb_Pair:          // 没有对应的音频文件
        case PlayHand::Hand_Bomb_Jokers_Single: // 没有对应的音频文件
        case PlayHand::Hand_Bomb_Jokers_Pair:   // 没有对应的音频文件
            number = FourBindTwo;
            break;
        default:
            break;
    }

    // 第一次出牌播放牌型音乐，否则播放“大你”，“管住”
    if (!isFirst && (number >= Plane) && (number <= FourBindTwo))
    {
        list->setCurrentIndex(MoreBiger1 + QRandomGenerator::global()->bounded(2));
    }
    else
    {
        list->setCurrentIndex(number);
    }
    list->player()->play();

    // 播放炸弹和飞机的音效
    if (type == PlayHand::Hand_Bomb || type == PlayHand::Hand_Bomb_Jokers)
    {
        playAssistMusic(BombVoice);
    }
    else if (
        type == PlayHand::Hand_Plane || type == PlayHand::Hand_Plane_Two_Single
        || type == PlayHand::Hand_Plane_Two_pair)
    {
        playAssistMusic(PlaneVoice);
    }
}

void BGMControl::playLastMusic(CardType type, Player::Sex sex)
{
    MediaType index = sex == Player::Man ? BGMControl::Man : BGMControl::Woman;
    MusicList* list = m_list[index];
    // 播放其正在播放其他音效的话等1.5秒在播放
    if (list->player()->playbackState() == QMediaPlayer::StoppedState)
    {
        list->setCurrentIndex(type);
        list->player()->play();
    }
    else
    {
        QTimer::singleShot(
            1500,
            this,
            [=]()
            {
                list->setCurrentIndex(type);
                list->player()->play();
            });
    }
}

void BGMControl::playPassMusic(Player::Sex sex)
{
    MediaType index = sex == Player::Man ? BGMControl::Man : BGMControl::Woman;
    MusicList* list = m_list[index];
    list->setCurrentIndex(Pass1 + QRandomGenerator::global()->bounded(4));
    list->player()->play();
}

void BGMControl::playAssistMusic(AssistMusic type)
{
    MusicList* list = m_list[Other];
    if (type == Dispatch)
    {
        list->setPlayMode(MusicList::CurInLoop);
    }
    else
    {
        list->setPlayMode(MusicList::Once);
    }

    list->setCurrentIndex(type);
    list->player()->play();
}

void BGMControl::stopAssistMusic()
{
    m_list[Other]->player()->stop();
}

void BGMControl::playEndingMusic(bool isWin)
{
    if (isWin)
    {
        m_list[Ending]->setCurrentIndex(0);
    }
    else
    {
        m_list[Ending]->setCurrentIndex(1);
    }
    m_list[Ending]->player()->play();
}

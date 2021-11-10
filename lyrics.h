#ifndef LYRICS_H
#define LYRICS_H

#include <QWidget>
#include <QString>
#include <QMap>
#include <iostream>
#include <QFile>
#include <QDebug>
#include <QRegExp>
#include <QList>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
using namespace std;

class Lyrics
{
public:
    QString lyricsPath;
    QList<QString> listLyricsText; //解析歌词 得到歌词中的文字内容
    QList<int> listLyricsTime;      //得到歌词中的时间

public:
    Lyrics(QString lyricsPath);
    Lyrics();
    bool readLyricsFile(QString lyricsPath);
    bool analysisLyricsFile(QString line);
    QList<QString> getListLyricsText() const;
    QList<int> getListLyricsTime() const;
};

#endif // LYRICS_H


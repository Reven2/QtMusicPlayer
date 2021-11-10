#include "lyrics.h"

//注意QT使用的是 UTF-8编码格格式 一定要注意你的歌词文件是不是UTF-8编码
//可以把歌词文件用记事本打开改变编码在保存


Lyrics::Lyrics()
{

}

Lyrics::Lyrics(QString lyricsPath)
{
    this->lyricsPath = lyricsPath;
}

//获得歌词文字
QList<QString> Lyrics::getListLyricsText()const
{
    return listLyricsText;
}

//获得歌词时间
QList<int> Lyrics::getListLyricsTime() const
{
    return listLyricsTime;
}


bool Lyrics::readLyricsFile(QString lyricsPath)
{
    QFile file(lyricsPath);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        listLyricsText.clear();
        listLyricsTime.clear();
        return false;

        qDebug()<<("readLyricsFile read false");
    }

    QString line="";
    //歌词逐行读入
    while((line = file.readLine())>0)
    {
        //qDebug()<<line;
        analysisLyricsFile(line);
    }
    return true;
}


//提取歌词文本和时间
bool  Lyrics:: analysisLyricsFile(QString line)
{
    if(line == NULL || line.isEmpty())
    {
        qDebug() <<"this line is empty ！";
        return false;
    }

    //使用正则表达式提取 时间 和 内容
    QRegularExpression regularExpression("\\[(\\d+)?:(\\d+)?(\\.\\d+)?\\](.*)?");

    int index = 0;

    //The QRegularExpressionMatch class provides the results
    //of a matching a QRegularExpression against a string.
    QRegularExpressionMatch match;
    match = regularExpression.match(line,index);//匹配目标字符串,匹配起始偏移量
    if(match.hasMatch())
    {
        int totalTime;
        //captured(int nth) 表示获取匹配到的第 nth 个匹配到的字符串段
        //计算该时间点的毫秒数
        totalTime = match.captured(1).toInt() *60000 +match.captured(2).toInt() *1000;

        //获取歌词文本
        QString currentText = QString::fromStdString(match.captured(4).toStdString());

        //QQ音乐歌词lrc文件会显示乱码 改变编码格式
        listLyricsText.push_back(currentText);
        listLyricsTime.push_back(totalTime);

        return true;
    }
    return false;
}












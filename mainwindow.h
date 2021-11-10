#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "playbutton.h"
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QSlider>
#include <QFileDialog>
#include <QListWidget>
#include <QTimer>
#include <QImage>
#include <QPixmap>
#include "lyrics.h"

class QStackedLayout;
class QMenu;


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    QString set_time(int time);


private:
    Ui::MainWindow *ui;


private:
    QMediaPlayer * m_QMediaPlayer;

    PlayButton *m_playbutton;
    PlayButton *next0_btn;
    PlayButton *next1_btn;
    PlayButton *word_btn;       //歌词按钮
    PlayButton *random_btn;
    PlayButton *volume_btn;
    PlayButton *add_btn;


    QStackedLayout *stackedLayout;  //本地 喜欢 收藏
    QStackedLayout *create_stackedLayout; //创建歌单

    QTimer *timer;

    QString m_playPath; //播放文件路径名称 为了加
    int m_playRow ;     //ListWidget item row
    int m_playTime;     //播放时长

    QImage pic ;        //歌曲封面metaDate 中的 ThumbnailImage
    QPixmap pic_show;   //转换后可以直接显示在label的歌曲图片

    Lyrics lyrics;
    int lyricsID;

    //全局变量 *Item ,slot实现需要 but signal&slot para should be the same
    QListWidgetItem *exe_item;    //右键菜单操作充当临时变量


    //右键菜单
    QMenu *set_song;
    QAction *add_like;
    QAction *add_collect;
    QAction *delete_song;

    //更换背景菜单
    QMenu *change_skin;
    QAction *default_skin;
    QAction *personalization;


    void init_sqlite();
    void readFromSQL_MusicList();
    void delete_sql_table();


protected:
    //鼠标进入/离开事件
//    void enterEvent(QEvent *event);
//    void leaveEvent(QEvent *event);

    //要重写的 eventFilter 事件
     bool eventFilter(QObject *watched, QEvent *event);

//    void dragEnterEvent(QDragEnterEvent *event);
//    void dropEvent(QDropEvent *event);

private slots:
    void on_play_clicked();
    void on_volume_clicked();
    void on_add_clicked();
    void on_next0_clicked();
    void on_next1_clicked();
    void on_random_clicked();
    void on_word_clicked();


    void on_song_slider_sliderMoved(int position); //slider control media
    void setSliderValue();                          //media feed back slider
    void on_verticalSlider_sliderMoved(int position);

    void getDuration();             //播放时间/总时间显示
    void setPlayTime();             //显示已经播放时间 和 歌词

    void getMetaDataOk();

    //Right-click menu
    //void on_like_customContextMenuRequested(const QPoint &pos);
    void on_delete_song();
    void on_delete_like();
    void on_delete_collect();

    //change skin menu slot
    void on_default_skin();
    void on_personlization();

    void on_add_like();
    void on_add_collect();
    void on_like_itemDoubleClicked(QListWidgetItem *item);
    void on_collect_itemDoubleClicked(QListWidgetItem *item);
    void on_MusicList_itemDoubleClicked(QListWidgetItem *item);
    void on_about_clicked();
    void on_create_clicked();
};
#endif // MAINWINDOW_H

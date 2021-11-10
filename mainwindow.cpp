#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QPixmap>
#include "playbutton.h"
#include <QVBoxLayout>
#include <QMouseEvent>
#include <QDragEnterEvent>
#include <QDragLeaveEvent>
#include <QMimeData>
#include <QFileDialog>
#include <QFileInfo>
#include <QDebug>
#include <QtWidgets>
#include <QMenu>
#include <QAction>
#include <QCursor>
#include <QMessageBox>
#include <QInputDialog>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
   ,ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //设置窗口固定大小
    setFixedSize(1024,600);
    setWindowTitle("Reven's MusicPlayer~");

    init_sqlite();
    delete_sql_table();


    m_QMediaPlayer = new QMediaPlayer;
    m_QMediaPlayer->setMedia(QUrl::fromLocalFile("F:/KuGouDownload/DAOKO、米津玄師 - 打上花火.mp3"));


    //timer to remind of slider to change
    timer = new QTimer();
    timer ->setInterval(2000);
    timer->start();


    //some buttons
    m_playbutton = new PlayButton(":/image/p1.png");
    next0_btn = new PlayButton(":/image/next0.png");
    next1_btn = new PlayButton(":/image/next1.png");
    word_btn = new PlayButton(":/image/word.png");
    random_btn = new PlayButton(":/image/random.png");
    volume_btn = new PlayButton(":/image/volume.png");
    add_btn = new PlayButton(":/image/add.png");


    //set parent
     next0_btn->setParent(this); //这个变量名起的不好 记住是 上一首 的意思 别被误导了
     next1_btn->setParent(this);
     m_playbutton->setParent(this);
     word_btn->setParent(this);
     random_btn->setParent(this);
     volume_btn->setParent(this);
     add_btn->setParent(this);

     //设置无边框 （透明）
     next0_btn->setFlat(true) ;//这个变量名起的不好 记住是 上一首 的意思 别被误导了
     next1_btn->setFlat(true);
     m_playbutton->setFlat(true);
     word_btn->setFlat(true);
     random_btn->setFlat(true);
     volume_btn->setFlat(true);
     add_btn->setFlat(true);
     ui->create->setFlat(true);

    //按钮布局对齐 我乱摆的
    m_playbutton->move(this->width()*0.5-m_playbutton->width()*0.5,this->height()*0.85);
    next0_btn->move(this->width()*0.5-next0_btn->width()*3,this->height()*0.865);
    next1_btn->move(this->width()*0.5+next0_btn->width()*2,this->height()*0.865);
    word_btn->move(this->width()*0.5+next0_btn->width()*5,this->height()*0.865);
    random_btn->move(this->width()*0.5-next0_btn->width()*5.5,this->height()*0.865);
    volume_btn->move(this->width()*0.5+next0_btn->width()*13,this->height()*0.9);
    add_btn->move(this->width()*0.5+next0_btn->width()*10,this->height()*0.9);

    //volume_btn->toolTip("Volume");


    //安装过滤器 然后记得重写一个 eventfileter 事件 不会写事件处理 不然也不用写这个Fileter 这种NT玩意
    volume_btn->installEventFilter(this);


    //slider layout
    ui->song_slider->move(this->width()*0.5-ui->song_slider->width()*0.5+10,this->height()*0.95);
    ui->verticalSlider->move(this->width()*0.5+ui->song_slider->width()-next0_btn->width()*0.5,this->height()*0.8);
    ui->verticalSlider->hide();


    //右键菜单 这段代码要好好注意 不报错但右键菜单不出来 bug卡了两天至今还不知道为什么 但现在又可以了
    //本地音乐右键菜单
    ui->MusicList->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->MusicList,&QListWidget::customContextMenuRequested,this,[=](const QPoint &pos)
    {
        QListWidgetItem *item = ui->MusicList->itemAt(pos); //获得item所在位置 得到item指针
        if(item == nullptr)
            return;

        exe_item = item;

        set_song = new QMenu(this);
        add_like = new QAction("add to the Like",this);
        add_collect = new QAction("add to  the Collect",this);
        delete_song = new QAction("delete from the List",this);

        set_song->addAction(add_like);
        set_song->addAction(add_collect);
        set_song->addAction(delete_song);

        //以下都是右键菜单对应的 SIGNAL && SLOT
        //注意信号和槽要 参数对应 这里槽无法传入指针 所以改用全局变量的办法解决
        connect(delete_song,SIGNAL(triggered()),this,SLOT(on_delete_song()));
        connect(add_like,SIGNAL(triggered()),this,SLOT(on_add_like()));
        connect(add_collect,SIGNAL(triggered()),this,SLOT(on_add_collect()));

        set_song->exec(QCursor::pos()); //鼠标位置弹出菜单

        delete set_song;
        delete add_like;
        delete add_collect;
        delete delete_song;
    });


    //我的喜欢 右键菜单
    ui->like->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->like,&QListWidget::customContextMenuRequested,this,[=](const QPoint &pos)
    {
        QListWidgetItem *item = ui->like->itemAt(pos); //获得item所在位置 得到item指针
        if(item == nullptr)
            return;

        exe_item = item;

        set_song = new QMenu(this);
        delete_song = new QAction("remove from the List",this);
        add_collect =new QAction("add to the Collect");

        set_song->addAction(delete_song);
        set_song->addAction(add_collect);

        //以下都是右键菜单对应的 SIGNAL && SLOT
        //注意信号和槽要 参数对应 这里槽无法传入指针 所以改用全局变量的办法解决
        connect(delete_song,SIGNAL(triggered()),this,SLOT(on_delete_like()));
        connect(add_collect,SIGNAL(triggered()),this,SLOT(on_add_collect()));

        set_song->exec(QCursor::pos()); //鼠标位置弹出菜单

        delete set_song;
        delete delete_song;
    });


    //我的收藏 右键菜单
    ui->collect->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->collect,&QListWidget::customContextMenuRequested,this,[=](const QPoint &pos)
    {
        QListWidgetItem *item = ui->collect->itemAt(pos); //获得item所在位置 得到item指针
        if(item == nullptr)
            return;

        exe_item = item;

        set_song = new QMenu(this);
        delete_song = new QAction("removefrom the List",this);
        add_like    = new QAction("add to the Like");

        set_song->addAction(delete_song);
        set_song->addAction(add_like);


        //以下都是右键菜单对应的 SIGNAL && SLOT
        //注意信号和槽要 参数对应 这里槽无法传入指针 所以改用全局变量的办法解决
        connect(delete_song,SIGNAL(triggered()),this,SLOT(on_delete_collect()));
        connect(add_like,SIGNAL(triggered()),this,SLOT(on_add_like()));

        set_song->exec(QCursor::pos()); //鼠标位置弹出菜单

        delete set_song;
        delete delete_song;
    });



    //更换皮肤 按钮菜单
    change_skin = new QMenu;
    default_skin = new QAction("Default Skin");
    personalization = new QAction("Choose By Self");

    change_skin->addAction(default_skin);
    change_skin->addAction(personalization);

    ui->skin->setMenu(change_skin);
    //去掉菜单提示箭头
    ui->skin->setStyleSheet("QPushButton::menu-indicator{image:none}");
    //菜单项链接槽
    connect(default_skin,SIGNAL(triggered()),this,SLOT(on_default_skin()));
    connect(personalization,SIGNAL(triggered()),this,SLOT(on_personlization()));



    //stacklayout setting
    QListWidgetItem *item = new QListWidgetItem(QIcon(":/image/myCollect.png"),"本地音乐");
    QListWidgetItem *item1 = new QListWidgetItem(QIcon(":/image/myStars.png"),"我的收藏");
    QListWidgetItem *item2 = new QListWidgetItem(QIcon(":/image/myLike .png"),"我的喜欢");
    ui->songLibrary->addItem(item);
    ui->songLibrary->addItem(item1);
    ui->songLibrary->addItem(item2);


    //设置无边框
    ui->songLibrary->setFrameShape(QListWidget::NoFrame);
    ui->collect->setFrameShape(QListWidget::NoFrame);
    ui->MusicList->setFrameShape(QListWidget::NoFrame);
    ui->like->setFrameShape(QListWidget::NoFrame);
    ui->create_list->setFrameShape(QListWidget::NoFrame);

    //设置Item之间的四周间隔
    ui->songLibrary->setSpacing(7);
    ui->MusicList->setSpacing(7);
    ui->like->setSpacing(7);
    ui->collect->setSpacing(7);
    ui->create_list->setSpacing(2);

    stackedLayout = new QStackedLayout;
    stackedLayout->addWidget(ui->MusicList);
    stackedLayout->addWidget(ui->collect);
    stackedLayout->addWidget(ui->like);             //切换我的喜欢歌单
    stackedLayout->addWidget(ui->LyricsWidget);    //切换歌词显示

    connect(ui->songLibrary,SIGNAL(currentRowChanged(int)),stackedLayout,SLOT(setCurrentIndex(int)));


//    QPushButton * p = new QPushButton(this);
//    p->setText("88");
//    //直接click()  不要用QPushButton：：clicked（）！ i DONT FUCK KNOW WHY
//    connect(p,SIGNAL(clicked()),this,SLOT(close()));


    connect(m_playbutton,SIGNAL(clicked()),this,SLOT(on_play_clicked()));  //暂停/开始
    connect(volume_btn,SIGNAL(clicked()),this,SLOT(on_volume_clicked())); //静音
    connect(volume_btn,SIGNAL(clicked()),ui->verticalSlider,SLOT(hide())); //单击隐藏音量条
    connect(add_btn,SIGNAL(clicked()),this,SLOT(on_add_clicked())); //添加文件
    connect(timer,SIGNAL(timeout()),this,SLOT(setSliderValue()));    //remain of slider to change
    connect(next0_btn,SIGNAL(clicked()),this,SLOT(on_next0_clicked()));  //pre
    connect(next1_btn,SIGNAL(clicked()),this,SLOT(on_next1_clicked()));  //next
    connect(random_btn,SIGNAL(clicked()),this,SLOT(on_random_clicked()));
    connect(word_btn,SIGNAL(clicked()),this,SLOT(on_word_clicked()));   //show lyrics widget
    connect(ui->about,SIGNAL(clicked()),this,SLOT(on_about_clicked()));

    connect(m_QMediaPlayer,SIGNAL(durationChanged(qint64)),this,SLOT(getDuration()));
    connect(m_QMediaPlayer,SIGNAL(positionChanged(qint64)),this,SLOT(setPlayTime()));

    readFromSQL_MusicList();

}


MainWindow::~MainWindow()
{
    delete ui;
}


QString MainWindow::set_time(int time)
{
    int min ,sec;
    time /= 1000;  //通过接口获得的时间是毫秒单位
    min = time/60;
    sec = time - min*60;
    return QString("%1:%2").arg(min).arg(sec);
}

void MainWindow::getDuration()
{
    m_playTime = m_QMediaPlayer->duration();
    ui->playTime->setText(set_time(m_playTime));
}

void MainWindow::setPlayTime()
{
    ui->musicTime->setText(set_time(m_QMediaPlayer->position()));

    //当播放时间改变 判断当前音乐的播放时间是否大于歌词时间 若大于则显示歌词
    if(!lyrics.getListLyricsTime().empty() && m_QMediaPlayer->position()>=lyrics.getListLyricsTime().at(lyricsID))
    {
        //这里一定要注意是否越界 当歌词快没的时候 .at（lyricsID + num） 很有可能越界 需要做判断
        //越界后果直接导致程序crash
        if(lyrics.listLyricsText.size() -lyricsID > 7)
        {
            ui->U3->setText(lyrics.getListLyricsText().at(lyricsID));
            ui->U2->setText(lyrics.getListLyricsText().at(lyricsID+1));
            ui->U1->setText(lyrics.getListLyricsText().at(lyricsID+2));
            ui->U0->setText(lyrics.getListLyricsText().at(lyricsID+3));
            ui->D1->setText(lyrics.getListLyricsText().at(lyricsID+4));
            ui->D2->setText(lyrics.getListLyricsText().at(lyricsID+5));
            ui->D3->setText(lyrics.getListLyricsText().at(lyricsID+6));
            //需要自增1 显示下一句歌词
            lyricsID++;
        }
        else return ;
    }
}


//https://www.cnblogs.com/xia-weiwen/p/6806709.html 参考
void MainWindow::init_sqlite()
{
    QSqlDatabase database;
    if (QSqlDatabase::contains("qt_sql_default_connection"))
    {
        //这是Qt默认连接名称 可以改的
        database = QSqlDatabase::database("qt_sql_default_connection");
        qDebug() << "connect OK" ;
    }
    else
    {
        database = QSqlDatabase::addDatabase("QSQLITE"); //”QSQLITE“ 是驱动名无法更改
        database.setDatabaseName("Music.db");
        database.setUserName("Reven");
        database.setPassword("123456");

        if (!database.open())
        {
            qDebug() << "Error: Failed to connect database." << database.lastError();
            exit(-1);
        }
    }


    QSqlQuery query;
    //QString create_sql = "create table music ( name varchar(100))";
    //query.prepare(create_sql);

    //创建一次数据表 就已经存在了！ 再次运行创建会报错 QSqlError("", "Unable to fetch row", "No query")
    if( !query.exec("create table music (name varchar(100), primary key(name) ) "))
    {
        QMessageBox::warning(NULL, "Create Table",
                             tr("Create Table error:%1").arg(query.lastError().text()));
    }

    else

        qDebug() <<"Table created!";
}


void MainWindow::delete_sql_table()
{
    QSqlQuery query;
    QString drop_table = "drop table music";
    query.prepare(drop_table);
    if(!query.exec(drop_table))
    {
        qDebug()<<"Drop table fail";
    }
}


void MainWindow::readFromSQL_MusicList()
{
    QSqlQuery query;
    QString select_sql = "select name from music";
    query.prepare(select_sql);
    if(!query.exec(select_sql))
    {
        qDebug()<<query.lastError();
    }
    else
    {
        while(query.next())
        {
            QString item_name = query.value(0).toString();
            QListWidgetItem *item = new QListWidgetItem(QIcon(":/image/note.png"),item_name);
            ui->MusicList->addItem(item);

           // qDebug() << "read from database " << item->text();
        }
        m_playPath = "D:/Songs";
    }
}


//用户交互的一些 buttons SLOT
void MainWindow::on_play_clicked()
{
    if(QMediaPlayer::PlayingState==m_QMediaPlayer->state())
    {
        m_QMediaPlayer->pause();
        m_playbutton->ChangeImage(":/image/p1.png");

    }
    else{
        m_QMediaPlayer->play();
        m_playbutton->ChangeImage(":/image/p2.png");

    }
}

void MainWindow::on_volume_clicked()
{
    if(m_QMediaPlayer->volume()!=0)
    {
        m_QMediaPlayer->setVolume(0);
        volume_btn->ChangeImage(":/image/mute.png");
    }
    else
    {
        m_QMediaPlayer->setVolume(50);
        volume_btn->ChangeImage(":/image/volume.png");
    }
}

void MainWindow::on_add_clicked()
{
    //这个实现方法可以直接把选择的目录下的所有MP3文件全部加入（不太符合现实音乐播放器用户操作）

//    //选择一个已有的目录
//     QString fileName = QFileDialog::getExistingDirectory(NULL,"Selct Music Dir",".",NULL);

//    //针对当前目录或者目录下的文件进行操作
//    QDir dir(fileName);
//    if(!dir.exists())
//        return;

//    QStringList nameFilters;
//    nameFilters<<"*.mp3";

//    //Returns a list of the names of all the files and directories in the directory
//    QStringList files = dir.entryList(nameFilters, QDir::Files|QDir::Readable, QDir::Name);
//    ui->MusicList->addItems(files);
//    m_playPath = fileName;

    //可以手动选择文件加入（比较推荐）
     QStringList fileName_list;
     QFileInfo fileInfo;
     QString fileFull ;
     QString fileName;
     QStringList fileFull_list = QFileDialog::getOpenFileNames(NULL,"Selct Music File",".",NULL);
     for(int i=0;i<fileFull_list.size();i++)
     {
         fileFull = fileFull_list[i];       //单个文件路径+文件名
         fileInfo = QFileInfo(fileFull);
         fileName = fileInfo.fileName();
         //QString name = item->text().left(item->text().lastIndexOf("."));
         //除去文件后缀名.mp3显示 美观一点
         //注意这里去掉了后缀 在播放时拼接路径的时候需要加上 ".mp3"
         fileName = fileName.left(fileName.lastIndexOf("."));
         fileName_list.append(fileName);
     }
        //QListWidgetItem *item = new QListWidgetItem(QIcon(":/image/myCollect.png"),"本地音乐");

        //设置给每个 item 都加上 Icon 再 additems ，因为additems 接口没有 QIcon 类型参数
        for(int i=0;i<fileName_list.size();i++)
        {
            QListWidgetItem *item = new QListWidgetItem(QIcon(":/image/note.png"),fileName_list[i]);
            ui->MusicList->addItem(item);
        }
        // ui->MusicList->addItems(fileName_list);  //上面 升级成加icon版本了 所以不用了 注释掉
         m_playPath =  fileInfo.absolutePath();  //播放需要这个文件的路径（不包括文件名）
         qDebug() <<m_playPath;


         //存入数据库
         for(int i=0;i<fileName_list.size();i++)
         {
             QSqlQuery sql_query ;
             QString insert_sql = "insert into music values(?)";
             sql_query.prepare(insert_sql);
             sql_query.addBindValue(fileName_list[i]);
             if(!sql_query.exec())
             {
                 qDebug()<<sql_query.lastError();
             }

             qDebug()<<"inseted "<< fileName_list[i];
         }

}

void MainWindow::on_next0_clicked()
{
    if(m_playRow == 0)
    {
        m_playRow = ui->MusicList->count() -1;  //倒数第一首歌
    }
    else{
        m_playRow--;  //上一首
    }

    QListWidgetItem *item = ui->MusicList->item(m_playRow);
    item->setSelected(true);    //设置这个item为选定状态
    m_QMediaPlayer->stop();
    m_QMediaPlayer->setMedia(QUrl::fromLocalFile(m_playPath+"/"+item->text()+".mp3"));
    m_QMediaPlayer->play();

}

void MainWindow::on_next1_clicked()
{
    if(m_playRow + 1 == ui->MusicList->count())
    {
        m_playRow = 0;     //已经到最后一首 放第一首
    }
    else
    {
        m_playRow++;
    }

    QListWidgetItem *item = ui->MusicList->item(m_playRow);
    item->setSelected(true);
    m_QMediaPlayer->stop();
    m_QMediaPlayer->setMedia(QUrl::fromLocalFile(m_playPath+"/"+item->text()+".mp3"));

    qDebug() <<m_playPath<<"/"<<item->text();
    m_QMediaPlayer->play();

}

void MainWindow::on_random_clicked()
{
    if(random_btn->normalImagePath!=":/image/order.png")
         random_btn->ChangeImage(":/image/order.png");
    else
        random_btn->ChangeImage(":/image/random.png");
}

void MainWindow::on_word_clicked()
{
    //注意index is the order of called addwidget() to add in the stacklayout
    stackedLayout->setCurrentIndex(3);  // lyrics widget 's index is 3
}

void MainWindow::on_about_clicked()
{
    QMessageBox::about(this,u8"About This Music Player Writed By Reven",
                       "<p>这是一款简陋的音乐播放器<p>"
                        "<p>    其源代码极其糟糕     变量名的不规范       内存泄露风险    擅用全局变量  "
                        "   没有设计模式        代码冗余      维护性极差   <p>");
}


//鼠标双击显示音量条事件 他妈的 刚刚看到可以用 if isVisiable setVisiable（false）
bool MainWindow::eventFilter(QObject *watched, QEvent *event)
{
    //判断想要观察的控件
    if(watched == volume_btn)
    {
        //是想要处理的事件类型
        if(event->type() == QEvent::MouseButtonDblClick)
        {
           // QMouseEvent *ev = static_cast<QMouseEvent *>(event);
            ui->verticalSlider->show();
            return true;
        }
    }
    //其他默认处理
    return QWidget::eventFilter(watched,event);

}

void MainWindow::on_song_slider_sliderMoved(int position)
{
    m_QMediaPlayer->setPosition(m_QMediaPlayer->duration()*position/100);//设置文件从第多少帧播放
}

void MainWindow::setSliderValue()
{
    // 当前播放进度时间/总持续时间 = slider 此时到达的位置
    ui->song_slider->setValue(m_QMediaPlayer->position()*100/m_QMediaPlayer->duration());
}

void MainWindow::on_verticalSlider_sliderMoved(int position)
{
    m_QMediaPlayer->setVolume(position);
}


//关于右键删除菜单选项槽
void MainWindow::on_delete_song()
{
    int del_row = ui->MusicList->row(exe_item);  //获取鼠标所指item
    qDebug() << del_row ;
    ui->MusicList->takeItem(del_row);
    update();
}

void MainWindow::on_delete_like()
{
    int del_row = ui->like->row(exe_item);  //获取鼠标所指item
    qDebug() << del_row ;
    ui->like->takeItem(del_row);        //删除
    update();
}

void MainWindow::on_delete_collect()
{
    int del_row = ui->collect->row(exe_item);  //获取鼠标所指item
    qDebug() << del_row ;
    ui->collect->takeItem(del_row);
    update();

}


//本地选择一首音乐添加到 我的喜欢
void MainWindow:: on_add_like()
{
    // ui->like->addItem(exe_item);     //不明白为什么这行代码不生效
    ui->like->addItem(exe_item->text());  //没有 Icon 版本的 item
}

//本地选择一首添加到 我的收藏
void MainWindow:: on_add_collect()
{
    ui->collect->addItem(exe_item->text());
}


//dont press mouse for three times！process will be ended forcefully!
void MainWindow::on_MusicList_itemDoubleClicked(QListWidgetItem *item)
{
    m_playRow = ui->MusicList->row(item); //返回给定项目的行

    m_QMediaPlayer->stop();

    //拼接路径文件名用来播放
    //Note: This function returns immediately after recording the specified source of the media.
    //It does not wait for the media to finish loading and does not check for errors. FUCK
    //提取过程是异步的，你必须等待metaDataChanged信号发射后再来取 FUCK FUCK FUCK FUCK
    // 不要放过文档上写的每一句话 他妈的 这么重要的事情 居然放在文档那么大一段中间说！
    m_QMediaPlayer->setMedia(QUrl::fromLocalFile(m_playPath+"/"+item->text()+".mp3"));
    qDebug()<<m_playPath<<"/"<<item->text()<<".mp3";

    m_QMediaPlayer->play();

    //QMediaPlayer load metaData is too slow!
    connect(m_QMediaPlayer,SIGNAL(metaDataChanged()),this,SLOT(getMetaDataOk()));

    //传进来的 Item.text()将得到项上面的文本 注意歌词文件和歌曲文件要同名
    QString name = item->text().left(item->text().lastIndexOf(".")); //除去文件后缀名字
    qDebug()<<m_playPath<<"/"<<name<<".lrc";
        //读取歌词文件
        if(!lyrics.readLyricsFile(m_playPath+"/"+name+".lrc")){
            ui->U0->setText("未检测到歌词文件,请检查歌词文件是否存在");
        }

        qDebug() <<"Read Lyrics OK";
        lyricsID = 0;

}


//aaa !!!!! FUCK FUCK FUCK !!!! I GOT THSI SHIT!!! WHAT A SHIT BUG !!! I GOT IT  !!!
//I AM CODE GOD
void MainWindow:: getMetaDataOk()
{
    //拼接正在播放歌曲信息
    QString info="";
    QString author = m_QMediaPlayer->metaData(QStringLiteral("Author")).toStringList().join(",");
    info.append(author);
    QString title = m_QMediaPlayer->metaData(QStringLiteral("Title")).toString();
    //QString albumTitle = player->metaData(QStringLiteral("AlbumTitle")).toString();
    info.append(" - "+title);
    ui->name->setText(info);

    //获取正在播放歌曲图片
    pic = m_QMediaPlayer->metaData("ThumbnailImage").value<QImage>();
    if(pic.isNull())
    {
        pic = QImage(":/image/pic.png");
        qDebug() << "pic is null";
    }
    pic_show = QPixmap::fromImage(pic);
    ui->picLabel->setPixmap(pic_show);
}


//使用 go to slot 则不用手动 connect 啦！
void MainWindow::on_like_itemDoubleClicked(QListWidgetItem *item)
{
    m_playRow = ui->MusicList->row(item); //返回给定项目的行

    m_QMediaPlayer->stop();
    //拼接路径文件名用来播放
    m_QMediaPlayer->setMedia(QUrl::fromLocalFile(m_playPath+"/"+item->text()+".mp3"));
    m_QMediaPlayer->play();

    //元数据已经异步载入完成时 会触发这个 SIGNAL - SLOT
    connect(m_QMediaPlayer,SIGNAL(metaDataChanged()),this,SLOT(getMetaDataOk()));

    //传进来的 Item.text()将得到项上面的文本 注意歌词文件和歌曲文件要同名
    QString name = item->text().left(item->text().lastIndexOf(".")); //除去文件后缀名字
    qDebug()<<m_playPath<<"/"<<name<<".lrc";
        //读取歌词文件
        if(!lyrics.readLyricsFile(m_playPath+"/"+name+".lrc")){
            ui->U0->setText("未检测到歌词文件,请检查歌词文件是否存在");
        }

        qDebug() <<"Read Lyrics OK";
        lyricsID = 0;

}


void MainWindow::on_collect_itemDoubleClicked(QListWidgetItem *item)
{
    m_playRow = ui->MusicList->row(item); //返回给定项目的行

    m_QMediaPlayer->stop();
    //拼接路径文件名用来播放
    m_QMediaPlayer->setMedia(QUrl::fromLocalFile(m_playPath+"/"+item->text()+".mp3"));
    m_QMediaPlayer->play();

    connect(m_QMediaPlayer,SIGNAL(metaDataChanged()),this,SLOT(getMetaDataOk()));

    //传进来的 Item.text()将得到项上面的文本 注意歌词文件和歌曲文件要同名
    QString name = item->text().left(item->text().lastIndexOf(".")); //除去文件后缀名字
    qDebug()<<m_playPath<<"/"<<name<<".lrc";
        //读取歌词文件
        if(!lyrics.readLyricsFile(m_playPath+"/"+name+".lrc")){
            ui->U0->setText("未检测到歌词文件,请检查歌词文件是否存在");
        }

        qDebug() <<"Read Lyrics OK";
        lyricsID = 0;
}


void MainWindow::on_default_skin()
{
    QString fileName=":/image/wallPaper.jpg";
    //生成一个保存这个设置的文件
    QSettings mysettings("./MusicPlayerSaving.ini",QSettings::IniFormat);
    mysettings.setIniCodec("UTF8");
    //para is a key-value
    mysettings.setValue("background/image-url",fileName);
    setStyleSheet(QString("QWidget#MainWindow{"
                          "border-radius:10px;"
                          "border-image: url(%1);}").arg(fileName));

}


void MainWindow::on_personlization()
{
    //从默认图片位置打开文件选择框
    QString fileName=QFileDialog::getOpenFileName(this,("选择自定义背景图片"),QStandardPaths::standardLocations(QStandardPaths::PicturesLocation).first(),u8"图片文件(*jpg *png)");
    if(!fileName.isEmpty())
    {
        QImage testImage(fileName);
        if(!testImage.isNull()){
            QSettings mysettings("./MusicPlayerSaving.ini",QSettings::IniFormat);
            mysettings.setIniCodec("UTF8");
            mysettings.setValue("background/image-url",fileName);
            setStyleSheet(QString("QWidget#MainWindow{"
                                  "border-radius:10px;"
                                  "border-image: url(%1);}").arg(fileName));
        }
    }
}



//创建歌单slot ,go to slot不用手动 connect（）
void MainWindow::on_create_clicked()
{
    bool Isok;
    QString listName = QInputDialog::getText(this,
                                             "Create a List",
                                             "Enter the name of the list that will create",
                                             QLineEdit::Normal,
                                             "Untitled",
                                             &Isok);

    if(Isok){

        QListWidgetItem *item = new QListWidgetItem(QIcon(":/image/list.png"),listName);
        ui->create_list->addItem(item);


    }
}


#include "playbutton.h"
#include <QPushButton>
#include <QDebug>
#include <QPixmap>

PlayButton::PlayButton(QString normalImage)
{
    this->normalImagePath = normalImage;
    //this->pressImagePath = pressImage;

    QPixmap pix;
    bool ret = pix.load(normalImage);
    if(!ret)
    {
        qDebug()<<"image load error";
        return;
    }

    //设置图片固定大小
    this->setFixedSize(pix.width(),pix.height());

    //设置不规则图片样式 fuck Why set this then my button be doesnt work??
   // this->setStyleSheet("QPushButton{border:0px;}");

    //设置图标 图标大小 have to do , or display a fucking pic in button
    this->setIcon(pix);
    this->setIconSize(QSize(pix.width(),pix.height()));
}

//not programing , will not use this tricks anymore
void PlayButton::ChangeImage(QString imge)
{
        this->normalImagePath = imge;
        //this->pressImagePath = pressImage;

        QPixmap pix;
        bool ret = pix.load(imge);
        if(!ret)
        {
            qDebug()<<"image load error";
            return;
        }

        //设置图片固定大小
        this->setFixedSize(pix.width(),pix.height());


        this->setStyleSheet("QPushButton{border:0px;}");

        //设置图标 图标大小
        this->setIcon(pix);
        this->setIconSize(QSize(pix.width(),pix.height()));
}




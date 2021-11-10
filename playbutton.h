#ifndef PLAYBUTTON_H
#define PLAYBUTTON_H

#include <QWidget>
#include <QPushButton>


class PlayButton : public QPushButton
{
    Q_OBJECT
public:
    //explicit PlayButton(QWidget *parent = nullptr);
   PlayButton(QString normalImage= "");
   void ChangeImage(QString imge);


//成员属性
    QString normalImagePath;


protected:
//    void enterEvent(QEvent *event);
//    void leaveEvent(QEvent *event);


signals:


};

#endif // PLAYBUTTON_H

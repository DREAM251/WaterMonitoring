/*!
\class KeyBoard
\version 1.0.0
\author Veelang

\note
软键盘图像界面调用，处理
*/

#include "keyboard.h"
#include <QIcon>
#include <QDebug>
#include <QSignalMapper>

/*!
\fn KeyBoard::KeyBoard( QWidget *parent)
Default constructor.
*/
KeyBoard::KeyBoard( QWidget *parent):
    QWidget(parent, Qt::Tool|Qt::FramelessWindowHint/*|Qt::WindowStaysOnTopHint*/),
    lastFocusedWidget(0)
{
    setupUi(this);
    setWindowOpacity(0.8);
    btns=findChildren<QPushButton *>();

    connect(qApp, SIGNAL(focusChanged(QWidget*,QWidget*)),
            this, SLOT(saveFocusWidget(QWidget*,QWidget*)));

    myMapping();

    should_move=false;

}


KeyBoard::~KeyBoard()
{

}


/*!
\fn void KeyBoard::myMapping()
建立键值与响应槽的对应关系
*/
void KeyBoard::myMapping()
{
    QSignalMapper *myMapper=new QSignalMapper(this);
    for(int i=0;i<btns.size();i++)
    {
        myMapper->setMapping(btns[i],btns[i]);
        connect(btns[i],SIGNAL(clicked()),myMapper,SLOT(map()));
    }
    //qDebug()<<"*****";
    connect(myMapper,SIGNAL(mapped(QWidget*)),this,SLOT(btnClicked(QWidget*)));
}

/*!
\fn void KeyBoard::btnClicked(QWidget *w)
按键事件处理
*/
void KeyBoard::btnClicked(QWidget *w)
{
     QPushButton *btnp=static_cast<QPushButton *>(w);

     int btn=btnp->objectName().remove("pushButton_").toInt();
     //qDebug()<<btn;

     if(btn<=45)
     {
         QChar ch=btnp->text().at(0);
         //qDebug()<<"ch"<<ch;
         Q_EMIT sendChar(ch);
     }
     else
     {
         Q_EMIT sendInt(btn);
     }
}

/*!
\fn void KeyBoard::saveFocusWidget(QWidget *oldFocus, QWidget *newFocus)
记录新的焦点句柄
*/
void KeyBoard::saveFocusWidget(QWidget *, QWidget *newFocus)
{
    if (newFocus != 0 && !this->isAncestorOf(newFocus))
    {
        lastFocusedWidget = newFocus;
        //qDebug()<<"lastFocusedWidget->objectName()"<<lastFocusedWidget->objectName();
    }
}

/*!
\fn bool KeyBoard::event(QEvent *e)
事件处理
*/
bool KeyBoard::event(QEvent *e)
{
    switch (e->type())
    {
    case QEvent::WindowActivate:
        if (lastFocusedWidget)
            lastFocusedWidget->activateWindow();
        break;
    case QEvent::MouseButtonPress:
        mousePressEvent(e);
        break;
    case QEvent::MouseButtonRelease:
        mouseReleaseEvent(e);
        break;
    case QEvent::MouseMove:
        mouseMoveEvent(e);
        break;
    default:
        break;
    }
    return QWidget::event(e);
}

/*!
\fn void KeyBoard::mousePressEvent (QEvent *e)
鼠标按下事件
*/
void KeyBoard::mousePressEvent (QEvent *e)
{
    QMouseEvent *event=static_cast<QMouseEvent *>(e);
    if (event->button()!=Qt::LeftButton) return;
    this->should_move = true;
    this->widget_pos = this->pos();
    this->mouse_pos = event->globalPos();

}

/*!
\fn void KeyBoard::mouseReleaseEvent(QEvent *e)
鼠标释放事件
*/
void KeyBoard::mouseReleaseEvent(QEvent *e)
{
    QMouseEvent *event=static_cast<QMouseEvent *>(e);
    if (event->button()!=Qt::LeftButton) return;
    this->should_move = false;

}


/*!
\fn void KeyBoard::mouseMoveEvent ( QEvent *e)
鼠标按下移动事件
*/
void KeyBoard::mouseMoveEvent ( QEvent *e)
{
    QMouseEvent *event=static_cast<QMouseEvent *>(e);
    if (this->should_move)
    {
        QPoint pos = event->globalPos();
        int x = pos.x()-this->mouse_pos.x();
        int y = pos.y()-this->mouse_pos.y();
        QWidget::move(this->widget_pos.x()+x,this->widget_pos.y()+y);
    }

}








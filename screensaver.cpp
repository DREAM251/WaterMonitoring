#include "screensaver.h"
#include "ui_screensaver.h"

#if defined (Q_WS_QWS)
#include <QWSServer>
#endif

#define M_SPEED  200 //unit:ms
#define M_UP     (-3)
#define M_DOWN     3
#define M_LEFT     (-3)
#define M_RIGHT     3


ScreenSaver::ScreenSaver(QWidget *parent) :
    QDialog(parent , Qt::WindowStaysOnTopHint),
    ui(new Ui::screensaver)
{
    ui->setupUi(this);

    x = 0;
    y = 0;
    xdir = M_RIGHT;
    ydir = M_DOWN;
    fmWidth = ui->frame->width();
    fmHeight = ui->frame->height();
    getScreenSize();
    timeset = 0;
    timer = new QTimer();
    timeOut = new QTimer();
    connect(timer,SIGNAL(timeout()),this,SLOT(timeup()));
    connect(timeOut,SIGNAL(timeout()),this,SLOT(slot_TimeStart()));
    hideMe();
}

ScreenSaver::~ScreenSaver()
{
    delete ui;
}

void ScreenSaver::timeup()
{
    if(!this->isVisible())
        return;

    x += xdir ,y += ydir;
    if(x<=0)
        xdir = M_RIGHT;
    else if(x+fmWidth>=dlgWidth)
        xdir = M_LEFT;

    if(y<=0)
        ydir = M_DOWN;
    else if(y+fmHeight>=dlgHeight)
        ydir = M_UP;

    ui->frame->move(x,y);

}

void ScreenSaver::hideMe()
{
    if(timeset>0)
        timeOut->start(timeset*60*1000);
    if(timer->isActive())
        timer->stop();
    if(this->isVisible()){
#if defined (Q_WS_QWS)
        QWSServer::setCursorVisible(true);
#endif
        this->hide();
    }
}

void ScreenSaver::setTimeout(int time)
{
    if (timeOut->isActive())
           timeOut->stop();
    timeset = time;
    if(timeset>0)
        timeOut->start(timeset*60*1000);
}

void ScreenSaver::getScreenSize()
{
    dlgWidth = this->width();
    dlgHeight = this->height();
}

void ScreenSaver::slot_TimeStart()
{
    if(timeOut->isActive())
        timeOut->stop();

    timer->start(M_SPEED);
    this->showFullScreen();
    this->setFocus();
#if defined (Q_WS_QWS)
    QWSServer::setCursorVisible(false);
#endif
    getScreenSize();
}

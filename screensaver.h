#ifndef SCREENSAVER_H
#define SCREENSAVER_H

#include <QDialog>
#include <QTimer>
#include <QDebug>

namespace Ui {
class screensaver;
}

class ScreenSaver : public QDialog
{
    Q_OBJECT
    
public:
    explicit ScreenSaver(QWidget *parent = 0);
    ~ScreenSaver();

private:
    Ui::screensaver *ui;


    QTimer *timer;
    QTimer *timeOut;
    int timeset;
    int x,y;
    int xdir,ydir;

    int fmWidth,fmHeight;
    int dlgWidth,dlgHeight;
    void getScreenSize();

private Q_SLOTS:
    void timeup();
    void slot_TimeStart();

public:
    void hideMe();
    void setTimeout(int time);
};

#endif // SCREENSAVER_H

#ifndef CONCINDEXFRAME_H
#define CONCINDEXFRAME_H

#include <QFrame>
#include "src/externInc.h"
namespace Ui {
class ConcIndexFrame;
}

class ConcIndexFrame : public QFrame
{
    GLOBEL_OBJECT
    Q_OBJECT  
public:
    explicit ConcIndexFrame(QWidget *parent = 0);
    ~ConcIndexFrame();   
    void initgo(MainWindow *ft);
    ConcIndexFrame& operator=(const ConcIndexFrame &src);
//////////////////////////////////////////
    double indexConc;
    int    indexRange;
    int    indexPipe;
    int    indexTimes;
    int    indexSelExec;

    double indexIndicating;
    double indexRepeat;
    double indexStable;
////////////////////////////////////////
    double getConc();
    int    getRange();
    int    getPipe();
    int    getTimes();
    int    getSelExec();

    double getIndicating();
    double getRepeat();
    double getStable();

    bool   setSelExec(int setValue);

    bool   setIndicating(double setValue);
    bool   setRepeat(double setValue);
    bool   setStable(double setValue);
//////////////////////////////////////
    bool renewUi();
    bool saveUi();
private:
    Ui::ConcIndexFrame *ui;
};

#endif // CONCINDEXFRAME_H

#ifndef DRIFTINDEXFRAME_H
#define DRIFTINDEXFRAME_H

#include <QFrame>
#include "src/externInc.h"

namespace Ui {
class DriftIndexFrame;
}

class WINDLL_EXPORT DriftIndexFrame : public QFrame
{
    GLOBEL_OBJECT
    Q_OBJECT
    
public:
    explicit DriftIndexFrame(QWidget *parent = 0);
    ~DriftIndexFrame();

    void initgo(MainWindow *ft);

    double indexConc;           //浓度
    int    indexRange;          //量程
    int    indexPipe;           //管道
    int    indexTimes;          //次数
    int    indexIgnoreTimes;    //忽略次数
    double indexIndex;          //指标
    double indexReferIndex;     //参考指标
    double indexReferIndex1;    //参考指标1
    int    indexSelExec;        //执行
    ////////////////////////////////////////
    double getConc();
    int    getRange();
    QString getStrRange();
    int    getPipe();
    int    getUiTimes();
    int    getTimes();
    int    getUiIgnoreTimes();
    int    getIgnoreTimes();
    int    getSelExec();
    double getIndex();
    double getReferIndex();
    double getReferIndex1();
    bool   setSelExec(int setValue);
    bool   setIndex(double setValue);
    bool   setReferIndex(double setValue);
    bool   setReferIndex1(double setValue);
    bool   setLabelIndexText(QString text);
    bool   setLabelReferIndexText(QString text);
    bool   setLabelReferIndex1Text(QString text);
    //////////////////////////////////////
    bool renewUi();
    bool saveUi();
    bool setLableConcVisible(bool flags);
    bool setLableRangeVisible(bool flags);
    bool setLableReferIndex1Visible(bool flags);
private:
    Ui::DriftIndexFrame *ui;
};

#endif // DRIFTINDEXFRAME_H

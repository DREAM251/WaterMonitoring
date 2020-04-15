#ifndef QFMAIN_H
#define QFMAIN_H

#include <QWidget>
#include <QSignalMapper>
#include <QTimer>
#include <QDateTime>
#include "elementinterface.h"
#include "querydata.h"
#include "calibframe.h"
#include "instructioneditor.h"
#include "modbusmodule.h"

namespace Ui {
class QFMain;
class SetUI;
class Maintaince;
class MeasureMode;
class LightVoltage;
}
class QFMain : public QWidget
{
    Q_OBJECT

public:
    explicit QFMain(QWidget *parent = 0);
    ~QFMain();

    void initSettings();
    void initCalibration();
    void initMaintaince();
    void initQuery();

public slots:
    void menuClicked(int i);
    void updateStatus();
    void login(int level);

    void loadSettings();
    void saveSettings();

    void OnlineOffline();
    void MeasureMethod();
    void Range();
    void SamplePipe();

    void SampleMeasure();
    void ZeroMeasure();
    void StandardMeasure();
    void QCMeasure();

Q_SIGNALS:
    void systemTrigger();
    void userTrigger();

private:
    Ui::QFMain *ui;
    Ui::SetUI *setui;
    Ui::Maintaince *maintaince;
    Ui::MeasureMode *measuremode;
    Ui::LightVoltage *lightVoltage;
    QSignalMapper *signalMapper;
    QTimer *timer;
    ElementInterface *element;
    int loginLevel;

    QueryData *queryData;
    QueryData *queryCalib;
    QueryData *queryError;
    QueryData *queryLog;
    CalibFrameUser *usercalib;
    CalibFrameFactory *factorycalib;
    InstructionEditor *editor;
    ModbusModule *modbusframe;

    QStringList nameOnlineOffline;
    QStringList nameMeasureMethod;
    QStringList nameRange;
    QStringList nameSamplePipe;

};

#endif // QFMAIN_H

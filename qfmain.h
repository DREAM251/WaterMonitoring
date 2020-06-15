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

enum AutoCalibrationType {
    AC_Idle,
    AC_UserCalibration,
    AC_FactoryCalibration
};

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

    void loadMaintaince();

    void OnlineOffline();
    void MeasureMethod();
    void Range();
    void SamplePipe();

    void SampleMeasure();

    void Drain();
    void Stop();
    void Clean();
    void OneStepExec();
    void FuncExec();
    void InitLoad();
    void SaveLigthVoltage();

    void UserCalibration();
    void FactoryCalibration();
    void TaskFinished(int type); // 业务结束
    void TashStop(int type); // 业务终止

Q_SIGNALS:
    void systemTrigger();
    void userTrigger();

private slots:
    void on_pushButton_clicked();

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
    AutoCalibrationType autoCalibrationType;

    QueryData *queryData;
    QueryData *queryCalib;
    QueryData *queryQC;
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
    QString explainString;
    QString explainString2;
    QString explainString3;
};

#endif // QFMAIN_H

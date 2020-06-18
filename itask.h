#ifndef ITASK_H
#define ITASK_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QTimer>
#include <QDateTime>
#include <QVariant>
#include "iprotocol.h"
#include "profile.h"

enum TaskType
{
    TT_Idle = 0,
    TT_Measure,
    TT_ZeroCalibration,
    TT_SampleCalibration,
    TT_ZeroCheck,
    TT_SampleCheck,
    TT_SpikedCheck,
    TT_ErrorProc,

    TT_Stop,
    TT_Clean,
    TT_Drain,
    TT_Initial,
    TT_Debug,
    TT_Initload,
    TT_Func,
    TT_Config,

    TT_END /*end flag, don't use*/
};

enum ErrorFlag
{
    EF_NoError = 0,
    EF_HeatError,
    EF_SamplingError,
    EF_BlankError,
    EF_Opwater
};

class CalibFrame;

class ITask : public QObject
{
    Q_OBJECT

public:
    struct CorrelationArguments
    {
        int timeTab[20];
        int tempTab[20];
        int loopTab[20];
    };

    ITask(QObject *parent = NULL);
    virtual ~ITask(){;}

    virtual bool start(IProtocol *protocol);
    virtual void stop();
    virtual void oneCmdFinishEvent();
    virtual void recvEvent();
    inline TaskType getTaskType() {return taskType;}
    inline bool isWorking(){return workFlag;}
    inline ErrorFlag isError(){return errorFlag;}
    inline int getLastProcessTime() {return processSeconds;}
    inline int getTotalStep() {return commandList.count();}
    inline int getStepnum() {return cmdIndex;}
    int getProcess();
    inline void setTaskType(TaskType type) { taskType = type;}
    inline float getRealTimeValue() {return realTimeConc;}

    virtual void loadParameters();
    virtual void saveParameters();
    virtual QStringList loadCommands(){return QStringList();}
    virtual void fixCommands(const QStringList &sources);

    virtual void sendNextCommand();

public slots:
    void DataReceived();
    void CommandEnd();
    void Timeout();

protected:
    IProtocol *protocol; // shared
    QStringList commandList;
    int cmdIndex;
    QString cmd;

    bool workFlag;
    ErrorFlag errorFlag;
    CorrelationArguments corArgs;

    TaskType taskType;
    QDateTime startTime;
    int processSeconds; // 一次完整测量花费的时间


    int pipe; //取液管道,-1代表默认管道
    float realTimeConc;
};


class MeasureTask : public ITask
{
public:
    struct WorkArguments
    {
        int range;
        bool rangeLock;
        int pipe;
        int mode;

        float lineark;
        float linearb;
        float quada;
        float quadb;
        float quadc;

        float userk;
        float userb;

        float smoothRange;
        float turbidityOffset;

        int blankErrorValue;
    };

    MeasureTask();

    // arguments:
    // 0 : range         int
    // 1 : rangeLock     bool
    // 2 : ...
    virtual bool start(IProtocol *protocol);

    void clearCollectedValues();
    virtual bool collectBlankValues();
    virtual bool collectColorValues();

    virtual void dataProcess();
    virtual float realTimeDataProcess(int blankValue,
                                      int colorValue,
                                      int blankValueC2,
                                      int colorValueC2);
    void recvEvent();
    virtual void loadParameters();
    virtual void saveParameters();
    virtual QStringList loadCommands();

    void testDataProcess();

protected:
    int blankSampleTimes;
    int colorSampleTimes;
    int blankValue;
    int blankValueC2;
    int colorValue;
    int colorValueC2;
    int measureway;
    WorkArguments args;

    double vabs;
    double conc;

    int lastBlankValue;
    int lastBlankValueC2;
};


class CalibrationTask : public MeasureTask
{
public:
    CalibrationTask();

    void loadParameters();
    virtual void dataProcess();
};


class QCTask : public MeasureTask
{

};

class ErrorTask : public ITask
{
public:
    virtual QStringList loadCommands();
};

class StopTask : public ITask
{
public:
    virtual QStringList loadCommands();
};

class CleaningTask : public ITask
{    
public:
    virtual QStringList loadCommands();
};

class EmptyTask : public ITask
{
public:
    virtual QStringList loadCommands();
};

class DebugTask : public ITask
{
public:
    virtual QStringList loadCommands();

protected:
    ConfigSender sender;
};

class InitialLoadTask : public ITask
{
public:
    virtual QStringList loadCommands();
};


class InitialTask : public ITask
{
public:
    virtual QStringList loadCommands();
};

class DeviceConfigTask : public ITask
{
public:
    void loadParameters();
    void sendNextCommand();

    void oneCmdFinishEvent();
    void recvEvent();

protected:
    ConfigSender sender;
};

#endif // ITASK_H

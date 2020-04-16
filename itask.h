#ifndef ITASK_H
#define ITASK_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QTimer>
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
    TT_Func,

    TT_END /*end flag, don't use*/
};

enum ErrorFlag
{
    EF_NoError = 0,
    EF_HeatError,
    EF_SamplingError,
    EF_BlankError
};

class ITask
{
public:
    struct CorrelationArguments
    {
        int timeTab[20];
        int tempTab[20];
        int loopTab[20];
    };

    ITask();
    virtual ~ITask(){;}

    virtual bool start(IProtocol *protocol);
    virtual void stop();
    virtual void timeEvent();
    virtual void recvEvent();
    inline bool isWorking(){return workFlag;}
    inline ErrorFlag isError(){return errorFlag;}

    virtual void loadParameters();
    virtual void saveParameters();
    virtual QStringList loadCommands(){return QStringList();}
    virtual void fixCommands(const QStringList &sources);

protected:
    IProtocol *protocol; // shared
    QStringList commandList;
    int cmdIndex;
    QString cmd;

    bool workFlag;
    ErrorFlag errorFlag;
    CorrelationArguments corArgs;
};


class MeasureTask : public ITask
{
public:
    struct WorkArguments
    {
        int range;
        bool rangeLock;
        int pipe;

        float lineark;
        float linearb;
        float quada;
        float quadb;
        float quadc;
    };

    MeasureTask();

    // arguments:
    // 0 : range         int
    // 1 : rangeLock     bool
    // 2 : ...
    virtual bool start(IProtocol *protocol);

    virtual bool collectBlankValues();
    virtual bool collectColorValues();

    virtual void dataProcess();
    virtual void recvEvent();
    virtual void loadParameters();
    virtual void saveParameters();
    virtual QStringList loadCommands();

private:
    int blankSampleTimes;
    int colorSampleTimes;
    int blankValue;
    int colorValue;
    WorkArguments args;

    double vabs;
    double conc;
};


class CalibrationTask : public MeasureTask
{

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
};


class InitialTask : public ITask
{
public:
    virtual QStringList loadCommands();
};

#endif // ITASK_H

#ifndef ITASK_H
#define ITASK_H

#include <QString>
#include <QTimer>
#include <QVariant>
#include "iprotocol.h"
#include <QObject>


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
    TT_STOP,

    TT_END /*end flag, don't use*/
};


class IInsFlow
{

};

class ITask
{
public:
    enum TaskStatus
    {
        Error,
        Idle,
        Busy
    };

    ITask();
    virtual ~ITask(){;}

    virtual bool start(const QList<QVariant> &arguments, IProtocol *protocol);
    virtual void stop();
    virtual void TTimeEvent();
    virtual void TRecvEvent();
    virtual bool collectBlankValues();
    virtual bool collectColorValues();
    inline bool isWorking(){return workFlag;}

protected:
    virtual void decodeArguments(const QList<QVariant> &){;}
    virtual QStringList loadCommands() = 0;
    virtual void fixCommands(const QStringList &sources);

private:
    IProtocol *protocol; // shared
    QStringList commandList;
    int current;
    QString cmd;
    int tickCount;
    bool workFlag;
    bool isError;
};


class MeasureTask : public ITask
{
    struct WorkArguments
    {
        int range;
        bool rangeLock;

    };

public:
    // arguments:
    // 0 : range         int
    // 1 : rangeLock     bool
    // 2 : ...
    bool start(QList<QVariant> arguments, IProtocol *protocol);
};


class CalibrationTask : public MeasureTask
{

};


class  QCTask : public MeasureTask
{

};

class StopTask : public ITask
{

};

class CleaningTask : public ITask
{

};

class EmptyTask : public ITask
{


};

class DebugTask : public ITask
{

};

#endif // ITASK_H

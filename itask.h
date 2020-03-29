#ifndef ITASK_H
#define ITASK_H

#include <QString>
#include <QVariant>
#include "iprotocol.h"
#include <QObject>


enum TaskType
{
    TT_Idle,
    TT_Measure,
    TT_ZeroCalibration,
    TT_SampleCalibration,
    TT_ZeroCheck,
    TT_SampleCheck,
    TT_SpikedCheck
};


class IInsFlow
{

};

class ITask : public QObject
{
    Q_OBJECT

public:
    ITask(QObject *parent = NULL);

    virtual bool start(QList<QVariant> arguments, IProtocol *protocol);
    void stop();

private:
    IProtocol *protocol;
};

class MeasureTask : public ITask
{
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

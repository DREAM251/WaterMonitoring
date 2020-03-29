#include "elementinterface.h"
#include "profile.h"

MeasureMode::MeasureMode() :
    workFlag(false)
{
}

bool MeasureMode::startAutoMeasure(MeasureMode::AutoMeasureMode mode, const QString &parameter)
{
    return true;
}

void MeasureMode::stopAutoMeasure()
{

}

void MeasureMode::MMTimerEvent()
{

    if (!isWorking())
        return;
}

QDateTime MeasureMode::getNextPoint()
{
    return QDateTime::currentDateTime();
}



/////////



ElementInterface::ElementInterface(const QString &element, QObject *parent) :
    QObject(parent),
    MeasureMode(),
    timer(new QTimer(this)),
    counter(0),
    currentTaskType(TT_Idle),
    protocol(NULL),
    errorProc(NULL),
    currentTask(NULL)
{
    ElementFactory ef(element);
    flowTable = ElementFactory::


    timer->start(100);
    connect(timer, SIGNAL(timeout()), this, SLOT(TimerEvent()));

}

ElementInterface::~ElementInterface()
{
    qDeleteAll(flowTable.values());
    if (errorProc) delete errorProc;
    if (protocol) delete protocol;
}

int ElementInterface::getLastMeasureTime()
{
    return 0;
}

int ElementInterface::getCurrentWorkTime()
{
    return 0;
}

int ElementInterface::startTask(TaskType type)
{
    if (currentTaskType != TT_Idle || !flowTable.contains(type))
        return 1;

    ITask *task = flowTable.value(type);
    if (task)
        currentTask = task;
    else
        return 2;

    if (!currentTask->start(getStartArguments(type), protocol))
        return 3;

    currentTaskType = type;
    return 0;
}

QList<QVariant> ElementInterface::getStartArguments(TaskType type)
{
    QList<QVariant> args;
    return args;
}

void ElementInterface::TimerEvent()
{
    counter++;

    if (counter % 10 == 0)
        MMTimerEvent();

}

void ElementInterface::externTriggerMeasure()
{

}

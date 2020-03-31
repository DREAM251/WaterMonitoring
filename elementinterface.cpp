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



ElementInterface::ElementInterface(ElementType element, QObject *parent) :
    QObject(parent),
    MeasureMode(),
    timer(new QTimer(this)),
    counter(0),
    currentTaskType(TT_Idle),
    protocol(NULL),
    currentTask(NULL),
    factory(element)
{
    for (int i = 1 + (int)TT_Idle; i < (int)TT_END; i++)
    {
        TaskType tt = (TaskType)(i);

        ITask *it = factory.getTask(tt);
        if (it)
            flowTable.insert(tt, it);
    }
    protocol = factory.getProtocol();

    timer->start(100);
    connect(timer, SIGNAL(timeout()), this, SLOT(TimerEvent()));
}

ElementInterface::~ElementInterface()
{
    qDeleteAll(flowTable.values());
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

    if (counter % 5 == 0)
    {
        if (currentTask && currentTaskType != TT_Idle)
        {
            currentTask->recvEvent();
        }
    }

    if (counter % 10 == 0) {
        MMTimerEvent();

        if (currentTask) {
            currentTask->timeEvent();

            if (currentTask->isError() != EF_NoError) {
                currentTaskType = TT_Idle;
                currentTask = NULL;
                startTask(TT_ErrorProc);
            }
            else if (!currentTask->isWorking()) {
                currentTaskType = TT_Idle;
                currentTask = NULL;

                // ...
            }
        }
    }
}

void ElementInterface::externTriggerMeasure()
{

}

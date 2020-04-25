#include "elementinterface.h"
#include "profile.h"

MeasureMode::MeasureMode(ElementType element) :
    workFlag(false),
    element(element)
{
}

MeasureMode::~MeasureMode()
{
}

bool MeasureMode::startAutoMeasure(MeasureMode::AutoMeasureMode mode,
                                   const QString &parameter)
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
    MeasureMode(element),
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

    startTask(TT_Initial);
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

Receiver ElementInterface::getReceiver(){return protocol->getReceiver();}
Sender ElementInterface::getSender(){return protocol->getSender();}

QString ElementInterface::translateStartCode(int i)
{
    QString str;
    switch (i)
    {
    case 1:
        str = tr("设备忙");
        break;
    case 2:
        str = tr("通信端口未打开");
        break;
    case 3:
        str = tr("业务不存在");
        break;
    case 4:
        str = tr("启动业务失败");
        break;
    default:
        break;
    }
    return str;
}

int ElementInterface::startTask(TaskType type)
{
    ITask *task = NULL;
    if (currentTaskType != TT_Idle)
        return 1;

    if (!protocol->portIsOpened())
        return 2;

    if (flowTable.contains(type) && (task = flowTable.value(type)) != 0)
        currentTask = task;
    else
        return 3;

    if (!currentTask->start(protocol))
        return 4;

    currentTaskType = type;
    return 0;
}

void ElementInterface::stopTasks()
{
    if (currentTask)
        currentTask->stop();
    currentTaskType = TT_Idle;

    ITask *task = flowTable.value(TT_Stop);
    if (task) {
        currentTask = task;
        if (currentTask->start(protocol)){
            currentTaskType = TT_Stop;
        }
    }
}

void ElementInterface::TimerEvent()
{
    counter++;

    if (counter % 5 == 0)
    {
        if (currentTask && currentTaskType != TT_Idle){
            currentTask->recvEvent();
        }
    }

    if (counter % 10 == 0) {
        MMTimerEvent();

        if (currentTask) {
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

    if (currentTask)
        currentTask->timeEvent();
}

void ElementInterface::externTriggerMeasure()
{

}

#include "elementinterface.h"
#include "profile.h"
#include <QDebug>

MeasureMode::MeasureMode(QString element) :
    workFlag(false),
    element(element)
{
}

MeasureMode::~MeasureMode()
{
}

bool MeasureMode::startAutoMeasure(MeasureMode::AutoMeasureMode ,
                                   const QString &)
{
    return true;
}

void MeasureMode::stopAutoMeasure()
{

}

void MeasureMode::MMTimerEvent()
{
    QTime ct = QTime::currentTime();

    if (ct.second() < 2)
    {
        QPair<int,int> nt = getNextPoint(ct.addSecs(-2));
        if (nt.first == ct.hour() && nt.second == ct.minute()) {
            startTask(TT_Measure);
        }
    }
}

QPair<int, int> MeasureMode::getNextPoint(const QTime &st)
{
    int hour = -1;
    int min = 0;
    DatabaseProfile profile;
    if (profile.beginSection("measuremode")) {
        int measureMode = profile.value("MeasureMethod", 0).toInt();
        switch (measureMode)
        {
        case 0: {// 周期模式
            int period = profile.value("MeasurePeriod", 60).toInt() * 60;
            QDateTime dt = profile.value("MeasureStartTime").toDateTime(),
                    cdt = QDateTime::currentDateTime();

            cdt.setTime(st);
            if (cdt > dt) {
                int minGap = ((dt.secsTo(cdt) / period) + 1) * period;
                dt = dt.addSecs(minGap);
            }
            hour = dt.time().hour();
            min = dt.time().minute();
        }break;

        case 1: {// 定点模式
            int period = profile.value("PointMin").toInt();
            int nexthour =  st.addSecs(-60 * period).hour() + 1;

            for(int i=0; i<24 ; i++)
            {
                int j = (i+nexthour)%24;
                bool en = profile.value(QString("Point%1").arg(j), false).toBool();

                if(en) {
                    hour = j;
                    min = period;
                    break;
                }
            }
        }break;

        default:
            break;
        }
    }
    return QPair<int, int>(hour, min);
}

/////////


ElementInterface::ElementInterface(QString element, QObject *parent) :
    QObject(parent),
    MeasureMode(element),
    timer(new QTimer(this)),
    counter(0),
    currentTaskType(TT_Idle),
    protocol(NULL),
    currentTask(NULL),
    factory(new ElementFactory(element))
{
    for (int i = 1 + (int)TT_Idle; i < (int)TT_END; i++)
    {
        TaskType tt = (TaskType)(i);

        ITask *it = factory->getTask(tt);
        if (it) {
            it->setTaskType(tt);
            flowTable.insert(tt, it);
        }
    }
    protocol = factory->getProtocol();

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
    if (currentTaskType != TT_Idle &&
            (currentTaskType != TT_Debug && type != TT_Debug) &&
            (currentTaskType != TT_Config && type != TT_Config))
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

    if (counter % 10 == 0) {
        MMTimerEvent();

        if (currentTask) {
            if (currentTask->isError() != EF_NoError) {
                currentTaskType = TT_Idle;
                currentTask = NULL;
                startTask(TT_ErrorProc);
            }
            else if (!currentTask->isWorking()) {
                emit TaskFinished(currentTask->getTaskType());

                currentTaskType = TT_Idle;
                currentTask = NULL;
            }
        }
    }
}

void ElementInterface::externTriggerMeasure()
{
}

#ifndef ELEMENTINTERFACE_H
#define ELEMENTINTERFACE_H

#include "elementfactory.h"
#include "profile.h"
#include <QDateTime>
#include <QObject>
#include <QTimer>

class MeasureMode
{
public:
    enum AutoMeasureMode
    {
        AMT_Once,
        AMT_Circle,
        AMT_Gap,
        AMT_Extern
    };

    MeasureMode(QString element);
    ~MeasureMode();

    bool startAutoMeasure(AutoMeasureMode mode, const QString &parameter);
    void stopAutoMeasure();

    inline bool isWorking(){return workFlag;}
    virtual int startTask(TaskType type) = 0;

    void MMTimerEvent();
    QPair<int, int> getNextPoint(const QTime &st = QTime::currentTime());

private:
    bool workFlag;
    AutoMeasureMode mode;
    QString element;
};

class ElementInterface : public QObject,
        public MeasureMode
{
    Q_OBJECT

public:
    ElementInterface(QString element, QObject *parent = NULL);
    ~ElementInterface();

    int getLastMeasureTime();
    int getCurrentWorkTime();
    Receiver getReceiver();
    Sender getSender();

    TaskType getTaskType() {return currentTaskType;}
    ITask *getTask() {return currentTask;}
    ElementFactory *getFactory() {return factory;}

    QString translateStartCode(int);
    int startTask(TaskType type);
    void stopTasks();

public Q_SLOTS:
    void TimerEvent();
    void externTriggerMeasure();

Q_SIGNALS:
    void TaskFinished(int);
    void TaskStop(int);

private:
    QTimer *timer;
    int counter;
    TaskType currentTaskType;

    QHash<TaskType, ITask *> flowTable;
    IProtocol *protocol;
    ITask *currentTask;
    ElementFactory *factory;
};

#endif // ELEMENTINTERFACE_H

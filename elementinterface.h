#ifndef ELEMENTINTERFACE_H
#define ELEMENTINTERFACE_H

#include "elementfactory.h"
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

    MeasureMode();

    bool startAutoMeasure(AutoMeasureMode mode, const QString &parameter);
    void stopAutoMeasure();

    inline bool isWorking(){return workFlag;}
    virtual int startTask(TaskType type) = 0;

    void MMTimerEvent();

    QDateTime getNextPoint();

private:
    bool workFlag;
    AutoMeasureMode mode;
};

class ElementInterface : public QObject,
        public MeasureMode
{
    Q_OBJECT

public:
    ElementInterface(ElementType element, QObject *parent = NULL);
    ~ElementInterface();

    int getLastMeasureTime();
    int getCurrentWorkTime();

    int startTask(TaskType type);
    QList<QVariant> getStartArguments(TaskType type);

public Q_SLOTS:
    void TimerEvent();
    void externTriggerMeasure();

private:
    QTimer *timer;
    int counter;
    TaskType currentTaskType;

    QHash<TaskType, ITask *> flowTable;
    IProtocol *protocol;
    ITask *currentTask;
    ElementFactory factory;
};

#endif // ELEMENTINTERFACE_H

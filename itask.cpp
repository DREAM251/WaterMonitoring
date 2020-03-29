#include "itask.h"
#include "common.h"

ITask::ITask(QObject *parent) :
    QObject(parent),
    protocol(NULL)
{
}

bool ITask::start(QList<QVariant> arguments, IProtocol *protocol)
{

    return true;
}

void ITask::stop()
{

}

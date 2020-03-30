#include "itask.h"
#include "common.h"

#include <QFile>

ITask::ITask() :
    protocol(NULL),
    current(0),
    tickCount(0),
    workFlag(false),
    isError(false)
{
}

bool ITask::start(const QList<QVariant> &arguments , IProtocol *sp)
{
    if (sp)
    {
        protocol = sp;
        current = 0;
        tickCount = 0;
        workFlag = true;
        isError = false;
        cmd.clear();

        decodeArguments(arguments);
        fixCommands(loadCommands());
        return true;
    }
    else
        return false;
}

void ITask::stop()
{
    protocol = NULL;
    workFlag = false;
}


void ITask::TTimeEvent()
{
    if (!protocol || !workFlag)
        return;

    // send next command
    if (protocol->isIdle())
    {
        if (current < commandList.count())
        {
            cmd = commandList[current++];
            protocol->sendData(cmd);
        }
        else
            stop();
    }
    else if (protocol->isTimeOut())
    {
        stop();
    }
}

void ITask::TRecvEvent()
{
    if (protocol && protocol->recvNewData())
    {
        if (protocol->isBlankStep())
        {
            bool finished = collectBlankValues();
            if (finished)
                protocol->skipCurrentStep();
        }

        else if (protocol->isColorStep())
        {
            bool finished = collectColorValues();
            if (finished)
                protocol->skipCurrentStep();
        }

        else if (protocol->isHeatStep())
        {
            if (protocol->getRecvHeatTemp() >= protocol->getSentHeatTemp())
            {
                protocol->skipCurrentStep();
            }
            else if ()

        }
    }
}

bool ITask::collectBlankValues()
{

}

bool ITask::collectColorValues()
{

}

void ITask::fixCommands(const QStringList &sources)
{

}

//
//
//
//
//
//

bool MeasureTask::start(QList<QVariant> arguments, IProtocol *protocol)
{

}

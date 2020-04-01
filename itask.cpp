#include "itask.h"
#include "common.h"

#include <QFile>

ITask::ITask() :
    protocol(NULL),
    cmdIndex(0),
    workFlag(false),
    errorFlag(EF_NoError)
{
}

bool ITask::start(const QList<QVariant> &arguments , IProtocol *sp)
{
    if (sp)
    {
        protocol = sp;
        protocol->reset();
        cmdIndex = 0;
        workFlag = true;
        errorFlag = EF_NoError;
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
    if (protocol)
        protocol->reset();
    protocol = NULL;
    workFlag = false;
}

void ITask::timeEvent()
{
    if (!protocol || !workFlag)
        return;

    // send next command
    if (protocol->isTimeOut())
    {
        stop();
    }
    else if (protocol->isIdle())
    {
        if (cmdIndex < commandList.count())
        {
            cmd = commandList[cmdIndex++];
            protocol->sendData(cmd);
        }
        else
            stop();
    }
}

void ITask::recvEvent()
{
    if (protocol && protocol->recvNewData())
    {
        if (protocol->getSender()->isHeatStep())
        {
            if (protocol->getSender()->getHeatTemp() >= protocol->getReceiver()->getHeatTemp())
                protocol->skipCurrentStep();
        }

        else if (protocol->getSender()->isHeatJudgeStep())
        {
            if (protocol->getSender()->getHeatTemp() < protocol->getReceiver()->getHeatTemp() - 2)
                errorFlag = EF_HeatError;
            protocol->skipCurrentStep();
        }

        else if (protocol->getSender()->isWaterLevelJudgeStep())
        {
            if (protocol->getReceiver()->getWaterLevel() == 0)
                errorFlag = EF_SamplingError;
            protocol->skipCurrentStep();
        }
    }
}



//
//
//
//
//
//

MeasureTask::MeasureTask() :
    blankValue(0),
    colorValue(0)
{}

bool MeasureTask::start(const QList<QVariant> &arguments, IProtocol *protocol)
{
    if (ITask::start(arguments, protocol))
    {
        blankValue = 0;
        colorValue = 0;
        blankSampleTimes = 0;
        colorSampleTimes = 0;

        return true;
    }
    return false;
}

bool MeasureTask::collectBlankValues()
{
    const int sampleMaxTimes = 10;
    if (blankSampleTimes < sampleMaxTimes)
    {
        blankSampleTimes++;
        blankValue += protocol->getSender()->getLightVoltage();
    }

    if (blankSampleTimes >= sampleMaxTimes)
    {
        blankValue = blankValue / blankSampleTimes;
        return true;
    }
    return false;
}

bool MeasureTask::collectColorValues()
{
    const int sampleMaxTimes = 10;
    if (blankSampleTimes < sampleMaxTimes)
    {
        blankSampleTimes++;
        colorValue += protocol->getReceiver()->getLightVoltage();
    }

    if (colorSampleTimes >= sampleMaxTimes)
    {
        colorValue = colorValue / colorSampleTimes;
        return true;
    }
    return false;
}

void MeasureTask::dataProcess()
{
    double vblank = blankValue > 0 ? blankValue : 1;
    double vcolor = colorValue > 0 ? colorValue : 1;
    vabs = log10(vblank / vcolor);

    conc = vabs * args.lineark + args.linearb;
}

void MeasureTask::recvEvent()
{
    ITask::recvEvent();
    if (protocol && protocol->recvNewData())
    {
        if (protocol->getSender()->isBlankStep())
        {
            bool finished = collectBlankValues();
            if (finished) {
                if (protocol->getSender()->isBlankJudgeStep() && blankValue < 2500)
                    errorFlag = EF_BlankError;
                else if (colorSampleTimes > 0)
                    dataProcess();

                protocol->skipCurrentStep();
            }
        }

        else if (protocol->getSender()->isColorStep())
        {
            bool finished = collectColorValues();
            if (finished)
                protocol->skipCurrentStep();
            else if (blankSampleTimes > 0)
                dataProcess();
        }
    }
}

void MeasureTask::decodeArguments(const QList<QVariant> &arguments)
{
    for (int i = 0; i < arguments.count(); i++)
    {
        switch (i)
        {
        case 0: args.range = arguments[i].toInt(); break;
        case 1: args.rangeLock = arguments[i].toBool(); break;
        }
    }
}

QStringList MeasureTask::loadCommands()
{
    QStringList paths, commands;
    paths << "cleaning1.txt" << "sampling.txt" ; // ...

    for (int i = 0; i < paths.count(); i++)
        commands += loadCommandFileLines(paths[i]);
    return commands;
}


QStringList CleaningTask::loadCommands()
{
    QString path = "cleaning1.txt";
    return loadCommandFileLines(path);
}


QStringList StopTask::loadCommands()
{
    QString path = "stop.txt";
    return loadCommandFileLines(path);
}

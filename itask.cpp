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
    protocol = NULL;
    workFlag = false;
}

void ITask::timeEvent()
{
    if (!protocol || !workFlag)
        return;

    // send next command
    if (protocol->isIdle())
    {
        if (cmdIndex < commandList.count())
        {
            cmd = commandList[cmdIndex++];
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
        blankValue += protocol->getLightVoltage();
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
        colorValue += protocol->getLightVoltage();
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

    conc = vabs * args.k + args.b;
}

void MeasureTask::recvEvent()
{
    if (protocol && protocol->recvNewData())
    {
        if (protocol->isBlankStep())
        {
            bool finished = collectBlankValues();
            if (finished) {
                if (protocol->isBlankJudgeStep() && blankValue < 2500)
                    errorFlag = EF_BlankError;
                else if (colorSampleTimes > 0)
                    dataProcess();

                protocol->skipCurrentStep();
            }
        }

        else if (protocol->isColorStep())
        {
            bool finished = collectColorValues();
            if (finished)
                protocol->skipCurrentStep();
            else if (blankSampleTimes > 0)
                dataProcess();
        }

        else if (protocol->isHeatStep())
        {
            if (protocol->getRecvHeatTemp() >= protocol->getSentHeatTemp())
                protocol->skipCurrentStep();
        }

        else if (protocol->isHeatJudgeStep())
        {
            if (protocol->getRecvHeatTemp() < protocol->getSentHeatTemp() - 2)
                errorFlag = EF_HeatError;
            protocol->skipCurrentStep();
        }

        else if (protocol->isWaterLevelJudgeStep())
        {
            if (protocol->getRecvWaterLevel() == 0)
                errorFlag = EF_SamplingError;
            protocol->skipCurrentStep();
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
    QString path = "cleaning.txt";
    return loadCommandFileLines(path);
}

#include "itask.h"
#include "defines.h"
#include "profile.h"
#include <QDebug>
#include <QFile>

ITask::ITask() :
    protocol(NULL),
    cmdIndex(0),
    workFlag(false),
    errorFlag(EF_NoError)
{
    for (int i = 0; i < 20; i++)
    {
        corArgs.timeTab[i] = 2;
        corArgs.tempTab[i] = 2;
        corArgs.loopTab[i] = 2;
    }
}

bool ITask::start(IProtocol *sp)
{
    if (sp)
    {
        protocol = sp;
        protocol->reset();
        cmdIndex = 0;
        workFlag = true;
        errorFlag = EF_NoError;
        cmd.clear();

        loadParameters();
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
        // 加热判定
        if (protocol->getSender().heatReachStep())
        {
            if (protocol->getReceiver().heatTemp() >= protocol->getSender().heatTemp())
                protocol->skipCurrentStep();
        }
        else if (protocol->getSender().heatJudgeStep())
        {
            if (protocol->getReceiver().heatTemp() + 2 < protocol->getSender().heatTemp()) {
                errorFlag = EF_HeatError;
                stop();
            }
            protocol->skipCurrentStep();
        }

        // 降温判定
        if (protocol->getSender().coolReachStep())
        {
            if (protocol->getReceiver().heatTemp() <= protocol->getSender().heatTemp())
                protocol->skipCurrentStep();
        }
        else if (protocol->getSender().heatJudgeStep())
        {
            if (protocol->getReceiver().heatTemp() - 2 > protocol->getSender().heatTemp()) {
                errorFlag = EF_HeatError;
                stop();
            }
            protocol->skipCurrentStep();
        }


        // 液位判定
        if (protocol->getSender().waterLevelReachStep())
        {
            if (protocol->getSender().waterLevel() == protocol->getReceiver().waterLevel())
                protocol->skipCurrentStep();
        }
        else if (protocol->getSender().waterLevelJudgeStep())
        {
            if (protocol->getReceiver().waterLevel() != protocol->getSender().waterLevel()) {
                errorFlag = EF_SamplingError;
                stop();
            }
            protocol->skipCurrentStep();
        }
    }
}

void ITask::loadParameters()
{
    DatabaseProfile profile;
    if (profile.beginSection("settings"))
    {
        for (int i = 0; i < 20; i++)
        {
            corArgs.loopTab[i] = profile.value(QString("Loop%1").arg(i), 1).toInt();
            corArgs.tempTab[i] = profile.value(QString("Temp%1").arg(i)).toInt();
            corArgs.timeTab[i] = profile.value(QString("Time%1").arg(i), 3).toInt();
        }
    }
}

void ITask::saveParameters()
{

}


// command correlation
void ITask::fixCommands(const QStringList &sources)
{
    QStringList tempList;
    // 循环处理
    int loopStart = -1;
    int loopEnd = -1;
    int loopCount = 1;
    QStringList loopList;
    for (int i = 0; i < sources.count(); i++)
    {
        Sender sen(sources[i].toLatin1());
        int loopFlag = sen.loopFix();

        if (loopFlag > 0 && (loopFlag - 1) / 2 < sizeof(corArgs.timeTab) / sizeof (int))
        {
            if (loopStart < 0 && loopFlag%2 == 1)
            {
                loopStart = loopFlag;
                loopCount = corArgs.loopTab[(loopFlag - 1) / 2];
            }
            else if (loopStart > 0 && loopFlag == loopStart + 1)
                loopEnd = loopFlag;
        }

        if (loopStart > 0)
        {
            loopList << sen.rawData();

            if (loopEnd > 0)
            {
                for (int x = 0; x < loopCount; x++)
                    tempList += loopList;
                loopList.clear();
                loopStart = -1;
                loopEnd = -1;
                loopCount = 1;
            }
        }
        else
            tempList << sen.rawData();
    }
    if (!loopList.isEmpty())
        tempList += loopList;

    // 时间关联
    // 加热降温关联
    commandList.clear();
    for (int i = 0; i < tempList.count(); i++)
    {
        Sender sen(tempList[i].toLatin1());

        int timeIndex = sen.timeFix();
        if (timeIndex > 0 && timeIndex < sizeof(corArgs.timeTab) / sizeof(int))
            sen.setStepTime(corArgs.timeTab[timeIndex-1] + sen.timeAddFix());

        int tempIndex = sen.tempFix();
        if (tempIndex > 0 && tempIndex < sizeof(corArgs.tempTab) / sizeof(int))
            sen.setHeatTemp(corArgs.tempTab[tempIndex-1]);

        commandList << sen.rawData();
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

bool MeasureTask::start(IProtocol *protocol)
{
    if (ITask::start(protocol))
    {
        blankValue = 0;
        colorValue = 0;
        blankSampleTimes = 0;
        colorSampleTimes = 0;

        return true;
    }
    return false;
}

// 空白值采集
bool MeasureTask::collectBlankValues()
{
    const int sampleMaxTimes = 10;
    if (blankSampleTimes < sampleMaxTimes)
    {
        blankSampleTimes++;
        blankValue += protocol->getReceiver().lightVoltage1();
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
        colorValue += protocol->getReceiver().lightVoltage1();
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
    saveParameters();
}

void MeasureTask::recvEvent()
{
    ITask::recvEvent();
    if (protocol && protocol->recvNewData())
    {
        // 空白检测
        if (protocol->getSender().blankStep())
        {
            bool finished = collectBlankValues();
            if (finished) {
                if (blankValue < 2500) {
                    errorFlag = EF_BlankError;
                    stop();
                }
                else if (colorSampleTimes > 0)
                    dataProcess();

                protocol->skipCurrentStep();
            }
        }

        // 显色检测
        else if (protocol->getSender().colorStep())
        {
            bool finished = collectColorValues();
            if (finished)
                protocol->skipCurrentStep();
            else if (blankSampleTimes > 0)
                dataProcess();
        }

        //
    }
}

void MeasureTask::loadParameters()
{
    ITask::loadParameters();

    DatabaseProfile profile;
    if (profile.beginSection("measure"))
    {
        args.range = profile.value("range").toInt();
        args.rangeLock = profile.value("rangeLock").toBool();
        args.pipe = profile.value("pipe").toInt();
        args.lineark = profile.value("lineark", 1).toFloat();
        args.linearb = profile.value("linearb").toFloat();
        args.quada = profile.value("quada").toFloat();
        args.quadb = profile.value("quadb", 1).toFloat();
        args.quadc = profile.value("quadc").toFloat();
    }
}

void MeasureTask::saveParameters()
{
    ITask::saveParameters();
    DatabaseProfile profile;

    if (profile.beginSection("measure"))
    {
        profile.setValue("conc", conc);
        profile.setValue("abs", vabs);
    }
}

QStringList MeasureTask::loadCommands()
{
    QStringList paths, commands;
    paths << "measure.txt"; // ...

    for (int i = 0; i < paths.count(); i++)
        commands += loadCommandFileLines(paths[i]);
    return commands;
}

//
//
//
//
//
//

QStringList CleaningTask::loadCommands()
{
    return loadCommandFileLines("cleaning1.txt");
}

//
//
//
//
//
//

QStringList StopTask::loadCommands()
{
    return loadCommandFileLines("stop.txt");
}

//
//
//
//
//
//

QStringList ErrorTask::loadCommands()
{
    return loadCommandFileLines("error.txt");
}

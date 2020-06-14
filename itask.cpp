#include "itask.h"
#include "defines.h"
#include "profile.h"
#include "../globelvalues.h"
#include <QDebug>
#include <QFile>
#include <math.h>
#include <QTime>
#include "smooth.h"

int getRandom(int min,int max)
{
    qsrand(QTime(0, 0, 0).secsTo(QTime::currentTime()));
    return qrand()%(max-min);
}

float QuadRoot(float y,float jx,float A,float B ,float C)
{
    if(A==0.0){
        if(B==0.0)
            return 0.0;
        else
            return (y-C)/B;
    }else{
        float j = B*B-4*A*(C-y);
        if(j<0){
            return 0.0;
        }else{
            float cx = -B/(2*A);
            float v1 = (-B+sqrt(j))/(2*A);
            float v2 = (-B-sqrt(j))/(2*A);
            //取与原始吸光度相同的一边
            if(jx<=cx){
                return v1<v2?v1:v2;
            }else{
                return v1>v2?v1:v2;
            }
        }
    }
}

ITask::ITask(QObject *parent) :
    QObject(parent),
    protocol(NULL),
    cmdIndex(0),
    errorFlag(EF_NoError),
    workFlag(false),
    processSeconds(0),
    pipe(-1),
    realTimeConc(0)
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
        connect(protocol, SIGNAL(DataReceived()), this, SLOT(DataReceived()));
        connect(protocol, SIGNAL(ComFinished()), this, SLOT(CommandEnd()));
        connect(protocol, SIGNAL(ComTimeout()), this, SLOT(Timeout()));
        cmdIndex = 0;
        errorFlag = EF_NoError;
        cmd.clear();
        workFlag = true;
        startTime = QDateTime::currentDateTime();

        loadParameters();
        fixCommands(loadCommands());
        sendNextCommand();
        return true;
    }
    else
        return false;
}

void ITask::stop()
{
    if (protocol) {
        protocol->disconnect(SIGNAL(DataReceived()));
        protocol->disconnect(SIGNAL(ComFinished()));
        protocol->disconnect(SIGNAL(ComTimeout()));
        protocol->reset();
    }
    workFlag = false;
    protocol = NULL;
}

void ITask::oneCmdFinishEvent()
{
    // 液位抽取是否成功判定
    if (protocol->getSender().waterLevelReachStep() ||
            protocol->getSender().waterLevelReachStep2() ||
            protocol->getSender().waterLevelReachStep3())
    {
        if (protocol->getSender().judgeStep() > protocol->getReceiver().pumpStatus())
        {
            QString pp = protocol->getSender().getTCValve1Name(
                        protocol->getSender().TCValve1());
            addErrorMsg(QObject::tr("%1抽取失败，请检查").arg(pp), 1);
            errorFlag = EF_SamplingError;
            stop();
            return;
        }
    }

    // 加热异常判定
    if (protocol->getSender().heatReachStep())
    {
        if (protocol->getReceiver().heatTemp() + 3 < protocol->getSender().heatTemp()) {
            addErrorMsg(QObject::tr("加热异常，请检查"), 1);
            errorFlag = EF_HeatError;
            stop();
            return;
        }
    }

    // 降温异常判定
    if (protocol->getSender().coolReachStep())
    {
        if (protocol->getReceiver().heatTemp() - 3 > protocol->getSender().heatTemp()) {
            addErrorMsg(QObject::tr("降温异常，请检查"), 1);
            errorFlag = EF_HeatError;
            stop();
            return;
        }
    }

    sendNextCommand();
}


void ITask::sendNextCommand()
{
    protocol->skipCurrentStep();
    if (cmdIndex < commandList.count())
    {
        cmd = commandList[cmdIndex++];
        protocol->sendData(cmd);
    }
    else {
        stop();

        processSeconds = getProcess();
        DatabaseProfile profile;
        if (profile.beginSection("measuremode")) {
            profile.setValue(QString("taskTime/%1").arg((int)taskType), processSeconds);
        }
    }
}

void ITask::recvEvent()
{   
    // 加热到达判定
    if (protocol->getSender().heatReachStep())
    {
        if (protocol->getReceiver().heatTemp() >= protocol->getSender().heatTemp() - 2)
            sendNextCommand();
    }

    // 降温到达判定
    if (protocol->getSender().coolReachStep())
    {
        if (protocol->getReceiver().heatTemp() <= protocol->getSender().heatTemp() + 2)
            sendNextCommand();
    }


    // 液位到达判定
    if (protocol->getSender().waterLevelReachStep() ||
            protocol->getSender().waterLevelReachStep2() ||
            protocol->getSender().waterLevelReachStep3())
    {
        if (protocol->getSender().judgeStep() <= protocol->getReceiver().pumpStatus())
            sendNextCommand();
    }

    // 定量结束判定
    if (protocol->getSender().waterLevel() > 0 &&
            protocol->getReceiver().waterLevel() < protocol->getSender().waterLevel())
        sendNextCommand();
}

int ITask::getProcess()
{
    return startTime.secsTo(QDateTime::currentDateTime());
}

void ITask::loadParameters()
{
    DatabaseProfile profile;
    if (profile.beginSection("settings"))
    {
        for (int i = 0; i < 20; i++)
        {
            corArgs.loopTab[i] = profile.value(QString("Loop%1").arg(i), 0).toInt();
            corArgs.tempTab[i] = profile.value(QString("Temp%1").arg(i)).toInt();
            corArgs.timeTab[i] = profile.value(QString("Time%1").arg(i), 3).toInt();
        }
    }
    if (profile.beginSection("measuremode")) {
        processSeconds = profile.value(QString("taskTime/%1").arg((int)taskType), 0).toInt();
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

        if (loopFlag > 0 && (loopFlag - 1) / 2 < sizeof(corArgs.loopTab) / sizeof (int))
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

    commandList.clear();
    for (int i = 0; i < tempList.count(); i++)
    {
        Sender sen(tempList[i].toLatin1());

        // 时间关联
        int timeIndex = sen.timeFix();
        if (timeIndex > 0 && timeIndex < sizeof(corArgs.timeTab) / sizeof(int))
            sen.setStepTime(corArgs.timeTab[timeIndex-1] + sen.timeAddFix());

        // 加热降温关联
        int tempIndex = sen.tempFix();
        if (tempIndex > 0 && tempIndex < sizeof(corArgs.tempTab) / sizeof(int))
            sen.setHeatTemp(corArgs.tempTab[tempIndex-1]);

        //  管道切换
        if (pipe >= 0 && sen.TCValve1() == 3)
            sen.setTCValve1(pipe);

        commandList << sen.rawData();
    }
}

void ITask::DataReceived()
{
    recvEvent();
}

void ITask::CommandEnd()
{
    oneCmdFinishEvent();
}

void ITask::Timeout()
{
    stop();
}


MeasureTask::MeasureTask() :
    blankValue(0),
    colorValue(0),
  blankValueC2(0),
  colorValueC2(0)
{
//    testDataProcess();
}

bool MeasureTask::start(IProtocol *protocol)
{
    if (ITask::start(protocol))
    {
        clearCollectedValues();
        return true;
    }
    return false;
}

void MeasureTask::clearCollectedValues()
{
    blankValue = 0;
    colorValue = 0;
    blankValueC2 = 0;
    colorValueC2 = 0;
    blankSampleTimes = 0;
    colorSampleTimes = 0;
    realTimeConc = 0;
}

// 空白值采集
bool MeasureTask::collectBlankValues()
{
    const int sampleMaxTimes = 10;

    if (++blankSampleTimes <= sampleMaxTimes)
    {
        blankValue += protocol->getReceiver().measureSignal();
        blankValueC2 += protocol->getReceiver().refLightSignal();
    }

    if (blankSampleTimes == sampleMaxTimes)
    {
        blankValue = blankValue / blankSampleTimes;
        blankValueC2 = blankValueC2 / blankSampleTimes;
        return true;
    }
    return false;
}

bool MeasureTask::collectColorValues()
{
    const int sampleMaxTimes = 10;

    if (++colorSampleTimes <= sampleMaxTimes)
    {
        colorValue += protocol->getReceiver().measureSignal();
        colorValueC2 += protocol->getReceiver().refLightSignal();
    }

    if (colorSampleTimes == sampleMaxTimes)
    {
        colorValue = colorValue / colorSampleTimes;
        colorValueC2 = colorValueC2 / colorSampleTimes;
        return true;
    }
    return false;
}

void MeasureTask::dataProcess()
{
    double vblank = blankValue > 0 ? blankValue : 1;
    double vcolor = colorValue > 0 ? colorValue : 1;
    double vblank1 = blankValueC2 > 100 ? blankValueC2 : 100;
    double vcolor1 = colorValueC2 > 100 ? colorValueC2 : 100;
    vabs = log10(vblank / vcolor) - log10(vblank1 / vcolor1) * (args.turbidityOffset); // 浊度系数填在这里

    // 出厂标定运算
    double v1 = vabs * vabs * args.quada + vabs *args.quadb + args.quadc;

    // 用户标定运算
    double v2 = v1 * args.lineark + args.linearb;

    //稀释比例
    double dilutionRate = 1.0;
    if (corArgs.loopTab[2] > 0)
        dilutionRate = (double)(corArgs.loopTab[2] + corArgs.loopTab[3]) / corArgs.loopTab[2];
    double v3 = v2 * dilutionRate;

    // 用户修正系数
    double v4 = v3 * args.userk + args.userb;

    double c = 0;
    // 防止负值或者很小的值
    if (v4 < 0.01) {
        v4 = getRandom(1, 100) / 1000.0;
    }
    // 平滑和反算
    smooth s(0);
    s.setThreshold(args.smoothRange);
    c = s.calc(v4);

    //反推显色值
    if (args.userk != 0.0 && args.lineark != 0.0)
    {
        float rv1 = (c - args.userb) / args.userk;
        float rv2 = rv1 / dilutionRate;
        float rv3 = (rv2 - args.linearb) / args.lineark;
        float rv4 = QuadRoot(rv3, log10(vblank / vcolor), args.quada, args.quadb, args.quadc)
                + log10(vblank1 / vcolor1) * (args.turbidityOffset);
        float rv5 = blankValue/pow(10, rv4);
        float rv6 = rv5 < 1 ? 1 : rv5;
        vabs = log10(vblank / rv6) - log10(vblank1 / vcolor1) * (args.turbidityOffset);
        colorValue = rv6;
    }

    QString strResult;
    conc = setPrecision(c, 4, &strResult);

    QList<QVariant> data;
    data << strResult;
    data << QString::number(vabs, 'f', 4);
    data << QString::number(blankValue);
    data << QString::number(colorValue);
    data << QString::number(blankValueC2);
    data << QString::number(colorValueC2);
    data << QString::number(protocol->getReceiver().mcu1Temp());
    data << QObject::tr("M");
    data << QString::number(vcolor);
    data << QString::number(args.turbidityOffset);
    data << QString("%1,%2,%3").arg(args.quada).arg(args.quadb).arg(args.quadc);
    data << QString("%1,%2").arg(args.lineark).arg(args.linearb);
    data << QString("%1,%2").arg(corArgs.loopTab[2]).arg(corArgs.loopTab[3]);
    data << QString("%1,%2").arg(args.userk).arg(args.userb);
    data << QString::number(args.smoothRange);

    addMeasureData(data);
    saveParameters();
}


float MeasureTask::realTimeDataProcess(int blankValue,
                                       int colorValue,
                                       int blankValueC2,
                                       int colorValueC2)
{
    double vblank = blankValue > 0 ? blankValue : 1;
    double vcolor = colorValue > 0 ? colorValue : 1;
    double vblank1 = blankValueC2 > 100 ? blankValueC2 : 100;
    double vcolor1 = colorValueC2 > 100 ? colorValueC2 : 100;
    vabs = log10(vblank / vcolor) - log10(vblank1 / vcolor1) * (args.turbidityOffset); // 浊度系数填在这里

    // 出厂标定运算
    double v1 = vabs * vabs * args.quada + vabs *args.quadb + args.quadc;

    // 用户标定运算
    double v2 = v1 * args.lineark + args.linearb;

    //稀释比例
    double dilutionRate = 1.0;
    if (corArgs.loopTab[2] > 0)
        dilutionRate = (double)(corArgs.loopTab[2] + corArgs.loopTab[3]) / corArgs.loopTab[2];
    double v3 = v2 * dilutionRate;

    // 用户修正系数
    double v4 = v3 * args.userk + args.userb;

    // 防止负值或者很小的值
    if (v4 < 0.01) {
        v4 = getRandom(1, 100) / 1000.0;
    }

    return v4;
}

void MeasureTask::recvEvent()
{
    ITask::recvEvent();
    // 空白检测
    if (protocol->getSender().blankStep())
    {
        bool finished = collectBlankValues();
        if (finished) {
            if (blankValue < args.blankErrorValue) {
                addErrorMsg(QObject::tr("空白值为%1，为异常值，请检查").arg(blankValue), 1);
                errorFlag = EF_BlankError;
                stop();
                return;
            } else if (colorSampleTimes > 0 && blankSampleTimes > 0)  {
                dataProcess();
                clearCollectedValues();
            }

            sendNextCommand();
        }
    }

    // 显色检测
    else if (protocol->getSender().colorStep())
    {
        bool finished = collectColorValues();
        if (finished) {
            // 计算
            if (colorSampleTimes > 0 && blankSampleTimes > 0) {
                dataProcess();
                clearCollectedValues();
            }

            sendNextCommand();
        }
    }

    // 实时计算
    else if (protocol->getSender().realTimeValueStep())
    {
          realTimeConc = realTimeDataProcess(blankValue == 0 ? lastBlankValue : blankValue,
                                             blankValueC2 == 0 ? lastBlankValueC2 : blankValueC2,
                                             protocol->getReceiver().measureSignal(),
                                             protocol->getReceiver().refLightSignal());
    }

}

void MeasureTask::loadParameters()
{
    ITask::loadParameters();

    DatabaseProfile profile;

    if (profile.beginSection("measuremode")) {
        args.mode = profile.value("OnlineOffline", 0).toInt();
        args.pipe = profile.value("SamplePipe").toInt();
    }

    if (profile.beginSection("measure"))
    {
        args.range = profile.value("range").toInt();
        args.rangeLock = profile.value("rangeLock").toBool();
        args.lineark = profile.value("lineark", 1).toFloat();
        args.linearb = profile.value("linearb").toFloat();
        args.quada = profile.value("quada").toFloat();
        args.quadb = profile.value("quadb", 1).toFloat();
        args.quadc = profile.value("quadc").toFloat();

        // 在线测量
        if (args.mode == 0)
        {
            pipe = 3;
            corArgs.loopTab[4] = 1;
            corArgs.loopTab[5] = 1;
        } else {
            switch (args.pipe)
            {
            case 0: pipe = 3; break;
            case 1: pipe = 4; break;
            case 2: pipe = 1; break;
            case 3: pipe = 8; break;
            }
            corArgs.loopTab[4] = 0;
            corArgs.loopTab[5] = 0;
        }

        lastBlankValue = profile.value("blankValue").toInt();
        lastBlankValueC2 = profile.value("blankValueC2").toInt();
    }

    if (profile.beginSection("settings"))
    {
        args.blankErrorValue = profile.value("BlankErrorThreshold").toInt();
        args.userk = profile.value("UserK", 1).toFloat();
        args.userb = profile.value("UserB").toFloat();

        args.smoothRange = profile.value("SmoothOffset").toFloat() / 100.0;
        args.turbidityOffset = profile.value("TurbidityOffset", 1).toFloat();
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

        profile.setValue("blankValue", blankValue);
        profile.setValue("blankValueC2", blankValueC2);
        profile.setValue("colorValue", colorValue);
        profile.setValue("colorValueC2", colorValueC2);
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

void MeasureTask::testDataProcess()
{
    args.lineark = 1856.93;
    args.linearb = 108.16;
    args.quada = 0;
    args.quadb = 1;
    args.quadc = 0;

    args.userk = 1;
    args.userb = 0;

    args.smoothRange = 0.1;
    args.turbidityOffset = 1;


    corArgs.loopTab[2] = 2;
    corArgs.loopTab[3] = 1;

    // 第1组
    blankValue = 20876;
    colorValue = 23036;
    blankValueC2 = 19816;
    colorValueC2 = 19812;
    dataProcess();
    qDebug() <<  "1:" <<  QString::number(conc)
         << QString::number(vabs, 'f', 4)
        << QString::number(colorValue);


    // 第2组
    blankValue = 20811;
    colorValue = 22923;
    blankValueC2 = 19761;
    colorValueC2 = 19754;
    dataProcess();
    qDebug() <<  "2:" <<  QString::number(conc)
         << QString::number(vabs, 'f', 4)
        << QString::number(colorValue);


    // 第3组
    blankValue = 20868;
    colorValue = 22989;
    blankValueC2 = 19743;
    colorValueC2 = 19731;
    dataProcess();
    qDebug() <<  "3:" <<  QString::number(conc)
         << QString::number(vabs, 'f', 4)
        << QString::number(colorValue);


    // 第4组
    blankValue = 20869;
    colorValue = 23015;
    blankValueC2 = 19759;
    colorValueC2 = 19744;
    dataProcess();
    qDebug() <<  "4:" <<  QString::number(conc)
         << QString::number(vabs, 'f', 4)
        << QString::number(colorValue);
}


///////////////////////////////////////////////////////////////////////


CalibrationTask::CalibrationTask() :
    MeasureTask()
{
}

void CalibrationTask::loadParameters()
{
    MeasureTask::loadParameters();

    if (pframe) {
        corArgs.loopTab[2] = pframe->getCurrentSample(); //
        corArgs.loopTab[3] = pframe->getCurrentWater(); //
        pipe = pframe->getCurrentPipe();
    }
}

void CalibrationTask::dataProcess()
{
    int vblank = blankValue > 0 ? blankValue : 1;
    int vcolor = colorValue > 0 ? colorValue : 1;
    int vblank2 = blankValueC2 > 0 ? blankValueC2 : 1;
    int vcolor2 = colorValueC2 > 0 ? colorValueC2 : 1;

    if (pframe)
    {
        pframe->setVLight(vblank, vcolor, vblank2, vcolor2);

        double lfitA,lfitB,lfitR;
        double qfitA,qfitB,qfitC,qfitR;
        DatabaseProfile dbprofile;
        if (dbprofile.beginSection("measure"))
        {
            lfitA=dbprofile.value("lineark",1).toDouble();
            lfitB=dbprofile.value("linearb",0).toDouble();
            lfitR=dbprofile.value("linearr",1).toDouble();

            qfitA=dbprofile.value("quada",0).toDouble();
            qfitB=dbprofile.value("quadb",1).toDouble();
            qfitC=dbprofile.value("quadc",0).toDouble();
            qfitR=dbprofile.value("quadr",1).toDouble();
        }

        QList<QVariant> data;
        data << pframe->getCurrentName();
        data << QString::number(pframe->getCurrentConc());
        data << QString::number(pframe->getCurrentAbs());
        data << QString::number(blankValue);
        data << QString::number(colorValue);
        data << QString::number(blankValueC2);
        data << QString::number(colorValueC2);
        data << QString::number(protocol->getReceiver().mcu1Temp());
        data << QObject::tr("C");
        data << QString::number(lfitA);
        data << QString::number(lfitB);
        data << QString::number(lfitR);
        data << QString::number(qfitA);
        data << QString::number(qfitB);
        data << QString::number(qfitC);
        data << QString::number(qfitR);

        addCalibrationData(data);
    }
}

////////////////////////////////////////////////////////////////////////

QStringList CleaningTask::loadCommands()
{
    return loadCommandFileLines("wash.txt");
}

QStringList StopTask::loadCommands()
{
    return loadCommandFileLines("stop.txt");
}

QStringList ErrorTask::loadCommands()
{
    return loadCommandFileLines("error.txt");
}

QStringList EmptyTask::loadCommands()
{
    return loadCommandFileLines("drain.txt");
}

QStringList InitialTask::loadCommands()
{
    return loadCommandFileLines("poweron.txt");
}


QStringList InitialLoadTask::loadCommands()
{
    return loadCommandFileLines("initialize.txt");
}

QStringList DebugTask::loadCommands()
{
    QStringList commandList = loadCommandFileLines("test.txt");

    if (commandList.count() > 0) {
        Sender sender(commandList[0].toLatin1());
        DatabaseProfile profile;
        if (profile.beginSection("pumpTest"))
        {
            int tv1 =  profile.value("TV1", 0).toInt();
            int tv2 =  profile.value("TV2", 0).toInt();
            int valve[12];
            for (int i = 0; i < 12; i++)
                valve[i] = profile.value(QString("valve%1").arg(i), 0).toInt();
            int workTime = profile.value("WorkTime", 100).toInt();
            int temp = profile.value("Temp", 0).toInt();
            int pump1 = profile.value("PumpRotate1", 0).toInt();
            int pump2 = profile.value("PumpRotate2", 0).toInt();
            int speed = profile.value("Speed", 20).toInt();

            sender.setTCValve1(tv1);
            sender.setTCValve2(tv2);
            sender.setStepTime(workTime);
            sender.setHeatTemp(temp);

            sender.setPeristalticPump(pump1);
            sender.setPeristalticPumpSpeed(speed);
            sender.setPump2(pump2);

            sender.setValve1(valve[0]);
            sender.setValve2(valve[1]);
            sender.setValve3(valve[2]);
            sender.setValve4(valve[3]);
            sender.setValve5(valve[4]);
            sender.setValve6(valve[5]);
            sender.setValve7(valve[6]);
            sender.setValve8(valve[7]);
            sender.setExtValve(valve[8]);
            sender.setFun(valve[9]);
//            sender.setExtControl1(valve[10]);
//            sender.setExtControl2(valve[11]);
//            sender.setExtControl3(valve[12]);
//            sender.setWaterLevel(valve[11]);
        }
        commandList[0] = sender.rawData();
    }
    return commandList;
}

void DeviceConfigTask::loadParameters()
{
    DatabaseProfile profile;
    if (profile.beginSection("lightVoltage"))
    {
        sender.setLed1Current(profile.value("Color1Current").toInt());
        sender.setLed2Current(profile.value("Color2Current").toInt());
        sender.setPD1Incred(profile.value("Color1Gain").toInt() + 1);
        sender.setPD2Incred(profile.value("Color2Gain").toInt() + 1);
        sender.setWaterLevelLed23Current(profile.value("CurrentHigh").toInt());
        sender.setWaterLevelLed1Current(profile.value("CurrentLow").toInt());
        sender.setWaterLevel1Threshold(profile.value("ThresholdHigh").toInt());
        sender.setWaterLevel2Threshold(profile.value("ThresholdMid").toInt());
        sender.setWaterLevel3Threshold(profile.value("ThresholdLow").toInt());
    }
}

void DeviceConfigTask::sendNextCommand()
{
    protocol->skipCurrentStep();
    protocol->sendConfig(sender);
}

void DeviceConfigTask::oneCmdFinishEvent()
{
    stop();
}

void DeviceConfigTask::recvEvent()
{
    stop();
}

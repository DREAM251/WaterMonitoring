#include "iprotocol.h"
#include <QStringList>
#include <QDebug>
#include "defines.h"
#include "common.h"
#include "profile.h"


#define PACKET_MIN_LENGTH  7
#define PACKET_HEAD_LENGTH 4

QByteArray checkSum(const QByteArray &by)
{
    int sum = 0;
    for (int i = 0; i < by.count(); i++)
        sum += (uint)(by[i]);

    return QByteArray(1, (char)(sum % 256)).toHex().toUpper();
}


QByteArray Sender::data()
{
    int index = sent.indexOf(':');
    if (index < 0)
        index = sent.length();

    QByteArray da = "#" + QString("00%1").arg(index + PACKET_MIN_LENGTH).right(2).toLatin1() + "1" + sent.left(index);
    return da + checkSum(da) + '!';
}

QString Sender::translateExplainCode()
{
    QString str;

    switch (explainCode())
    {
    case 1:str = QObject::tr("降温");break;
    case 2:str = QObject::tr("排空比色池");break;
    case 3:str = QObject::tr("排空计量管");break;
    case 4:str = QObject::tr("开采样");break;
    case 5:str = QObject::tr("水样润洗");break;
    case 6:str = QObject::tr("进") + getTCValve1Name(TCValve1());break;
    case 7:str = QObject::tr("消解");break;
    case 8:str = QObject::tr("空白检测");break;
    case 9:str = QObject::tr("比色检测");break;
    case 10:str = QObject::tr("流路清洗");break;
    case 11:str = QObject::tr("显色");break;
    case 12:str = QObject::tr("静置");break;
    case 13:str = QObject::tr("鼓泡");break;
    case 14:str = QObject::tr("试剂替换");break;
    case 15:str = QObject::tr("空闲");break;
    }
    return str;
}

QString Sender::getTCValve1Name(int i)
{
    switch (i)
    {
    case 1:return QObject::tr("tv1");break;
    case 2:return QObject::tr("tv2");break;
    case 3:return QObject::tr("tv3");break;
    case 4:return QObject::tr("tv4");break;
    case 5:return QObject::tr("tv5");break;
    case 6:return QObject::tr("tv6");break;
    case 7:return QObject::tr("tv7");break;
    case 8:return QObject::tr("tv8");break;
    case 9:return QObject::tr("tv9");break;
    case 10:return QObject::tr("tv10");break;
    }
    return "";
}


int Sender::step(){return sent.left(4).toInt();}
int Sender::stepTime(){return sent.mid(4, 4).toInt();}
int Sender::peristalticPump(){return sent.mid(8, 1).toInt();}
int Sender::peristalticPumpSpeed(){return sent.mid(9, 2).toInt();}
int Sender::pump2(){return sent.mid(11, 1).toInt();}
int Sender::TCValve1(){return sent.mid(12, 1).toInt();}
int Sender::TCValve2(){return sent.mid(13, 1).toInt();}
int Sender::valve1(){return sent.mid(14, 1).toInt();}
int Sender::valve2(){return sent.mid(15, 1).toInt();}
int Sender::valve3(){return sent.mid(16, 1).toInt();}
int Sender::valve4(){return sent.mid(17, 1).toInt();}
int Sender::valve5(){return sent.mid(18, 1).toInt();}
int Sender::valve6(){return sent.mid(19, 1).toInt();}
int Sender::valve7(){return sent.mid(20, 1).toInt();}
int Sender::valve8(){return sent.mid(21, 1).toInt();}
int Sender::extValve(){return sent.mid(22, 1).toInt();}
int Sender::extControl1(){return sent.mid(23, 1).toInt();}
int Sender::extControl2(){return sent.mid(24, 1).toInt();}
int Sender::extControl3(){return sent.mid(25, 1).toInt();}
int Sender::_420mA(){return sent.mid(26, 4).toInt();}
int Sender::fun(){return sent.mid(30, 1).toInt();}
int Sender::waterLevel(){return sent.mid(31, 1).toInt();}
float Sender::heatTemp(){return sent.mid(32, 4).toFloat() / 10.0;}
int Sender::waterLedControl(){return sent.mid(36, 1).toInt();}
int Sender::measureLedControl(){return sent.mid(37, 1).toInt();}
//int Sender::reserve(){return sent.mid(36, 4).toInt();}
//int Sender:::(){return sent.mid(40, 1).toInt();}

int Sender::timeFix(){return sent.mid(43, 2).toInt();}
int Sender::timeAddFix(){return sent.mid(45, 4).toInt();}
int Sender::tempFix(){return sent.mid(49, 2).toInt();}
int Sender::loopFix(){return sent.mid(51, 2).toInt();}
int Sender::judgeStep(){return sent.mid(53, 1).toInt();}
bool Sender::waterLevelReachStep(){return judgeStep() == 1;}
bool Sender::waterLevelReachStep2(){return judgeStep() == 2;}
bool Sender::waterLevelReachStep3(){return judgeStep() == 3;}
bool Sender::heatReachStep(){return judgeStep() == 4;}
bool Sender::heatJudgeStep(){return judgeStep() == -1;}
bool Sender::coolReachStep(){return judgeStep() == 5;}
bool Sender::coolJudgeStep(){return judgeStep() == -1;}
bool Sender::blankStep(){return sent.mid(54, 1).toInt() == 1;}
bool Sender::colorStep(){return sent.mid(54, 1).toInt() == 2;}
int Sender::explainCode(){return sent.mid(55, 2).toInt();}

void Sender::setStep(int i){sent.replace(0, 4, QString("0000%1").arg(i).right(4).toLatin1());}
void Sender::setStepTime(int i){sent.replace(4, 4, QString("0000%1").arg(i).right(4).toLatin1());}
void Sender::setPeristalticPump(int i){sent.replace(8, 1, QString("0000%1").arg(i).right(1).toLatin1());}
void Sender::setPeristalticPumpSpeed(int i){sent.replace(9, 2, QString("0000%1").arg(i).right(2).toLatin1());}
void Sender::setPump2(int i){sent.replace(11, 1, QString("0000%1").arg(i).right(1).toLatin1());}
void Sender::setTCValve1(int i)
{
    if (i >= 10)
        sent[12] = 'A';
    else
        sent.replace(12, 1, QString("0000%1").arg(i).right(1).toLatin1());
}
void Sender::setTCValve2(int i)
{
    if (i >= 10)
        sent[13] = 'A';
    else
        sent.replace(13, 1, QString("0000%1").arg(i).right(1).toLatin1());
}
void Sender::setValve1(int i){sent.replace(14, 1, QString("0000%1").arg(i).right(1).toLatin1());}
void Sender::setValve2(int i){sent.replace(15, 1, QString("0000%1").arg(i).right(1).toLatin1());}
void Sender::setValve3(int i){sent.replace(16, 1, QString("0000%1").arg(i).right(1).toLatin1());}
void Sender::setValve4(int i){sent.replace(17, 1, QString("0000%1").arg(i).right(1).toLatin1());}
void Sender::setValve5(int i){sent.replace(18, 1, QString("0000%1").arg(i).right(1).toLatin1());}
void Sender::setValve6(int i){sent.replace(19, 1, QString("0000%1").arg(i).right(1).toLatin1());}
void Sender::setValve7(int i){sent.replace(20, 1, QString("0000%1").arg(i).right(1).toLatin1());}
void Sender::setValve8(int i){sent.replace(21, 1, QString("0000%1").arg(i).right(1).toLatin1());}
void Sender::setExtValve(int i){sent.replace(22, 1, QString("0000%1").arg(i).right(1).toLatin1());}
void Sender::setExtControl1(int i){sent.replace(23, 1, QString("0000%1").arg(i).right(1).toLatin1());}
void Sender::setExtControl2(int i){sent.replace(24, 1, QString("0000%1").arg(i).right(1).toLatin1());}
void Sender::setExtControl3(int i){sent.replace(25, 1, QString("0000%1").arg(i).right(1).toLatin1());}
void Sender::set420mA(int i){sent.replace(26, 4, QString("0000%1").arg(i).right(4).toLatin1());}
void Sender::setFun(int i){sent.replace(30, 1, QString("0000%1").arg(i).right(1).toLatin1());}
void Sender::setWaterLevel(int i){sent.replace(31, 1, QString("0000%1").arg(i).right(1).toLatin1());}
void Sender::setHeatTemp(float i){sent.replace(32, 4, QString("0000%1").arg((int)(i*10)).right(4).toLatin1());}



ConfigSender::ConfigSender(const QByteArray &src) :
    sent(src)
{}


ConfigSender::ConfigSender()
{
    sent = "0001000000000000000000000000000000";
}

QByteArray ConfigSender::data()
{
    QByteArray length = QString("00%1").arg(sent.length() + PACKET_MIN_LENGTH).right(2).toLatin1();
    QByteArray da = "#" + length + "1" + sent;
    return da + checkSum(da) + '!';
}
/*
void ConfigSender::setWaterLevelAlwaysOn(int i)
{sent.replace(4, 1, QString("0000%1").arg(i).right(1).toLatin1());}

void ConfigSender::setLedAlwaysOn(int i )
{sent.replace(5, 1, QString("0000%1").arg(i).right(1).toLatin1());}

void ConfigSender::setWaterLevelRealTimeCheck(int i)
{sent.replace(6, 1, QString("0000%1").arg(i).right(1).toLatin1());}

void ConfigSender::setWasteWaterRealTimeCheck(int i)
{sent.replace(7, 1, QString("0000%1").arg(i).right(1).toLatin1());}
*/

void ConfigSender::setWaterLevelLed1Current(int i)
{sent.replace(4, 3, QString("0000%1").arg(i).right(3).toLatin1());}

void ConfigSender::setWaterLevelLed23Current(int i)
{sent.replace(7, 3, QString("0000%1").arg(i).right(3).toLatin1());}

void ConfigSender::setLed1Current(int i)
{sent.replace(10, 3, QString("0000%1").arg(i).right(3).toLatin1());}

void ConfigSender::setLed2Current(int i)
{sent.replace(13, 3, QString("0000%1").arg(i).right(3).toLatin1());}
/*
void ConfigSender::set420mA1(int i)
{sent.replace(20, 1, QString("0000%1").arg(i).right(1).toLatin1());}

void ConfigSender::set420mA2(int i)
{sent.replace(21, 4, QString("0000%1").arg(i).right(4).toLatin1());}
*/
void ConfigSender::setPD1Incred(int i)
{sent.replace(16, 1, QString("0000%1").arg(i).right(1).toLatin1());}

void ConfigSender::setPD2Incred(int i)
{sent.replace(17, 1, QString("0000%1").arg(i).right(1).toLatin1());}

void ConfigSender::setWaterLevel1Threshold(int i)
{sent.replace(26, 4, QString("0000%1").arg(i).right(4).toLatin1());}

void ConfigSender::setWaterLevel2Threshold(int i)
{sent.replace(22, 4, QString("0000%1").arg(i).right(4).toLatin1());}

void ConfigSender::setWaterLevel3Threshold(int i)
{sent.replace(18, 4, QString("0000%1").arg(i).right(4).toLatin1());}

int ConfigSender::step()
{return sent.left(4).toInt();}

/*
void ConfigSender::setTempFixBit(int i)
{sent.replace(27, 1, QString("0000%1").arg(i).right(1).toLatin1());}

void ConfigSender::setTempFixValue(int i)
{sent.replace(28, 4, QString("0000%1").arg(i).right(4).toLatin1());}
*/

int Receiver::check()
{
    if (!recv.startsWith("#"))
        return -1;

    int dataLen = recv.length();
    if (dataLen < PACKET_MIN_LENGTH)
        return 1;

    int len = recv.mid(1,2).toInt();
    if (len < PACKET_MIN_LENGTH)
        return -2;

    if (dataLen < len)
        return 2;

    //    QByteArray ver = recv.mid(3,1);
#ifndef NO_CHECK_STEP
    QByteArray cs = recv.mid(len - 3, 2);
    if (checkSum(recv.left(len - 3)) == cs) {
        return 0;
    }else
        mcuLogger()->error("checksum error :" + cs);
#else
    return 0;
#endif

    return -3;
}

QByteArray Receiver::data()
{
    return recv;
}


int Receiver::step()          {return recv.mid(PACKET_HEAD_LENGTH, 4).toInt();}
int Receiver::stepTime()      {return recv.mid(PACKET_HEAD_LENGTH + 4, 4).toInt();}
int Receiver::extControl1()   {return recv.mid(PACKET_HEAD_LENGTH + 8, 1).toInt();}
int Receiver::extControl2()   {return recv.mid(PACKET_HEAD_LENGTH + 9, 1).toInt();}
int Receiver::extControl3()   {return recv.mid(PACKET_HEAD_LENGTH + 10, 1).toInt();}
int Receiver::_420mA()        {return recv.mid(PACKET_HEAD_LENGTH + 11, 4).toInt();}
int Receiver::pumpStatus()    {return recv.mid(PACKET_HEAD_LENGTH + 15, 1).toInt();}
int Receiver::waterLevel()    {return recv.mid(PACKET_HEAD_LENGTH + 16, 1).toInt();}
float Receiver::heatTemp()      {return recv.mid(PACKET_HEAD_LENGTH + 17, 4).toFloat() / 10.0;}
int Receiver::mcu1Temp()      {return recv.mid(PACKET_HEAD_LENGTH + 21, 2).toInt();}
int Receiver::mcu2Temp()      {return recv.mid(PACKET_HEAD_LENGTH + 23, 2).toInt();}
int Receiver::lightVoltage3() {return recv.mid(PACKET_HEAD_LENGTH + 25, 5).toInt();}
int Receiver::lightVoltage2() {return recv.mid(PACKET_HEAD_LENGTH + 30, 5).toInt();}
int Receiver::lightVoltage1() {return recv.mid(PACKET_HEAD_LENGTH + 35, 5).toInt();}
int Receiver::refLightSignal(){return recv.mid(PACKET_HEAD_LENGTH + 45, 5).toInt();}
int Receiver::measureSignal(){return recv.mid(PACKET_HEAD_LENGTH + 40, 5).toInt();}

void Receiver::setStep(int i)          {recv.replace(PACKET_HEAD_LENGTH, 4, QString("0000%1").arg(i).right(4).toLatin1());}


IProtocol::IProtocol(const QString &portParamter, QObject *parent) :
    QObject(parent),
    timeoutFlag(false),
    newDataFlag(false),
    timeCount(0),
    sendType(0),
    timer(new QTimer(this)),
    port(new QextSerialPort(QextSerialPort::EventDriven, this)),
    counter(new ProtocolCounter(this))
{
    QStringList strlist = portParamter.split(",");
    if (strlist.count() == 5)
    {
        port->setPortName(strlist[0]);
        port->setBaudRate(BaudRateType(strlist[1].toInt()));
        if (strlist[2].toUpper() == "E")
            port->setParity(PAR_EVEN);
        else if (strlist[2].toUpper() == "O")
            port->setParity(PAR_ODD);
        else
            port->setParity(PAR_NONE);
        port->setDataBits(DataBitsType(strlist[3].toInt()));
        port->setStopBits(strlist[4] == "1" ? STOP_1 : STOP_2);

        if (!port->isOpen())
            port->open(QIODevice::ReadWrite);
        else
            port->close();

        //If using polling mode, we need a QTimer
        if (port->isOpen()) {
            if (port->queryMode() == QextSerialPort::Polling)
                timer->start(100);
        } else {
            timer->stop();
            addErrorMsg(QString("上下位机通信端口打开失败%1").arg(port->portName()), 1);
        }
    }
    else
        qDebug() << "parameters error" << portParamter;

    connect(timer, SIGNAL(timeout()), this, SLOT(onReadyRead()));
    connect(port, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
    connect(counter, SIGNAL(timing(bool)), this, SLOT(onCounterTimeout(bool)));
}

IProtocol::~IProtocol()
{
}

bool IProtocol::recvNewData()
{
    if (newDataFlag)
    {
        newDataFlag = false;
        return true;
    }
    return false;
}

void IProtocol::reset()
{
    dataSender = Sender();
    configSender = ConfigSender();
    dataReceiver = Receiver();
    timeoutFlag = false;
    newDataFlag = false;
    timeCount = 0;
    counter->stop();
}

bool IProtocol::portIsOpened()
{
    return port->isOpen();
}


void IProtocol::sendData(const QString &cmd)
{
    if (port->isOpen()) {
        dataSender = Sender(cmd.toLatin1());
        port->write(dataSender.data());
        counter->start(dataSender.stepTime());
        counter->lock();
        timeCount = 0;
        sendType = 0;

        mcuLogger()->info("send:" + dataSender.data());
    }
}

void IProtocol::sendConfig(const ConfigSender &sender)
{
    if (port->isOpen()) {
        configSender = sender;
        port->write(configSender.data());
        counter->start(1);
        counter->lock();
        sendType = 1;

        mcuLogger()->info("conf:" + configSender.data());
    }
}

void IProtocol::skipCurrentStep()
{
    counter->stop();
}

void IProtocol::onReadyRead()
{
    if (port->bytesAvailable())
    {
        recvTemp += port->readAll();

        Receiver tempr = Receiver(recvTemp);
        int ret = tempr.check();

        if (ret < 0)
        {
            recvTemp.clear();
        }
        else if (ret == 0)
        {
            // 只记录工作状态下接收的数据
            if (!isIdle())
                mcuLogger()->info("recv:" + recvTemp);
            recvTemp.clear();

            int step = 0;
            switch (sendType)
            {
            case 0: step = dataSender.step(); break;
            case 1: step = configSender.step(); break;
            }

#ifdef NO_STEP_CHECK
            step = tempr.step();
#endif
            if (tempr.step() == step) {
                dataReceiver = tempr;
                newDataFlag = true;
                timeoutFlag = false;
                counter->unlock();

                emit DataReceived();
            }
        }
    }
}

void IProtocol::onCounterTimeout(bool islast)
{
    if (counter->locked())
    {
        timeCount++;
        int mod = timeCount % 5;
        int times = timeCount / 5;

        if (times >= 5) {
            timeoutFlag = true;
            counter->stop();

            emit ComTimeout();
        }
        else if (mod == 0)
        {
            switch (sendType)
            {
            case 0:
                port->write(dataSender.data());
                mcuLogger()->info("resd:" + dataSender.data());
                break;
            case 1: port->write(configSender.data());
                mcuLogger()->info("resd:" + configSender.data());
                break;
            }
        }
    }
    else if (islast)
        emit ComFinished();
}


ProtocolCounter::ProtocolCounter(QObject *parent) :
    QObject(parent),
    timerid(0),
    counts(0),
    max(0),
    islock(false)
{}

void ProtocolCounter::start(int seconds)
{
    stop();
    timerid = startTimer(1000);
    counts = 0;
    max = seconds;
}

void ProtocolCounter::stop()
{
    islock = false;
    if(timerid)
        killTimer(timerid);
    timerid = 0;
    counts = max;
}

void ProtocolCounter::timerEvent(QTimerEvent *event)
{
    if(event->timerId() == timerid && !islock) {
        counts++;
        if (counts >= max) {
            stop();
            emit timing(true);
            return;
        }
    }
    emit timing(false);
}


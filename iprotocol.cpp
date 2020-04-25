#include "iprotocol.h"
#include <QStringList>
#include <QDebug>
#include "defines.h"
#include "common.h"
#include "profile.h"

extern QString recvComData;


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
int Sender::heatTemp(){return sent.mid(32, 4).toInt();}
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
bool Sender::heatJudgeStep(){return judgeStep() == 5;}
bool Sender::coolReachStep(){return judgeStep() == 6;}
bool Sender::coolJudgeStep(){return judgeStep() == 7;}
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
void Sender::setHeatTemp(int i){sent.replace(32, 4, QString("0000%1").arg(i).right(4).toLatin1());}



ConfigSender::ConfigSender(const QByteArray &src) :
    sent(src)
{}


ConfigSender::ConfigSender()
{
    sent = "000111111001001001000200033118000000";
}

QByteArray ConfigSender::data()
{
    QByteArray length = QString("00%1").arg(sent.length() + PACKET_MIN_LENGTH).right(2).toLatin1();
    QByteArray da = "#" + length + "1" + sent;
    return da + checkSum(da) + '!';
}

void ConfigSender::setWaterLevelAlwaysOn(int i)
{sent.replace(4, 1, QString("0000%1").arg(i).right(1).toLatin1());}

void ConfigSender::setLedAlwaysOn(int i )
{sent.replace(5, 1, QString("0000%1").arg(i).right(1).toLatin1());}

void ConfigSender::setWaterLevelRealTimeCheck(int i)
{sent.replace(6, 1, QString("0000%1").arg(i).right(1).toLatin1());}

void ConfigSender::setWasteWaterRealTimeCheck(int i)
{sent.replace(7, 1, QString("0000%1").arg(i).right(1).toLatin1());}

void ConfigSender::setWaterLevelLed1Current(int i)
{sent.replace(8, 3, QString("0000%1").arg(i).right(3).toLatin1());}

void ConfigSender::setWaterLevelLed23Current(int i)
{sent.replace(11, 3, QString("0000%1").arg(i).right(3).toLatin1());}

void ConfigSender::setLed1Current(int i)
{sent.replace(14, 3, QString("0000%1").arg(i).right(3).toLatin1());}

void ConfigSender::setLed2Current(int i)
{sent.replace(17, 3, QString("0000%1").arg(i).right(3).toLatin1());}

void ConfigSender::set420mA1(int i)
{sent.replace(20, 1, QString("0000%1").arg(i).right(1).toLatin1());}

void ConfigSender::set420mA2(int i)
{sent.replace(21, 4, QString("0000%1").arg(i).right(4).toLatin1());}

void ConfigSender::setPD1Incred(int i)
{sent.replace(25, 1, QString("0000%1").arg(i).right(1).toLatin1());}

void ConfigSender::setPD2Incred(int i)
{sent.replace(26, 1, QString("0000%1").arg(i).right(1).toLatin1());}

void ConfigSender::setTempFixBit(int i)
{sent.replace(27, 1, QString("0000%1").arg(i).right(1).toLatin1());}

void ConfigSender::setTempFixValue(int i)
{sent.replace(28, 4, QString("0000%1").arg(i).right(4).toLatin1());}


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
int Receiver::heatTemp()      {return recv.mid(PACKET_HEAD_LENGTH + 17, 4).toInt();}
int Receiver::mcu1Temp()      {return recv.mid(PACKET_HEAD_LENGTH + 21, 2).toInt();}
int Receiver::mcu2Temp()      {return recv.mid(PACKET_HEAD_LENGTH + 23, 2).toInt();}
int Receiver::lightVoltage1() {return recv.mid(PACKET_HEAD_LENGTH + 25, 5).toInt();}
int Receiver::lightVoltage2() {return recv.mid(PACKET_HEAD_LENGTH + 30, 5).toInt();}
int Receiver::lightVoltage3() {return recv.mid(PACKET_HEAD_LENGTH + 35, 5).toInt();}
int Receiver::measureSignal1(){return recv.mid(PACKET_HEAD_LENGTH + 40, 5).toInt();}
int Receiver::measureSignal2(){return recv.mid(PACKET_HEAD_LENGTH + 45, 5).toInt();}

void Receiver::setStep(int i)          {recv.replace(PACKET_HEAD_LENGTH, 4, QString("0000%1").arg(i).right(4).toLatin1());}


IProtocol::IProtocol(const QString &portParamter, QObject *parent) :
    QObject(parent),
    timeoutFlag(false),
    newDataFlag(false),
    timeCount(0),
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
    connect(counter, SIGNAL(timing()), this, SLOT(onCounterTimeout()));
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
            dataReceiver = tempr;

#ifdef NO_CHECK_STEP
            dataReceiver.setStep(dataSender.step());
#endif
            if (dataReceiver.step() == dataSender.step()) {
                newDataFlag = true;
                timeoutFlag = false;
                counter->unlock();
            }

            // 只记录工作状态下接收的数据
            if (!isIdle())
                mcuLogger()->info("recv:" + recvTemp);

            recvComData = dataReceiver.data();
            recvTemp.clear();
        }
        else if (ret > 0)
        {
        }
    }
}

void IProtocol::onCounterTimeout()
{
    if (counter->locked())
    {
        timeCount++;
        int mod = timeCount % 5;
        int times = timeCount / 5;

        if (times >= 5) {
            timeoutFlag = true;
            counter->stop();
        }
        else if (mod == 0)
        {
            port->write(dataSender.data());
            mcuLogger()->info("resd:" + dataSender.data());
        }
    }
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
        }
    }
    emit timing();
}


#include "iprotocol.h"
#include <QStringList>
#include <QDebug>
#include "defines.h"

#define PACKET_MAX_LENGTH  7
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

    QByteArray da = "#" + QString("00%1").arg(index + PACKET_MAX_LENGTH).right(2).toLatin1() + "1" + sent.left(index);
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
int Sender::extValve(){return sent.mid(21, 1).toInt();}
int Sender::extControl1(){return sent.mid(22, 1).toInt();}
int Sender::extControl2(){return sent.mid(23, 1).toInt();}
int Sender::extControl3(){return sent.mid(24, 1).toInt();}
int Sender::fun(){return sent.mid(25, 1).toInt();}
int Sender::waterLevel(){return sent.mid(26, 1).toInt();}
int Sender::heatTemp(){return sent.mid(27, 4).toInt();}

int Sender::timeFix(){return sent.mid(36, 2).toInt();}
int Sender::timeAddFix(){return sent.mid(38, 4).toInt();}
int Sender::tempFix(){return sent.mid(42, 2).toInt();}
int Sender::loopFix(){return sent.mid(44, 2).toInt();}
bool Sender::waterLevelReachStep(){return sent.mid(46, 1).toInt() == 1;}
bool Sender::waterLevelJudgeStep(){return sent.mid(46, 1).toInt() == 2;}
bool Sender::heatReachStep(){return sent.mid(46, 1).toInt() == 3;}
bool Sender::heatJudgeStep(){return sent.mid(46, 1).toInt() == 4;}
bool Sender::coolReachStep(){return sent.mid(46, 1).toInt() == 5;}
bool Sender::coolJudgeStep(){return sent.mid(46, 1).toInt() == 6;}
bool Sender::blankStep(){return sent.mid(47, 1).toInt() == 1;}
bool Sender::colorStep(){return sent.mid(47, 1).toInt() == 2;}
int Sender::explainCode(){return sent.mid(48, 2).toInt();}

void Sender::setStep(int i){sent.replace(0, 4, QString("0000%1").arg(i).right(4).toLatin1());}
void Sender::setStepTime(int i){sent.replace(4, 4, QString("0000%1").arg(i).right(4).toLatin1());}
void Sender::setPeristalticPump(int i){sent.replace(8, 1, QString("0000%1").arg(i).right(1).toLatin1());}
void Sender::setPeristalticPumpSpeed(int i){sent.replace(9, 2, QString("0000%1").arg(i).right(2).toLatin1());}
void Sender::setPump2(int i){sent.replace(11, 1, QString("0000%1").arg(i).right(1).toLatin1());}
void Sender::setTCValve1(int i){sent.replace(12, 1, QString("0000%1").arg(i).right(1).toLatin1());}
void Sender::setTCValve2(int i){sent.replace(13, 1, QString("0000%1").arg(i).right(1).toLatin1());}
void Sender::setValve1(int i){sent.replace(14, 1, QString("0000%1").arg(i).right(1).toLatin1());}
void Sender::setValve2(int i){sent.replace(15, 1, QString("0000%1").arg(i).right(1).toLatin1());}
void Sender::setValve3(int i){sent.replace(16, 1, QString("0000%1").arg(i).right(1).toLatin1());}
void Sender::setValve4(int i){sent.replace(17, 1, QString("0000%1").arg(i).right(1).toLatin1());}
void Sender::setValve5(int i){sent.replace(18, 1, QString("0000%1").arg(i).right(1).toLatin1());}
void Sender::setValve6(int i){sent.replace(19, 1, QString("0000%1").arg(i).right(1).toLatin1());}
void Sender::setValve7(int i){sent.replace(20, 1, QString("0000%1").arg(i).right(1).toLatin1());}
void Sender::setExtValve(int i){sent.replace(21, 1, QString("0000%1").arg(i).right(1).toLatin1());}
void Sender::setExtControl1(int i){sent.replace(22, 1, QString("0000%1").arg(i).right(1).toLatin1());}
void Sender::setExtControl2(int i){sent.replace(23, 1, QString("0000%1").arg(i).right(1).toLatin1());}
void Sender::setExtControl3(int i){sent.replace(24, 1, QString("0000%1").arg(i).right(1).toLatin1());}
void Sender::setFun(int i){sent.replace(25, 1, QString("0000%1").arg(i).right(1).toLatin1());}
void Sender::setWaterLevel(int i){sent.replace(26, 1, QString("0000%1").arg(i).right(1).toLatin1());}
void Sender::setHeatTemp(int i){sent.replace(27, 4, QString("0000%1").arg(i).right(4).toLatin1());}






int Receiver::check()
{
    if (!recv.startsWith("#"))
        return -1;

    int dataLen = recv.length();
    if (dataLen < PACKET_MAX_LENGTH)
        return 1;

    int len = recv.mid(1,2).toInt();
    if (len < PACKET_MAX_LENGTH)
        return -2;

    if (dataLen < len)
        return 2;

    //    QByteArray ver = recv.mid(3,1);
    QByteArray cs = recv.mid(len - 3, 2);
    if (checkSum(recv.left(len - 3)) == cs) {
        return 0;
    }else
        mcuLogger()->error("checksum error :" + cs);
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
int Receiver::waterLevel()    {return recv.mid(PACKET_HEAD_LENGTH + 15, 1).toInt();}
int Receiver::heatTemp()      {return recv.mid(PACKET_HEAD_LENGTH + 16, 4).toInt();}
int Receiver::mcu1Temp()      {return recv.mid(PACKET_HEAD_LENGTH + 20, 3).toInt();}
int Receiver::mcu2Temp()      {return recv.mid(PACKET_HEAD_LENGTH + 23, 3).toInt();}
int Receiver::lightVoltage1() {return recv.mid(PACKET_HEAD_LENGTH + 26, 5).toInt();}
int Receiver::lightVoltage2() {return recv.mid(PACKET_HEAD_LENGTH + 31, 5).toInt();}
int Receiver::lightVoltage3() {return recv.mid(PACKET_HEAD_LENGTH + 36, 5).toInt();}
int Receiver::measureSignal1(){return recv.mid(PACKET_HEAD_LENGTH + 41, 5).toInt();}
int Receiver::measureSignal2(){return recv.mid(PACKET_HEAD_LENGTH + 46, 5).toInt();}



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
            qDebug() << "com port open failed";
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
    timeoutFlag = false;
    newDataFlag = false;
    timeCount = 0;
    counter->stop();
}


void IProtocol::sendData(const QString &cmd)
{
    if (port->isOpen()) {
        dataSender = Sender(cmd.toLatin1());
        port->write(dataSender.data());
        counter->start(dataSender.stepTime());
        counter->lock();
        timeCount = 0;
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
            if (dataReceiver.step() == dataSender.step()) {
                newDataFlag = true;
                timeoutFlag = false;
                counter->unlock();
            }
            mcuLogger()->info("receiver:" + recvTemp);
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
        }
    }
}


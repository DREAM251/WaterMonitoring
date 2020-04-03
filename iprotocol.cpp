#include "iprotocol.h"
#include <QStringList>
#include <QDebug>

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

int Sender::getStep(){return sent.left(4).toInt();}
int Sender::getStepTime(){return sent.mid(4, 4).toInt();}
int Sender::getHeatTemp(){return 0;}
int Sender::getLightVoltage(){return 0;}
void Sender::setTime(int seconds){sent.replace(4,4, QString("0000%1").arg(seconds).right(4).toLatin1());}
void Sender::setTemp(int temp){}

int Sender::timeFix(){return sent.mid(8, 2).toInt();}
int Sender::tempFix(){return sent.mid(10, 2).toInt();}
int Sender::loopFix(){return sent.mid(12, 2).toInt();}

bool Sender::isBlankStep(){return false;}
bool Sender::isColorStep(){return false;}
bool Sender::isHeatStep(){return false;}
bool Sender::isBlankJudgeStep(){return false;}
bool Sender::isHeatJudgeStep(){return false;}
bool Sender::isWaterLevelJudgeStep(){return false;}





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
        qDebug() << "checksum error " << cs;
    return -3;
}

QByteArray Receiver::data()
{
    return recv;
}

int Receiver::getStep(){return recv.mid(PACKET_HEAD_LENGTH, 4).toInt();}
int Receiver::getStepTime(){return recv.mid(PACKET_HEAD_LENGTH + 4, 4).toInt();}
int Receiver::getHeatTemp(){return 0;}
int Receiver::getWaterLevel(){return 0;}
int Receiver::getLightVoltage() {return 0;}






IProtocol::IProtocol(const QString &portParamter) :
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
        if (strlist[2] == "N")
            port->setParity(PAR_EVEN);
        else if (strlist[2] == "O")
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
        counter->start(dataSender.getStepTime());
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
            recvTemp.clear();
            if (dataReceiver.getStep() == dataSender.getStep()) {
                newDataFlag = true;
                timeoutFlag = false;
                counter->unlock();
            }
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


#include "iprotocol.h"
#include <QStringList>
#include <QDebug>

#define HEAD_LENGTH   7

QByteArray checkSum(const QByteArray &by)
{
    int sum = 0;
    for (int i = 0; i < by.count(); i++)
        sum += (uint)(by[i]);

    return QByteArray(1, (char)(sum % 256)).toHex().toUpper();
}


IProtocol::IProtocol(const QString &portParamter) :
    timeoutFlag(false),
    newDataFlag(false),
    timeCount(0),
    timer(new QTimer(this)),
    port(new QextSerialPort(QextSerialPort::EventDriven, this)),
    counter(new ProtocolCounter(this)),
    sender(new Sender()),
    receiver(new Receiver())
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
    delete sender;
    delete receiver;
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


QByteArray IProtocol::addHeader(const QByteArray &src)
{
    QByteArray h = "#";
    QByteArray len = (QString("000") + QString::number(src.length())).right(3).toLatin1();
    QByteArray cs = checkSum(QByteArray("1" + src));
    QByteArray ver = "1";
    return h + len + cs + ver + src;
}

void IProtocol::sendData(const QString &cmd)
{
    if (port->isOpen()) {
        sender->sent = cmd.toLatin1();
        port->write(addHeader(sender->sent));
        counter->start(sender->getStepTime());
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
    const int headerMaxLen = HEAD_LENGTH;
    if (port->bytesAvailable())
    {
        QByteArray recvPart = port->readAll();
        QByteArray h = "#";
        QByteArray ver;
        QByteArray len;
        QByteArray cs;
        QByteArray sec;

        if (recvPart.startsWith(h))
            recvTemp = recvPart;
        else
        {
            if (recvTemp.startsWith(h))
                recvTemp += recvPart;
            else {
                recvTemp.clear();
                return;
            }
        }

        if (recvTemp.length() > headerMaxLen)
        {
            len = recvTemp.mid(1,3);
            cs = recvTemp.mid(4,2);
            ver = recvTemp.mid(6,1);

            int packetLen = len.toInt() + HEAD_LENGTH;
            if (packetLen > recvTemp.length())
                return;
            else if (packetLen < recvTemp.length()) {
                sec = recvTemp.left(packetLen);
                recvTemp = recvTemp.right(packetLen);
                if (!recvTemp.startsWith(h))
                    recvTemp.clear();
            }
            else {
                sec = recvTemp;
                recvTemp.clear();
            }

            if (checkSum(sec.mid(headerMaxLen - 1)) == cs) {
                receiver->recv = sec.mid(headerMaxLen);
                if (receiver->getStep() == sender->getStep()) {
                    newDataFlag = true;
                    timeoutFlag = false;
                    counter->unlock();
                }
            } else
                qDebug() << "checksum error " << checkSum(sec.right(headerMaxLen - 1)) << cs;
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
            port->write(addHeader(sender->sent));
        }
    }
}

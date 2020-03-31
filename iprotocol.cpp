#include "iprotocol.h"
#include <QStringList>

IProtocol::IProtocol(const QString &portParamter) :
    timer(new QTimer(this)),
    port(new QextSerialPort(QextSerialPort::EventDriven, this)),
    counter(new Counter(this)),
    timeoutCounter(new Counter(this))
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
            qDebug() << __func__ << ": com port open failed";
        }
    }
    else
        qDebug() << __func__ << ": parameters error" << portParamter;

    connect(timer, SIGNAL(timeout()), SLOT(onReadyRead()));
    connect(port, SIGNAL(readyRead()), SLOT(onReadyRead()));
}

void IProtocol::recvNewData()
{
    if (newDataFlag)
    {
        newDataFlag = false;
        return true;
    }
    return false;
}

void IProtocol::isIdle()
{
    counter->isIdle();
}

void IProtocol::sendData(const QString &cmd)
{
    if (port->isOpen()) {
        sent = cmd.toLatin1();
        port->write(sent);
        timeoutCounter->start(5);
    }
}

void IProtocol::skipCurrentStep()
{
    counter->stop();
}

QByteArray IProtocol::checkSum(const QByteArray &by)
{
    int sum = 0;
    for (int i = 0; i < by.count(); i++)
        sum += (uint)(by[i]);

    return QByteArray(1, (char)(sum % 256)).toHex().toUpper();
}

void IProtocol::onReadyRead()
{
    int headerMaxLen = 7;
    if (port->bytesAvailable())
    {
        QByteArray recvPart = port->readAll();

        if (recvPart.startsWith("#"))
            recvTemp = recvPart;
        else
        {
            if (recvTemp.startsWith("#"))
                recvTemp += recvPart;
            else {
                recvTemp.clear();
                return;
            }
        }

        if (recvTemp.length() > headerMaxLen)
        {
            int packetLen = recvTemp.mid(1,3).toInt();
            if (packetLen <= recvTemp.length()) {
                recvTemp = recvTemp.left(packetLen);
                if (checkSum(recvTemp.right(headerMaxLen)) == recvTemp.mid(4,2)) {
                    recv = recvTemp;
                    newDataFlag = true;
                } else
                    qDebug() << __func__ << "checksum error" << recvTemp;
            }
        }
    }
}

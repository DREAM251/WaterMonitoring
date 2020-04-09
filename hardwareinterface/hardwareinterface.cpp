#include "hardwareinterface.h"
#include <QtNetwork/QTcpSocket>
#include "qextserialport-1.2rc/qextserialport.h"
#include <QTimer>
//#include "src/Trace.h"
//#include "src/Common.h"

#define TraceDebugPrint

//VSerialPort::VSerialPort(QString portName, quint32 baudrate, quint16 databit, quint8 parity, quint16 stopbit)
//{
//    TraceDebugPrint;
//    VSerialPortPar.port     = portName;
//    VSerialPortPar.baud     = baudrate;
//    VSerialPortPar.databit  = databit;
//    VSerialPortPar.parity   = parity;
//    VSerialPortPar.stopbit  = stopbit;

//    myCom = new QextSerialPort;
//    connect(myCom, SIGNAL(readyRead()), this, SLOT(readMyCom()));
//    connect(myCom, SIGNAL(readyRead()), this, SIGNAL(readyRead()));

//        serialPort = new ManageSerialPort;
//        connect(serialPort,SIGNAL(newDataReceived(const QByteArray&)),this,SIGNAL(newDataReceived(const QByteArray &)));
//}

VSerialPort::VSerialPort()
{
    TraceDebugPrint;
    VSerialPortPar.port     = "COM1";
    VSerialPortPar.baud     = 9600;
    VSerialPortPar.databit  = 8;
    VSerialPortPar.parity   = 'N';
    VSerialPortPar.stopbit  = 1;

    myCom = new QextSerialPort;

    QTimer *timer = new QTimer(this);
    timer->start(10);
    connect(timer, SIGNAL(timeout()), this, SLOT(readMyCom()));
    connect(myCom, SIGNAL(readyRead()), this, SLOT(readMyCom()));
    //connect(myCom, SIGNAL(readyRead()), this, SIGNAL(readyRead()));

}

VSerialPort::~VSerialPort()
{
    TraceDebugPrint;
    //delete serialPort;
   delete myCom;
}

#if 0
bool VSerialPort::openHardwareInterface()
{
    TraceDebugPrint;
    qDebug()<<"open state:"<<serialPort->isOpen()<<endl;
    if(serialPort->isOpen()){
        serialPort->close();
    }


    serialPort->setPort(VSerialPortPar.port);
    switch(VSerialPortPar.baud){
    case 38400:
        serialPort->setBaudRate(BAUD38400);
        break;
    case 115200:
        serialPort->setBaudRate(BAUD115200);
        break;
    default:
        serialPort->setBaudRate(BAUD9600);
        break;
    }
    switch(VSerialPortPar.databit){
    case 5:
        serialPort->setDataBits(DATA_5);
        break;
    case 6:
        serialPort->setDataBits(DATA_6);
        break;
    case 7:
        serialPort->setDataBits(DATA_7);
        break;
    default:
        serialPort->setDataBits(DATA_8);
        break;
    }
    switch(VSerialPortPar.parity){
    case 'o':
    case 'O':
        serialPort->setParity(PAR_ODD);
        break;
    case 'e':
    case 'E':
        serialPort->setParity(PAR_EVEN);
        break;
    default:
        serialPort->setParity(PAR_NONE);
        break;
    }
    switch(VSerialPortPar.stopbit){
    case 2:
        serialPort->setStopBits(STOP_2);
        break;
    default:
        serialPort->setStopBits(STOP_1);
        break;
    }
    serialPort->setFlowControl(FLOW_OFF);
    serialPort->setTimeout(0, 100);
    if(!serialPort->open())
    {
        qDebug()<<"open is error"<<endl;
        return false;
    }
     qDebug()<<"open is ok"<<endl;
     serialPort->disableSending();
    serialPort->enableSending();
    serialPort->disableReceiving();
    serialPort->enableReceiving();
    serialPort->receiveData();

    return true;
}
bool VSerialPort::closeHardwareInterface()
{
    TraceDebugPrint;
    if(NULL != serialPort)
    {
        if(serialPort->isOpen())
        {
            serialPort->close();
        }
    }
    return true;
}
void VSerialPort::sendData(QByteArray & data)
{
    TraceDebugPrint;
    serialPort->sendData(data);
}
#endif
///////////////////////////////////////////////////////////////////////////////////////

bool VSerialPort::openHardwareInterface()
{
    TraceDebugPrint;
    if(myCom->isOpen()){
        myCom->close();
    }
    myCom->setPortName(VSerialPortPar.port);
    switch(VSerialPortPar.baud){
    case 38400:
        myCom->setBaudRate(BAUD38400);
        break;
    case 115200:
        myCom->setBaudRate(BAUD115200);
        break;
    default:
        myCom->setBaudRate(BAUD9600);
        break;
    }
    switch(VSerialPortPar.databit){
    case 5:
        myCom->setDataBits(DATA_5);
        break;
    case 6:
        myCom->setDataBits(DATA_6);
        break;
    case 7:
        myCom->setDataBits(DATA_7);
        break;
    default:
        myCom->setDataBits(DATA_8);
        break;
    }
    switch(VSerialPortPar.parity){
    case 'o':
    case 'O':
        myCom->setParity(PAR_ODD);
        break;
    case 'e':
    case 'E':
        myCom->setParity(PAR_EVEN);
        break;
    default:
        myCom->setParity(PAR_NONE);
        break;
    }
    switch(VSerialPortPar.stopbit){
    case 2:
        myCom->setStopBits(STOP_2);
        break;
    default:
        myCom->setStopBits(STOP_1);
        break;
    }
    myCom->setFlowControl(FLOW_OFF);
    myCom->setTimeout(100);
    if(!myCom->open(QIODevice::ReadWrite))
    {
        return false;
    }
    return true;
}

bool VSerialPort::closeHardwareInterface()
{
    TraceDebugPrint;
    if( NULL != myCom )
    {
        if(myCom->isOpen())
        {
            myCom->close();
        }
    }
    return true;
}
void VSerialPort::sendData(QByteArray & data)
{
    TraceDebugPrint;
    myCom->write(data);
}

void VSerialPort::setPortName(const QString &name)
{
    VSerialPortPar.port     = name;
}

void VSerialPort::setBaudRate(VSerialPort::BaudRate baud)
{
    VSerialPortPar.baud     = baud;
}

void VSerialPort::setDataBits(VSerialPort::DataBits data)
{
    VSerialPortPar.databit  = data;
}

void VSerialPort::setParity(VSerialPort::Parity parity)
{
    VSerialPortPar.parity   = parity;
}

void VSerialPort::setStopBits(VSerialPort::StopBits stop)
{
    VSerialPortPar.stopbit  = stop;
}

void VSerialPort::setFlowControl(VSerialPort::FlowControl)
{

}

bool VSerialPort::isOpen()
{
    return myCom->isOpen();
}

qint64 VSerialPort::write(const QByteArray &data)
{
    return myCom->write(data);
}

qint64 VSerialPort::read(char *data, qint64 max)
{
    return myCom->read(data, max);
}

bool VSerialPort::bytesAvailable()
{
    return myCom->bytesAvailable();
}

QByteArray VSerialPort::readAll()
{
    return myCom->readAll();
}

void VSerialPort::readMyCom()
{
    if (myCom && myCom->bytesAvailable() > 0)
        emit readyRead();
}
//////////////////////////////////////////////////////////////////////////


VTcpPort::VTcpPort(QString hostName, quint16 port):hostIpAddr(hostName),hostPort(port)
{
    mBufferRecv= new uchar[1024];
    mTcpSocket = new QTcpSocket(this);
    connect(mTcpSocket,SIGNAL(stateChanged(QAbstractSocket::SocketState)),
            this,SIGNAL(sigTcpStateChanged(QAbstractSocket::SocketState)));
    connect(mTcpSocket,SIGNAL(readyRead()),this,SLOT(slotTcpNewDataReceived()));
}

VTcpPort::~VTcpPort()
{
    delete []mBufferRecv;
    delete mTcpSocket;
}

bool VTcpPort::openHardwareInterface()
{
    mTcpSocket->connectToHost(hostIpAddr, hostPort);
    return true;
}

bool VTcpPort::closeHardwareInterface()
{
    mTcpSocket->abort();
    return true;
}

void VTcpPort::sendData(QByteArray & data)
{
    int ret = 0;
    ret = mTcpSocket->write(data);
    if (ret < 0)
        qDebug()<<Q_FUNC_INFO<<":"<<"tcp senddata error"<<endl;
}
void VTcpPort::slotTcpNewDataReceived()
{
    int length = mTcpSocket->read( (char*)&mBufferRecv[0], 1024 );
    QByteArray receiveData = QByteArray( (char*)mBufferRecv, length );
    emit newDataReceived(receiveData);
}








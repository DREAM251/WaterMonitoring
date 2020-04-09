#include "interface.h"

InterfaceSerial::InterfaceSerial() :
    Interface()
{
    serialPort = new VSerialPort();
}

InterfaceSerial::~InterfaceSerial()
{
    Close();
    delete serialPort;
}

// 打开串口，成功返回>0的值
int InterfaceSerial::Open()
{
    if( serialPort->open())
        return 1;
    else{
//        _DEBUG_("open port failed!");
        return 0;
    }
}

// 关闭串口
void InterfaceSerial::Close()
{
    serialPort->close();
}

// 读取一个数据包
int InterfaceSerial::Read(char *p)
{
    int l =  serialPort->bytesAvailable() , unrecv_count = 0 , length = 0;
    if( l<=0 )
        return 0;

    length = serialPort->read(p , 256);
    do{
        sleep_ms(1);
        l =  serialPort->bytesAvailable() ;
        if( l<=0 )
            unrecv_count++;
        else{
            length += serialPort->read(p+length , 256-length);
            unrecv_count=0;
        }
    }while(unrecv_count<MESSAGE_FINISH_TIME && length<256);

    return length;
}

// 发送一个数据包
int InterfaceSerial::Write(char *by , int len)
{
    QByteArray byd(by, len);
    return serialPort->write(byd);
}

// 串口设置
void InterfaceSerial::setPortName(QString name)
{
    serialPort->setPortName( name );
}

void InterfaceSerial::setBaudRate(const QString BaudRateType )
{
    if(BaudRateType == "38400")
        serialPort->setBaudRate(VSerialPort::Baud38400);
    else if(BaudRateType == "115200")
        serialPort->setBaudRate(VSerialPort::Baud115200);
    else
        serialPort->setBaudRate(VSerialPort::Baud9600);
}

void InterfaceSerial::setDataBits(const QString DataBitsType)
{
    if(DataBitsType == "Data5")
        serialPort->setDataBits(VSerialPort::Data5);
    else if(DataBitsType == "Data6")
        serialPort->setDataBits(VSerialPort::Data6);
    else if(DataBitsType == "Data7")
        serialPort->setDataBits(VSerialPort::Data7);
    else
        serialPort->setDataBits(VSerialPort::Data8);
}

void InterfaceSerial::setParity(const QString ParityType)
{
    if (ParityType == "EvenParity")
        serialPort->setParity(VSerialPort::EvenParity);
    else if (ParityType == "OddParity")
        serialPort->setParity(VSerialPort::OddParity);
    else
        serialPort->setParity(VSerialPort::NoParity);
}

void InterfaceSerial::setStopBits(const QString StopBitsType)
{
    if (StopBitsType == "TwoStop")
        serialPort->setStopBits(VSerialPort::TwoStop);
    else
        serialPort->setStopBits(VSerialPort::OneStop);
}

void InterfaceSerial::setFlowControl(const QString FlowType)
{
    if(FlowType == "HardwareControl")
        serialPort->setFlowControl(VSerialPort::HardwareControl);
    else if(FlowType == "SoftwareControl")
        serialPort->setFlowControl(VSerialPort::SoftwareControl);
    else
        serialPort->setFlowControl(VSerialPort::NoFlowControl);
}

void InterfaceSerial::setTimeout(ulong, ulong )
{
    //serialPort->setTimeout(x,y);
}

void InterfaceSerial::flush()
{
    //serialPort->readAll();
    //serialPort->flush();
}

bool InterfaceSerial::IsOpen()
{
    return serialPort->isOpen();
}

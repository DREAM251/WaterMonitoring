
#ifndef __MASTERINTERFACE_H_
#define __MASTERINTERFACE_H_

#include <QObject>
#include <QTimer>
#include <QByteArray>
#include "mbdefines.h"

// 通信接口，仅提供模板
class Interface 
{
protected:
    Interface(){}
    ~Interface(){}

    virtual int Open()=0;
    virtual void Close()=0;
    virtual int Read(char *) = 0;
    virtual int Write(char *, int len) = 0;
};

#if defined(MODBUS_INTERFACE_SERIAL)
#include <hardwareinterface.h>
//#include <qextserialport/qextserialport.h>

// 串口接口，用于MODBUS RTU和ASCII
class InterfaceSerial : public Interface
{
public:
    InterfaceSerial();
    ~InterfaceSerial();

    QByteArray bySend , byRecv;
    // 打开串口，成功返回>0的值
    int Open();
    // 关闭串口
    void Close();
    // 读取一个数据包
    int Read(char *);
    // 发送一个数据包
    int Write(char *, int len);

    // 串口的设置，不想说明
    void setPortName(QString name);
    void setBaudRate(const QString BaudRateType);
    void setDataBits(const QString DataBitsType);
    void setParity(const QString ParityType);
    void setStopBits(const QString StopBitsType);
    void setFlowControl(const QString FlowType);
    void setTimeout(ulong, ulong);
    void flush();
    bool IsOpen();

protected:
    VSerialPort *serialPort;//串口模块的实例(第三方)
};
#endif

#if defined(MODBUS_INTERFACE_TCPIP)
#include <QTcpSocket>

// TCP接口，用于MODBUS TCP协议（未实现）
class InterfaceTCP : public QTcpSocket, 
    public Interface
{
public:
    InterfaceTCP();
    ~InterfaceTCP();

protected:
    int Open(){return 0;}
    void Close(){;}
    int Read(char *){return 0;}
    int Write(char *, int len){return 0;}
};
#endif

#endif // MASTERINTERFACE_H


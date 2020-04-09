
#ifndef __NTMODBUSRTU_H_
#define __NTMODBUSRTU_H_

#include "mbprotocol.h"
#include "interface.h"
#include <qtimer.h>

//Modbus从机
class NTModbusSlave : public QObject , public InterfaceSerial ,
        public RTUSlaveProtocol
{
    Q_OBJECT

public:
    NTModbusSlave(QObject *parent=NULL);
    ~NTModbusSlave();

    int Start(); // 打开串口并启动监听,成功返回值>0
    void Stop(); // 停止监听
    bool IsReady(); // 是否准备就绪
    void SendData(char *, int len);

protected:
    int comErrorCode;//接收到的错误代码
    QTimer *timer;
    //接收的临时数据指针和长度
    char *pTemp ;
    int length;

    //接收到数据,error_code异常代码
    virtual void DataReceiveFinished(int error_code);
    int Responds();// 接收到数据后的应答

private slots:
    void readyRead();
    void timeout();

Q_SIGNALS:
    void SendFinish();
    void RecvFinish();
};

//Modbus主机
class NTModbusMaster : public QObject ,
        public InterfaceSerial ,
        public RTUMasterProtocol
{
    Q_OBJECT

public:
    NTModbusMaster(QObject *parent = 0);
    ~NTModbusMaster();

    //启动和停止进程
    void Stop();
    int Start();
    // 是否准备就绪
    bool IsReady();
    int SendData(char *, int len);

protected:
    //接收数据的指针和长度
    char pTemp [MAX_DATA_LENGTH] ;
    int length;
    QTimer *startflag;

    //接收完成,error_code异常代码
    virtual void termination(int error_code);
    //void run();
public:
    //读取线圈
    int GetCoilStatus(unsigned short fregister , unsigned short count);
    //设置线圈
    int SetCoilStatus(unsigned short fregister , std::vector<bool> &values) ;
    //读取输入状态
    int GetInputStatus(unsigned short fregister , unsigned short count) ;
    //读取输入寄存器
    int GetInputRegister(unsigned short fregister , unsigned short count) ;
    //读取保持寄存器
    int GetHoldRegister(unsigned short fregister , unsigned short count) ;
    //设置保持寄存器
    int SetHoldRegister(unsigned short fregister , std::vector<unsigned short> &values) ;
    int SetHoldRegister(unsigned short fregister , unsigned short values) ;

private:
    //发送数据并启动进程监听从机应答
    int SendAndListen();

private slots:
    void timeout();
    void readyRead();

Q_SIGNALS:
    void SendFinish();
};

#endif




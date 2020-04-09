#include "ntmodbus.h"

//===========================================================================
//ModbusSlave
//Modbus从机,添加收发的功能
//===========================================================================
NTModbusSlave::NTModbusSlave(QObject *parent) :  QObject(parent) ,
    InterfaceSerial() ,
    RTUSlaveProtocol()
{
    length = 0;
    pTemp = new char[MAX_DATA_LENGTH];
    comErrorCode = NO_EXCEPTION;
    timer = new QTimer();
    connect(timer,SIGNAL(timeout()),this,SLOT(timeout()));
    connect(serialPort,SIGNAL(readyRead()),this,SLOT(readyRead()));
}

NTModbusSlave::~NTModbusSlave()
{
    Stop();
}

// 停止监听并关闭串口
void NTModbusSlave::Stop()
{
    Close();
}

bool NTModbusSlave::IsReady()
{
    return IsOpen();
}

void NTModbusSlave::SendData(char *pdata, int len)
{
    bySend = QByteArray(pdata,len);
    Write(pdata,len);

    emit SendFinish();
}

void NTModbusSlave::DataReceiveFinished(int error_code)
{
    comErrorCode = error_code;

    byRecv = QByteArray(pTemp,length);
    if(timer->isActive())
        timer->stop();
    length = 0;

    emit RecvFinish();
}

// 打开串口并启动监听,成功返回值>0
int NTModbusSlave::Start()
{
    int ret = 1;
    if(IsOpen())
        Close();

    if(!Open())
        ret = 0;

    return ret ;
}

// 应答
int NTModbusSlave::Responds()
{
    unsigned char *p;

    // 预处理 -- ASCII->HEX
    int l =0 , r = Pretreatment((unsigned char*)pTemp , length);
    if(r > 0){
        length = r;
        // 数据处理
        r = ProcessMessage((unsigned char*)pTemp , length );

        if(r == 0)
        {
            RWLocalData();
            if(GetLastException() == NO_EXCEPTION)
            {
                p = BuildMessage(&l);
            }
            else
                p = BuildExceptionMessage(&l);
            if(l > 0)
                SendData((char *)p , l);
            DataReceiveFinished(r);
        }
    }

    return 0;
}

void NTModbusSlave::readyRead()
{
//    qDebug() << length;
    if(length<0 || length >= 256)
        return;

    timer->start(50);
    int count = serialPort->read(pTemp+length,256-length);
    length += count;
    if(length>0)
        Responds();
}

void NTModbusSlave::timeout()
{
    DataReceiveFinished(COMM_TIME_OUT);
}

//===========================================================================
//ModbusRTUMaster
//RTU主机,添加收发的功能
//===========================================================================
NTModbusMaster::NTModbusMaster(QObject *parent)  :
    QObject(parent) ,
    InterfaceSerial() ,
    RTUMasterProtocol()
{
    length = 0;
    startflag = new QTimer;
    connect(serialPort,SIGNAL(readyRead()),this,SLOT(readyRead()));
    connect(startflag,SIGNAL(timeout()),this,SLOT(timeout()));
}

NTModbusMaster::~NTModbusMaster()
{
    Stop();
}

//结束进程并关闭端口
void NTModbusMaster::Stop()
{
    //Close();
    this->termination(0);
}

//启动接收数据的进程
int NTModbusMaster::Start()
{
    if( !IsOpen() )
        return 0;

    startflag->start(2000);
    return 1;
}

bool NTModbusMaster::IsReady()
{
    return IsOpen() && (!startflag->isActive());
}

int NTModbusMaster::SendData(char *pdata, int len)
{
    bySend = QByteArray(pdata,len);
    //qDebug() << "Modbus Send:" << bySend.toHex();
    emit SendFinish();
    return Write(pdata , len);
}

void NTModbusMaster::termination(int error_code)
{
    _UNUSED_(error_code);

    if(startflag->isActive())
        startflag->stop();
}

//向从机发送一条读取线圈的指令
int NTModbusMaster::GetCoilStatus(unsigned short fregister , unsigned short count)
{
//    if(IsRunning())
//        return 0;
    GetRegister(fregister , count , FC_READ_COIL_STATUS);
    return SendAndListen();
}

//向从机发送一条设置线圈的指令
int NTModbusMaster::SetCoilStatus(unsigned short fregister , std::vector<bool> &values)
{
//    if(IsRunning())
//        return 0;

    std::vector<unsigned short> va;
    int count  = values.size();
    if(count == 1){
        if(values.at(0))
            va.push_back(UINT8TOUINT16(0xFF , 0x00));
        else
            va.push_back(0x0000);

        SetRegister(fregister , count , va , FC_FORCE_SINGLE_COIL);
    }else{
        Bits2Bytes(values , count , va);
        SetRegister(fregister , count, va , FC_FORCE_MULTIPLE_COILS);
    }
    return SendAndListen();
}

//向从机发送一条设置线圈的指令
int NTModbusMaster::GetInputStatus(unsigned short fregister , unsigned short count)
{
//    if(IsRunning())
//        return 0;
    GetRegister(fregister , count , FC_READ_INPUT_STATUS);
    return SendAndListen();
}

//向从机发送一条设置线圈的指令
int NTModbusMaster::GetInputRegister(unsigned short fregister , unsigned short count)
{
//    if(IsRunning())
//        return 0;
    GetRegister(fregister , count , FC_READ_INPUT_REGISTERS);
    return SendAndListen();
}

//向从机发送一条读取保持寄存器的指令
int NTModbusMaster::GetHoldRegister(unsigned short fregister , unsigned short count)
{
//    if(IsRunning())
//        return 0;
    GetRegister(fregister , count , FC_READ_HOLDING_REGISTERS);
    return SendAndListen();
}

//向从机发送一条设置保持寄存器的指令
int NTModbusMaster::SetHoldRegister(unsigned short fregister , std::vector<unsigned short> &values)
{
//    if(IsRunning())
//        return 0;
    int count = values.size() ;
     if( count == 1)
         SetRegister(fregister ,count, values , FC_PRESET_SINGLE_REGISTER);
     else
         SetRegister(fregister ,count, values , FC_PRESET_MULTIPLE_REGISTERS);

     return SendAndListen();
}

int NTModbusMaster::SetHoldRegister(unsigned short fregister , unsigned short value)
{
    std::vector<unsigned short> values(1,value);
    return SetHoldRegister(fregister ,values);
}

//发送数据并启动进程监听接收的数据
int NTModbusMaster::SendAndListen()
{
    int cmdl = 0 ;
      unsigned char *p = 0;
     p = BuildMessage(&cmdl);

     if(cmdl > 0){
         flush();
         length = 0;
         return (Start() && SendData((char*)p , cmdl)>0 );
     }
     return 0;
}

void NTModbusMaster::timeout()
{
    int r = COMM_TIME_OUT;
    termination(r);
}

void NTModbusMaster::readyRead()
{
    int r = 0;
    if(length<0 || length >= 256)
        return;

    int count = serialPort->read(pTemp+length , 256-length);
    length += count;
    if(length>0 && startflag->isActive())
    {
        // 预处理 -- ASCII->HEX
        r = Pretreatment((unsigned char*)pTemp , length);
        if(r > 0){
            length = r;

            r = ProcessMessage((unsigned char*)pTemp , length);// 处理数据
            if(r == 0)
            {
                RWLocalData();//根据接收的数据设置系统的参数
                termination(r);
            }
        }
    }
}

//等待接收数据，5s的超时等待
/*void NTModbusMaster::run()
{
    int count = 0 , r = 0;

    while(count < SLAVE_TIMEOUT)
    {
        length = Read(pTemp);
        if(length>0){
            // 预处理 -- ASCII->HEX
            r = Pretreatment((unsigned char*)pTemp , length);
            if(r > 0){
                length = r;
                // 处理数据
                r = ProcessMessage((unsigned char*)pTemp , length);
                if(r == 0)
                    RWLocalData();//根据接收的数据设置系统的参数
            }
            goto THREAD_END;
        }

        count++;
        sleep_ms(1);
        if(stopThread == true){
            r = COMM_TIME_OUT;
            goto THREAD_END;
        }
    }
    r = COMM_TIME_OUT;

THREAD_END:
    termination(r);
}
*/

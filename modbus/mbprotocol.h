#ifndef __PROTOCOL_H___
#define __PROTOCOL_H___

#include "mbdefines.h"
#include <vector>
//#include <stdlib.h>

// This structure is byte-aligned
typedef struct {
    unsigned char device_address;//设备地址
    unsigned char function_code;// 功能码
    unsigned short first;// 首个寄存器的地址
    unsigned short number;// 寄存器个数
    unsigned char data_bytes; // 数据个数
    std::vector<unsigned short> data; // 数据
} modbus_rtu_data;


class ModbusProtocolBaseEx
{
protected:    
    ModbusProtocolBaseEx();//构造函数
    ~ModbusProtocolBaseEx();//析构函数

public:
    //设置和读取当前设备地址(主从通用)
    void SetDeviceAddress(unsigned char addr);
    unsigned char GetDeviceAddress();

    //获取最后出现的异常代码
    unsigned char GetLastException();

    //设置和读取当前的传输模式
    int GetTransmissionMode();
    bool SetTransmissionMode(int mode);

protected:
    //读取或者设置系统中的数据，返回异常代码。
    virtual unsigned char GetSystemCoilStatus(unsigned short first,int counts,std::vector<bool> &values);
    virtual unsigned char SetSystemCoilStatus(unsigned short first,std::vector<bool> &values) ;
    virtual unsigned char GetSystemInputStatus(unsigned short first,int counts,std::vector<bool> &values);
    virtual unsigned char SetSystemInputStatus(unsigned short first,std::vector<bool> &values) ;
    virtual unsigned char GetSystemHoldRegister(unsigned short first,int counts,std::vector<unsigned short> &values);
    virtual unsigned char SetSystemHoldRegister(unsigned short first,std::vector<unsigned short> &values) ;
    virtual unsigned char GetSystemInputRegister(unsigned short first,int counts,std::vector<unsigned short> &values);
    virtual unsigned char SetSystemInputRegister(unsigned short first,std::vector<unsigned short> &values) ;

    // If CRC is correct returns 0 else returns INVALID_CHECK
    int Verification(unsigned char *msg,const int msg_length);
    // 追加校验信息到数据的结尾
    int AppendCheck(unsigned char *msg, int length);
    // 清理
    void ClearBuffer(modbus_rtu_data &mrd);
    // byte和bit的相互转换
    void Bytes2Bits(std::vector<unsigned short> &vi ,int counts , std::vector<bool> &vo);
    void Bits2Bytes(std::vector<bool> &vi , int count, std::vector<unsigned short> &vo);

    // 预处理，将ASCII的协议转换成RTU的协议(如果是RTU协议则不处理)，执行以下操作：
    // 1，验证并去掉ASCII的头尾，并在结尾补充一个00，使数据对齐
    // 2，将处理后的ASCII数据转换成HEX形式，并按照RTU的处理方式处理数据
    int Pretreatment(unsigned char *in , int len);
    // 后处理，将RTU的协议转换成ASCII的协议
    int Posttreatment(unsigned char *in , int len);

protected:

    //设备地址(主从通用)
    unsigned char device_address;
    unsigned char nt_exception_code;
    int transmission_mode;

    //发送和接收到的原始数据
    unsigned char rraw[MAX_DATA_LENGTH];
    unsigned char sraw[MAX_DATA_LENGTH];
    int slength,rlength;

    //格式化的数据
    modbus_rtu_data sformat , rformat;
};


class RTUSlaveProtocol : public ModbusProtocolBaseEx
{
public:
    RTUSlaveProtocol();
    ~RTUSlaveProtocol();

    unsigned char getSlaveAddress();

protected:
    // 分解从主机发送进来的数据，返回0代表，数据正常，其他值为异常代码
    int ProcessMessage(unsigned char *msg , int len);
    //从机根据主机rtu数据结构体生成字符,如果数据长度0数据组合失败
    unsigned char *BuildMessage(int *len);
    //根据解析出来的数据设置或者读取系统的参数
    void RWLocalData();
    //生成异常数据的字符串,如果数据长度0数据组合失败
    unsigned char *BuildExceptionMessage(int *len);

private:
    //从机这两个函数不能使用
    virtual unsigned char SetSystemInputStatus(unsigned short first,std::vector<bool> &values) ;
    virtual unsigned char SetSystemInputRegister(unsigned short first,std::vector<unsigned short> &values) ;

};


class RTUMasterProtocol: public ModbusProtocolBaseEx
{
public:
    RTUMasterProtocol();
    ~RTUMasterProtocol();

protected:
    //主机设置寄存器,type传入功能码
    void SetRegister(unsigned short fregister ,int count, std::vector<unsigned short> &data, unsigned char type);
    //主机读取寄存器
    void GetRegister(unsigned short fregister , unsigned short count , unsigned char type);

    //根据发送的指令预测从机应该返回的数据的长度
    unsigned char expect_length;
    int GetRespondsLength(modbus_rtu_data &c);

    //根据主机的指令成指令字符串 , 返回指令
    unsigned char* BuildMessage(int *len);
    //分解从从机发送进来的数据，返回0数据正常其他值为异常代码
    int ProcessMessage(unsigned char *msg , int len);
    void RWLocalData();

private:
    //主机这两个函数不能使用
    virtual unsigned char GetSystemCoilStatus(unsigned short first,int counts,std::vector<bool> &values);
    virtual unsigned char GetSystemInputStatus(unsigned short first,int counts,std::vector<bool> &values);
    virtual unsigned char GetSystemHoldRegister(unsigned short first,int counts,std::vector<unsigned short> &values);
    virtual unsigned char GetSystemInputRegister(unsigned short first,int counts,std::vector<unsigned short> &values);
};

#endif 


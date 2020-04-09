#include "mbprotocol.h"

//===========================================================================
//ModbusProtocolBase
//RTU主从的公共部分提取
//===========================================================================
ModbusProtocolBaseEx::ModbusProtocolBaseEx()
{
    rlength = slength = 0;
    device_address = 1;
    nt_exception_code = NO_EXCEPTION;
    transmission_mode = TRANSMISSION_MODE_RTU;

    ClearBuffer(sformat);
    ClearBuffer(rformat);
}

//析构函数
ModbusProtocolBaseEx::~ModbusProtocolBaseEx()
{
}

//设置和读取当前设备地址
void ModbusProtocolBaseEx::SetDeviceAddress(unsigned char addr)
{
    device_address = addr;
}

unsigned char ModbusProtocolBaseEx::GetDeviceAddress()
{
    return device_address;
}

//获取最后出现的异常代码
unsigned char ModbusProtocolBaseEx::GetLastException()
{
    return nt_exception_code;
}

//读取当前的传输模式
int ModbusProtocolBaseEx::GetTransmissionMode()
{
    return transmission_mode;
}

//设置当前的传输模式
bool ModbusProtocolBaseEx::SetTransmissionMode(int mode)
{
    if(mode==TRANSMISSION_MODE_RTU || mode == TRANSMISSION_MODE_ASCII){
        transmission_mode = mode;
        return true;
    }

    return false;
}

//获取系统中保持寄存器的值
unsigned char ModbusProtocolBaseEx::GetSystemCoilStatus(unsigned short first_register, int counts, std::vector<bool> &values)
{
    _UNUSED_(first_register);
    _UNUSED_(counts);
    _UNUSED_(values);

    return ILLEGAL_DATA_ADDRESS;
}

//设置系统中线圈的状态
unsigned char ModbusProtocolBaseEx::SetSystemCoilStatus(unsigned short first_register, std::vector<bool> &values)
{
    _UNUSED_(first_register);
    _UNUSED_(values);

    return ILLEGAL_DATA_ADDRESS;
}

//获取系统中输入寄存器的值
unsigned char ModbusProtocolBaseEx::GetSystemInputStatus(unsigned short first_register, int counts, std::vector<bool> &values)
{
    _UNUSED_(first_register);
    _UNUSED_(counts);
    _UNUSED_(values);

    return ILLEGAL_DATA_ADDRESS;
}

unsigned char ModbusProtocolBaseEx::SetSystemInputStatus(unsigned short first, std::vector<bool>  &values)
{
    _UNUSED_(first);
    _UNUSED_(values);

    return ILLEGAL_DATA_ADDRESS;
}

//获取系统中保持寄存器的值
unsigned char ModbusProtocolBaseEx::GetSystemHoldRegister(unsigned short first_register,
        int counts,
        std::vector<unsigned short> &values)
{
    _UNUSED_(first_register);
    _UNUSED_(counts);
    _UNUSED_(values);

    return ILLEGAL_DATA_ADDRESS;
}

//设置系统中保存寄存器的值
unsigned char ModbusProtocolBaseEx::SetSystemHoldRegister(unsigned short first_register,std::vector<unsigned short> &values)
{
    _UNUSED_(first_register);
    _UNUSED_(values);

    return ILLEGAL_DATA_ADDRESS;
}

//获取系统中输入寄存器的值
unsigned char ModbusProtocolBaseEx::GetSystemInputRegister(unsigned short first_register,int counts, std::vector<unsigned short> &values)
{
    _UNUSED_(first_register);
    _UNUSED_(counts);
    _UNUSED_(values);

    return ILLEGAL_DATA_ADDRESS;
}

unsigned char ModbusProtocolBaseEx::SetSystemInputRegister(unsigned short first, std::vector<unsigned short> &values)
{
    _UNUSED_(first);
    _UNUSED_(values);

    return ILLEGAL_DATA_ADDRESS;
}

/* If CRC is correct returns 0 else returns INVALID_CHECK */
int ModbusProtocolBaseEx::Verification(unsigned char *msg,const int msg_length)
{
    int ret = 0;

    if(transmission_mode == TRANSMISSION_MODE_RTU){
        unsigned short crc_calc;
        unsigned short crc_received;

        crc_calc = crc16(msg, msg_length - 2);
        crc_received = (msg[msg_length - 2] << 8) | msg[msg_length - 1];

        /* Check CRC of msg */
        if (crc_calc == crc_received)
            ret = 0;
        else
            ret = INVALID_CHECK;
    }else if(transmission_mode == TRANSMISSION_MODE_ASCII){
        unsigned char lrc_calc;
        unsigned char lrc_received;

        lrc_calc = lrc(msg, msg_length - 2);
        lrc_received = msg[msg_length - 2];

        /* Check LRC of msg */
        if (lrc_calc == lrc_received)
            ret = 0;
        else
            ret = INVALID_CHECK;
    }
    return ret;
}

// 追加校验信息到数据的结尾
int ModbusProtocolBaseEx::AppendCheck(unsigned char *msg , int length)
{
    int len = length;
    if(transmission_mode == TRANSMISSION_MODE_RTU){
        unsigned short crc = crc16(msg,length);
        msg[len++] = UINT16_HIGHBYTE(crc);
        msg[len++] = UINT16_LOWBYTE(crc);
    }else if(transmission_mode == TRANSMISSION_MODE_ASCII){
        msg[len++] = lrc(msg, length);
    }
    return len;
}

void ModbusProtocolBaseEx::ClearBuffer(modbus_rtu_data &mrd)
{
    mrd.device_address=0;//设备地址
    mrd.function_code = 0;/* 功能码 */
    mrd.first = 0;/* 首个寄存器的地址*/
    mrd.number =0;/* 寄存器个数 */
    mrd.data_bytes = 0; /* 数据个数 */
    mrd.data.clear(); /* 数据 */
}

void ModbusProtocolBaseEx::Bytes2Bits(std::vector<unsigned short> &vi,int count , std::vector<bool> &vo)
{
    unsigned char v = 0x00;
    for(int i=0;i<count;i++){
        v = UINT16_LOWBYTE(vi[i/8]);
        if(GET_UINT8_BIT(v,i%8))
            vo.push_back(true);
        else
            vo.push_back(false);
    }
}

void ModbusProtocolBaseEx::Bits2Bytes(std::vector<bool> &vi, int count,std::vector<unsigned short> &vo)
{    
    unsigned char t = 0x00;
    vo.clear();

    for(int i=0;i<count;i++){
        t = vi[i]?SET_UINT8_BIT(t,i%8):RESET_UINT8_BIT(t,i%8);
        if(i%8==7 && i+1 != count){
            vo.push_back(t);
            t = 0x00;
        }
    }
    vo.push_back(t);
}

// 预处理，将ASCII的协议转换成RTU的协议。
// 返回转换后的数据长度，如果长度<=0说明数据存在异常
int ModbusProtocolBaseEx::Pretreatment(unsigned char *in, int len)
{
    int ret = 0;
    // RTU协议不处理
    if(transmission_mode == TRANSMISSION_MODE_RTU)
        return len;

    //检测头尾
    if(in[0]!=':' && in[len-2]!='\r' && in[len-1]!='\n')
        return INVALID_DATA;

    //检测字符
    for(int i=1;i<len-2;i++){
        if(in[i]<'0'||(in[i]>'9'&&in[i]<'A')||(in[i]>'F'&&in[i]<'a')||in[i]>'f')
            return INVALID_DATA;
    }

    //检测长度
    if(len%2 == 0)
        return INVALID_DATA;

    // 转换
    for(;ret<(len-3)/2;ret++){
        in[ret] = ASCII2HEX(in[ret*2+1])*0x10 + ASCII2HEX(in[ret*2+2]);
    }

    //结尾追加“00”
    in[ret++] = 0x00;
    return ret;
}

// 后处理，将RTU的协议转换成ASCII的协议
int ModbusProtocolBaseEx::Posttreatment(unsigned char *in, int len)
{
    int expect_len = 0;
    if(transmission_mode == TRANSMISSION_MODE_RTU)
        expect_len = len;
    else if(transmission_mode == TRANSMISSION_MODE_ASCII){
        expect_len = len*2+1;
        for(int i=len-1;i>=0;i--){
            in[i*2+2] = HEX2ASCII(in[i]%0x10);
            in[i*2+1] = HEX2ASCII(in[i]/0x10);
        }
        in[0] = ':';
        in[expect_len++] = '\r';
        in[expect_len++] = '\n';
    }

    return expect_len;
}

//===========================================================================
//RTUSlaveProtocol
//从机部分
//===========================================================================
RTUSlaveProtocol::RTUSlaveProtocol() :
    ModbusProtocolBaseEx()
{

}

//析构函数
RTUSlaveProtocol::~RTUSlaveProtocol()
{
    
}

unsigned char RTUSlaveProtocol::getSlaveAddress()
{
    return rformat.device_address;
}

// 分解从主机发送进来的数据，返回0代表，数据正常，其他值为异常代码
int RTUSlaveProtocol::ProcessMessage(unsigned char *msg, int len)
{
    //数据校验
    if(len<MIN_DATA_LENGTH)//检验数据长度
        return DATA_LENGTH_ERROR;
    if(Verification(msg,len) != 0) //CRC校验
        return INVALID_CHECK;
//    if(msg[0] != device_address)
//        return ADDRESS_MISMATCH;

    memcpy(rraw,msg,len);
    rlength = len;
    rformat.device_address = rraw[0];
    rformat.function_code = rraw[1];
    switch(rformat.function_code){
    case FC_READ_COIL_STATUS:
    case FC_READ_INPUT_STATUS:
    case FC_READ_HOLDING_REGISTERS:
    case FC_READ_INPUT_REGISTERS:{
        if(len != READ_DATA_LENGTH)
            return DATA_LENGTH_ERROR;
        rformat.first = UINT8TOUINT16(rraw[2],rraw[3]);
        rformat.number = UINT8TOUINT16(rraw[4],rraw[5]);
    }break;
    case FC_FORCE_SINGLE_COIL:
    case FC_PRESET_SINGLE_REGISTER:{
        if(len != SET_SINGEL_DATA_LENGTH)
            return DATA_LENGTH_ERROR;

        rformat.number = 1;
        rformat.first = UINT8TOUINT16(rraw[2],rraw[3]);
        unsigned short res =  UINT8TOUINT16(rraw[4],rraw[5]);
        rformat.data.clear();
        if(rformat.function_code == FC_PRESET_SINGLE_REGISTER)
            rformat.data.push_back(res);
        else{
            /*强行使格式和写多个线圈的格式一致*/
            if(res == 0xFF00)
                rformat.data.push_back(0x0001);
            else if(res == 0x0000)
                rformat.data.push_back(0x0000);
            else
                return INVALID_DATA;
        }
    }break;
    case FC_FORCE_MULTIPLE_COILS:{
        if(len<SET_MIN_DATA_LENGTH)
            return DATA_LENGTH_ERROR;
        rformat.first = UINT8TOUINT16(rraw[2],rraw[3]);
        rformat.number = UINT8TOUINT16(rraw[4],rraw[5]);
        rformat.data_bytes = rraw[6];
        int l = SET_MIN_DATA_LENGTH + rformat.data_bytes;
        if(l != len || rformat.data_bytes != (rformat.number+7)/8)
            return DATA_LENGTH_ERROR;

        rformat.data.clear();
        for(int i=0;i<rformat.data_bytes;i++)
            rformat.data.push_back(UINT8TOUINT16(0x00,rraw[7+i]));
    }break;
    case FC_PRESET_MULTIPLE_REGISTERS:{
        if(len<SET_MIN_DATA_LENGTH)
            return DATA_LENGTH_ERROR;
        rformat.first = UINT8TOUINT16(rraw[2],rraw[3]);
        rformat.number = UINT8TOUINT16(rraw[4],rraw[5]);
        rformat.data_bytes = rraw[6];
        int l = SET_MIN_DATA_LENGTH + rformat.data_bytes;
        if(l != len || rformat.data_bytes != rformat.number*2)
            return DATA_LENGTH_ERROR;

        rformat.data.clear();
        for(int i=0;i<rformat.data_bytes;i+=2){
            int j = 7 + i;
            rformat.data.push_back(UINT8TOUINT16(rraw[j],rraw[j+1]));
        }
    }break;
    default:
        return INVALID_DATA;
    }

    return 0;
}

//根据解析出来的数据设置或者读取系统的参数
void RTUSlaveProtocol::RWLocalData()
{
    sformat.device_address = rformat.device_address;
    sformat.function_code = rformat.function_code;
    sformat.first = rformat.first;
    sformat.number = rformat.number;

    switch(sformat.function_code)
    {
    case FC_READ_COIL_STATUS:
    case FC_READ_INPUT_STATUS:{
        std::vector<bool> data;
        if(sformat.function_code == FC_READ_COIL_STATUS)
            nt_exception_code = GetSystemCoilStatus(sformat.first , sformat.number , data);
        else
            nt_exception_code = GetSystemInputStatus(sformat.first , sformat.number , data);

        if(nt_exception_code == NO_EXCEPTION)
            Bits2Bytes(data , data.size() , sformat.data);
    }break;
    case FC_READ_HOLDING_REGISTERS:
        sformat.data.clear();
        nt_exception_code = GetSystemHoldRegister(sformat.first ,sformat.number, sformat.data);
        break;
    case FC_READ_INPUT_REGISTERS:
        sformat.data.clear();
        nt_exception_code = GetSystemInputRegister(sformat.first ,sformat.number, sformat.data);
        break;
    case FC_FORCE_SINGLE_COIL:{
        std::vector<bool> data;
        if(rformat.data[0])
            data.push_back(true);
        else
            data.push_back(false);
        nt_exception_code = SetSystemCoilStatus(sformat.first , data);
    }break;
    case FC_FORCE_MULTIPLE_COILS:{
        std::vector<bool> data;
        Bytes2Bits(rformat.data , rformat.number , data);
        nt_exception_code = SetSystemCoilStatus(sformat.first , data);
    }break;
    case FC_PRESET_SINGLE_REGISTER:
    case FC_PRESET_MULTIPLE_REGISTERS:
        nt_exception_code = SetSystemHoldRegister(sformat.first , rformat.data);
        break;
    default:
        break;
    }
}

//从机根据主机rtu数据结构体生成字符,返回数据长度，如果返回0数据组合失败
unsigned char *RTUSlaveProtocol::BuildMessage(int *len)
{
    int index = 0;
    sraw[index++] = sformat.device_address;
    sraw[index++] = sformat.function_code;
    
    switch(sformat.function_code){
    case FC_READ_COIL_STATUS:
    case FC_READ_INPUT_STATUS:{
        sraw[index++] = sformat.data_bytes =(unsigned char)( (sformat.number+7)/8 );
        for(size_t i=0;i<sformat.data.size();i++)
            sraw[index++] = UINT16_LOWBYTE(sformat.data.at(i));
    }break;
    case FC_READ_HOLDING_REGISTERS:
    case FC_READ_INPUT_REGISTERS:{
        sraw[index++] = sformat.data_bytes =(unsigned char) (sformat.number*2);
        for(size_t i=0;i<sformat.data.size();i++){
            unsigned short t = sformat.data.at(i);

            index = 3+i*2;
            sraw[index++] = UINT16_HIGHBYTE(t);
            sraw[index++] = UINT16_LOWBYTE(t);
        }
    }break;
    case FC_FORCE_SINGLE_COIL:
    case FC_PRESET_SINGLE_REGISTER:
    case FC_PRESET_MULTIPLE_REGISTERS:
    case FC_FORCE_MULTIPLE_COILS:{
        memcpy(sraw,rraw,6);
        index = 6;
    }break;
    default:
        *len = slength = 0;
        return sraw;
    }

    slength = AppendCheck(sraw,index);
    *len = slength = Posttreatment(sraw,slength);
    return sraw;
}

//生成异常数据的字符串，返回异常数据长度
unsigned char* RTUSlaveProtocol::BuildExceptionMessage(int *len)
{
    sraw[0] = sformat.device_address;
    sraw[1] = sformat.function_code + EXCEPTION_OFFSET;
    sraw[2] = nt_exception_code;

    slength = AppendCheck(sraw,3);
    *len = slength = Posttreatment(sraw,slength);
    return sraw;
}

//从机不存在该指令
unsigned char RTUSlaveProtocol::SetSystemInputStatus(unsigned short first, std::vector<bool> &values)
{
    _UNUSED_(first);
    _UNUSED_(values);

    return  ILLEGAL_FUNCTION;
}

//从机不存在该指令
unsigned char RTUSlaveProtocol::SetSystemInputRegister(unsigned short first, std::vector<unsigned short> &values)
{
    _UNUSED_(first);
    _UNUSED_(values);

    return  ILLEGAL_FUNCTION;
}


//===========================================================================
//RTUMasterProtocol
//RTU主机特有的部分
//===========================================================================
RTUMasterProtocol::RTUMasterProtocol()
{

}

RTUMasterProtocol::~RTUMasterProtocol()
{

}

//设置寄存器
void RTUMasterProtocol::SetRegister(unsigned short fregister,int count, std::vector<unsigned short> &data , unsigned char type)
{
    sformat.device_address = device_address;
    sformat.function_code = type;
    sformat.first = fregister;// 首个寄存器的地址
    sformat.number = count;// 寄存器个数
    sformat.data_bytes = 0; // 数据个数
    sformat.data = data; // 数据
}

//读取寄存器
void RTUMasterProtocol::GetRegister(unsigned short fregister, unsigned short count, unsigned char type)
{
    sformat.device_address = device_address;
    sformat.function_code = type;
    sformat.first = fregister;// 首个寄存器的地址
    sformat.number = count;// 寄存器个数
    sformat.data_bytes = 0; // 数据个数
    sformat.data.clear(); // 数据
}

//根据主机的指令成指令字符串
unsigned char* RTUMasterProtocol::BuildMessage(int *len)
{
    unsigned short value = 0x0000;
    int index = 0;
    sraw[index++] = sformat.device_address;
    sraw[index++] = sformat.function_code;
    sraw[index++] = UINT16_HIGHBYTE(sformat.first);
    sraw[index++] = UINT16_LOWBYTE(sformat.first);

    switch(sformat.function_code){
    case FC_READ_COIL_STATUS:
    case FC_READ_INPUT_STATUS:
    case FC_READ_HOLDING_REGISTERS:
    case FC_READ_INPUT_REGISTERS:
        sraw[index++] = UINT16_HIGHBYTE(sformat.number);
        sraw[index++] = UINT16_LOWBYTE(sformat.number);
        break;
    case FC_FORCE_SINGLE_COIL:
    case FC_PRESET_SINGLE_REGISTER:
        if( 1 != sformat.data.size() ){
            *len = slength = 0;
            return sraw;
        }
        value = sformat.data.at(0);
        sraw[index++] = UINT16_HIGHBYTE(value);
        sraw[index++] = UINT16_LOWBYTE(value);
        break;
    case FC_FORCE_MULTIPLE_COILS:{
        sraw[index++] = UINT16_HIGHBYTE(sformat.number);
        sraw[index++] = UINT16_LOWBYTE(sformat.number);
        int dbs = sraw[index++] = (sformat.number+7)/8 ;
        if(dbs != (int)sformat.data.size()){
            *len = slength = 0;
            return sraw;
        }
        for(int i=0;i<dbs;i++)
            sraw[index++] = sformat.data.at(i);
        }break;
    case FC_PRESET_MULTIPLE_REGISTERS:{
        sraw[index++] = UINT16_HIGHBYTE(sformat.number);
        sraw[index++] = UINT16_LOWBYTE(sformat.number);
        sraw[index++] = sformat.number*2 ;
        if(sformat.number != (int)sformat.data.size()){
            *len = slength = 0;
            return sraw;
        }
        for(int i=0;i<sformat.number;i++){
            value = sformat.data.at(i);
            sraw[index++] = UINT16_HIGHBYTE(value);
            sraw[index++] = UINT16_LOWBYTE(value);
        }
        }break;
    default:
        *len = slength = 0;
        return sraw;
    }

    slength = AppendCheck(sraw,index);
    *len = slength = Posttreatment(sraw,slength);
    expect_length = GetRespondsLength(sformat);

    return sraw;
}

//分解从从机发送进来的数据，返回0数据正常其他值为异常代码
int RTUMasterProtocol::ProcessMessage(unsigned char *msg, int len)
{
    //数据校验
    if(len!=EXCEPTION_DATA_LENGTH && len!=expect_length)
        return DATA_LENGTH_ERROR;
    if(msg[0] != sformat.device_address)
        return ADDRESS_MISMATCH;
    if(Verification(msg,len)!=0)
        return INVALID_CHECK;

    memcpy(rraw,msg,len);
    rlength = len;

    //接收到的是从机上传异常代码
    if(rraw[1]==sformat.function_code+EXCEPTION_OFFSET
            && len==EXCEPTION_DATA_LENGTH){
        nt_exception_code =  rraw[2];
        return nt_exception_code;
    }
    //接收到从机上传的数据
    else if(rraw[1] == sformat.function_code &&
             len==expect_length){
        rformat.device_address = rraw[0];
        rformat.function_code = rraw[1];

        switch(rformat.function_code){
        case FC_READ_COIL_STATUS:
        case FC_READ_INPUT_STATUS:
            rformat.data_bytes = rraw[2];

            rformat.data.clear();
            for(int i=0;i<rformat.data_bytes;i++)
                rformat.data.push_back(UINT8TOUINT16(0x00 , rraw[i+3]));
            break;
        case FC_READ_HOLDING_REGISTERS:
        case FC_READ_INPUT_REGISTERS:
            rformat.data_bytes = rraw[2];

            rformat.data.clear();
            for(int i=0;i<rformat.data_bytes/2;i++)
                rformat.data.push_back(UINT8TOUINT16(rraw[2*i+3],rraw[2*i+4]));
            break;
        case FC_FORCE_SINGLE_COIL:
        case FC_PRESET_SINGLE_REGISTER:
        case FC_FORCE_MULTIPLE_COILS:
        case FC_PRESET_MULTIPLE_REGISTERS:
            rformat.first = UINT8TOUINT16(rraw[2],rraw[3]);

            if(rformat.first == sformat.first)
                break;
        default:
            return INVALID_DATA;
        }
    }else{
        return SELECT_FAILURE;
    }
    return NO_EXCEPTION;
}

//根据发送的指令预测从机应该返回的数据的长度
int RTUMasterProtocol::GetRespondsLength(modbus_rtu_data &c)
{
    int ln = 0;
    switch(c.function_code){
    case FC_READ_COIL_STATUS:
    case FC_READ_INPUT_STATUS:
        ln = 5+(c.number+7)/8;
        break;
    case FC_READ_HOLDING_REGISTERS:
    case FC_READ_INPUT_REGISTERS:
        ln = 5+c.number*2;
        break;
    case FC_FORCE_SINGLE_COIL:
    case FC_PRESET_SINGLE_REGISTER:
    case FC_FORCE_MULTIPLE_COILS:
    case FC_PRESET_MULTIPLE_REGISTERS:
        ln = 8;
        break;
    }

    return ln;
}

//根据解析出来的数据设置或者读取系统的参数
void RTUMasterProtocol::RWLocalData()
{
    std::vector<bool> data;
    switch(rformat.function_code)
    {
    case FC_READ_COIL_STATUS:
        Bytes2Bits(rformat.data , sformat.number , data);
        nt_exception_code = SetSystemCoilStatus(sformat.first , data);
        break;
    case FC_READ_INPUT_STATUS:
        Bytes2Bits(rformat.data , sformat.number , data);
        nt_exception_code = SetSystemInputStatus(sformat.first , data);
        break;
    case FC_READ_HOLDING_REGISTERS:
        nt_exception_code = SetSystemHoldRegister(sformat.first , rformat.data);
        break;
    case FC_READ_INPUT_REGISTERS:
        nt_exception_code = SetSystemInputRegister(sformat.first , rformat.data);
        break;

    default:
        break;
    }
}

//主机不存在该指令
unsigned char RTUMasterProtocol::GetSystemCoilStatus(unsigned short first_register, int counts, std::vector<bool> &values)
{
    _UNUSED_(first_register);
    _UNUSED_(counts);
    _UNUSED_(values);

    return ILLEGAL_FUNCTION;
}

//主机不存在该指令
unsigned char RTUMasterProtocol::GetSystemInputStatus(unsigned short first_register, int counts, std::vector<bool> &values)
{
    _UNUSED_(first_register);
    _UNUSED_(counts);
    _UNUSED_(values);

    return ILLEGAL_FUNCTION;
}

//主机不存在该指令
unsigned char RTUMasterProtocol::GetSystemHoldRegister(unsigned short first_register,
        int counts,
        std::vector<unsigned short> &values)
{
    _UNUSED_(first_register);
    _UNUSED_(counts);
    _UNUSED_(values);

    return ILLEGAL_FUNCTION;
}

//主机不存在该指令
unsigned char RTUMasterProtocol::GetSystemInputRegister(unsigned short first_register,int counts, std::vector<unsigned short> &values)
{
    _UNUSED_(first_register);
    _UNUSED_(counts);
    _UNUSED_(values);

    return ILLEGAL_FUNCTION;
}

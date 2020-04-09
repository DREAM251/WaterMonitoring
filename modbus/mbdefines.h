#ifndef __MODBUS_DEF_H_
#define __MODBUS_DEF_H_

#include <qglobal.h>

/* Function codes */
#define FC_READ_COIL_STATUS          0x01  /* discretes inputs */
#define FC_READ_INPUT_STATUS         0x02  /* discretes outputs */
#define FC_READ_HOLDING_REGISTERS    0x03
#define FC_READ_INPUT_REGISTERS      0x04
#define FC_FORCE_SINGLE_COIL         0x05
#define FC_PRESET_SINGLE_REGISTER    0x06
#define FC_READ_EXCEPTION_STATUS     0x07
#define FC_FORCE_MULTIPLE_COILS      0x0F
#define FC_PRESET_MULTIPLE_REGISTERS 0x10
#define FC_REPORT_SLAVE_ID           0x11

/* Protocol exceptions */
#define NO_EXCEPTION            0x00
#define ILLEGAL_FUNCTION        0x01
#define ILLEGAL_DATA_ADDRESS    0x02
#define ILLEGAL_DATA_VALUE      0x03
#define SLAVE_DEVICE_FAILURE    0x04
#define SERVER_FAILURE          0x04
#define ACKNOWLEDGE             0x05
#define SLAVE_DEVICE_BUSY       0x06
#define SERVER_BUSY             0x06
#define NEGATIVE_ACKNOWLEDGE    0x07
#define MEMORY_PARITY_ERROR     0x08
#define GATEWAY_PROBLEM_PATH    0x0A
#define GATEWAY_PROBLEM_TARGET  0x0B

/* offsets */
#define EXCEPTION_OFFSET    0x80
#define COIL_STATUS_OFFSET    0x00000000UL
#define INPUT_STATUS_OFFSET    0x00010000UL
#define HOLDING_REGISTER_OFFSET    0x00040000UL
#define INPUT_REGISTER_OFFSET    0x00030000UL
#define REGISTER_OFFSET_MASK    0xFFFF0000UL

/* Local */
#define ADDRESS_MISMATCH        -0x01
#define COMM_TIME_OUT           -0x0C
#define PORT_SOCKET_FAILURE     -0x0D
#define SELECT_FAILURE          -0x0E
#define TOO_MANY_DATA           -0x0F
#define INVALID_CHECK             -0x10
#define INVALID_EXCEPTION_CODE  -0x11
#define CONNECTION_CLOSED       -0x12
#define DATA_LENGTH_ERROR       -0x13
#define TOO_LITTLE_DATA         -0x14
#define INVALID_DATA            -0x15

#define MIN_DATA_LENGTH             5
#define MAX_DATA_LENGTH             520
#define EXCEPTION_DATA_LENGTH       5
#define READ_DATA_LENGTH             8
#define SET_SINGEL_DATA_LENGTH       8
#define SET_MIN_DATA_LENGTH          9

//传输模式
#define TRANSMISSION_MODE_RTU     0
#define TRANSMISSION_MODE_ASCII     1
#define TRANSMISSION_MODE_TCP     2

/*
 *Modbus 通讯时规定主机发送完一组命令必须间隔3.5个字符再发送下一组新命令，
 *这个3.5字符主要用来告诉其他设备这次命令（数据）已结束
 */
#define MESSAGE_FINISH_TIME    10//这里简化为10MS

#define SLAVE_TIMEOUT  2000 //从机应答超时（毫秒）

#ifndef __BIG_ENDIAN
#define  __BIG_ENDIAN  4321
#endif
#ifndef __LITTLE_ENDIAN
#define __LITTLE_ENDIAN  1234
#endif

#ifndef _UNUSED_
#define _UNUSED_(x) (void)(x)
#endif

#if defined (Q_WS_WIN)
#include <windows.h>
#define sleep_ms(x)  Sleep(x)
#else
#include <unistd.h>
#define sleep_ms(x)  usleep(x*1000)
#endif

//计算CRC
unsigned short crc16(unsigned char *buffer, unsigned short buffer_length);
//计算LRC
unsigned char lrc(unsigned char* pdata, int length);
//按照MODBUS的格式将UINT16分解成两个unsigned char类型
unsigned char u16split(unsigned short v , int part);
//按照MODBUS的字节顺序将两个字节合并成一个
unsigned short u8combine(unsigned char highbyte , unsigned char lowbyte);
//获取当前系统的大小端格式
int get_system_endian();
//ASCII转成一个HEX格式,返回hex形式数据
unsigned char ASCII2HEX(unsigned char a);
//一个HEX转成ASCII格式,返回ASCII形式数据
unsigned char HEX2ASCII(unsigned char a);

typedef union{
    unsigned char I8[2];
    unsigned short I16;
}I82I16;

#define UINT16_HIGHBYTE(x)  (u16split(x,0))
#define UINT16_LOWBYTE(x)   (u16split(x,1))
#define UINT8TOUINT16(H,L)  (u8combine(H,L))
#define GET_UINT8_BIT(src,index)  (0x01<<index&src?true:false)
#define SET_UINT8_BIT(src,index)     (src|0x01<<index)
#define RESET_UINT8_BIT(src,index)   (src&~(0x01<<index))

//接口
#define MODBUS_INTERFACE_SERIAL

#endif // MODBUS_DEF_H



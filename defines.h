#ifndef _DEFINES_H_
#define _DEFINES_H_


#include <qglobal.h>
#include "common.h"

//板子供货商
#define VENDER_WEIQIAN //微嵌


#ifndef Q_WS_X11
    #if defined VENDER_WEIQIAN
        #define DTU_PORT      "/dev/ttyAMA1"
        #define EXT_PORT      "/dev/ttyAMA2"
        #define UL_PORT       "/dev/ttyAMA3"
        #define PROBE_PORT    "/dev/ttyAMA5"
    #endif
#else
    #define EXT_PORT        "/dev/ttyS0"
    #define DTU_PORT        "/dev/ttyUSB0"
    #define UL_PORT         "/dev/ttyS2"
    #define PROBE_PORT      "/dev/ttyS1"
#endif


#define     processLogger()     (LOG_WRITER::getObject("logs/process"))
#define     probeLogger()       (LOG_WRITER::getObject("logs/probe"))
#define     systemLogger()      (LOG_WRITER::getObject("logs/system"))
#define     mcuLogger()         (LOG_WRITER::getObject("logs/mcu"))
#define     modbusLogger()      (LOG_WRITER::getObject("logs/modbus"))
#define     logger()            (LOG_WRITER::getObject("logs/debug"))

#endif

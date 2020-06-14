﻿#include "elementfactory.h"
#include "defines.h"

ElementFactory::ElementFactory(QString type) :
    element(type)
{
}

ITask *ElementFactory::getTask(TaskType type)
{
    ITask *it = NULL;

    switch (type)
    {
    case TT_Measure:it = static_cast<ITask *>(new MeasureTask());break;
    case TT_ZeroCalibration:it = static_cast<ITask *>(new CalibrationTask());break;
    case TT_SampleCalibration:it = static_cast<ITask *>(new CalibrationTask());break;
    case TT_ZeroCheck:it = static_cast<ITask *>(new MeasureTask());break;
    case TT_SampleCheck:it = static_cast<ITask *>(new MeasureTask());break;
    case TT_SpikedCheck:it = static_cast<ITask *>(new MeasureTask());break;
    case TT_ErrorProc:it = static_cast<ITask *>(new ErrorTask());break;
    case TT_Stop:it = static_cast<ITask *>(new StopTask());break;
    case TT_Clean:it = static_cast<ITask *>(new CleaningTask());break;
    case TT_Drain:it = static_cast<ITask *>(new EmptyTask());break;
    case TT_Initial:it = static_cast<ITask *>(new InitialTask());break;
    case TT_Debug:it = static_cast<ITask *>(new DebugTask());break;
    case TT_Initload:it = static_cast<ITask *>(new InitialLoadTask());break;
    case TT_Func:break;
    case TT_Config:it = static_cast<ITask *>(new DeviceConfigTask());break;
    default:
        break;
    }
    return it;
}

IProtocol *ElementFactory::getProtocol()
{
    return new IProtocol(UL_PORT",9600,n,8,1");
}

QString ElementFactory::getElementName()
{
    if (element == "NH3N/") {
        return QObject::tr("氨氮");
    }
    else if (element == "TP/") {
        return QObject::tr("总磷");
    }
    else if (element == "TN/") {
        return QObject::tr("总氮");
    }
    else
        return QObject::tr("CODCr");
}

QString ElementFactory::getElementUnit()
{
    return "mg/L";
}

QString ElementFactory::getDeviceName()
{
    if (element == "NH3N/") {
        return QObject::tr("ZS-VS01型氨氮（NH3N）在线监测仪");
    }
    else if (element == "TP/") {
        return QObject::tr("ZS-VS01型总磷（TP）在线监测仪");
    }
    else if (element == "TN/") {
        return QObject::tr("ZS-VS01型总氮（TN）在线监测仪");
    }
    else
        return QObject::tr("ZS-VS01型化学需氧量（CODcr）在线监测仪");
}

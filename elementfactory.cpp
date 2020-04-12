#include "elementfactory.h"
#include "defines.h"

ElementFactory::ElementFactory(ElementType type) :
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
    default:
        break;
    }
    return it;
}

IProtocol *ElementFactory::getProtocol()
{
    return new IProtocol(UL_PORT",9600,n,8,1");
}

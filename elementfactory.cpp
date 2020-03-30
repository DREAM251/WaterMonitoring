#include "elementfactory.h"
#include "protocolv1.h"
#include "nh3ntask.h"

ElementFactory::ElementFactory(const QString &elementName) :
    element(elementName)
{
}

ElementFactory::ElementFactory(ElementType elementName)
{

}

ITask *ElementFactory::getTask(TaskType type)
{
    ITask *it = NULL;

    switch (type)
    {
    case TT_Measure:break;
    case TT_ZeroCalibration:break;
    case TT_SampleCalibration:break;
    case TT_ZeroCheck:break;
    case TT_SampleCheck:break;
    case TT_SpikedCheck:break;
    case TT_ErrorProc:break;
    default:
        break;
    }
    return it;
}

IProtocol *ElementFactory::getProtocol()
{

}

#ifndef ELEMENTFACTORY_H
#define ELEMENTFACTORY_H

#include "iprotocol.h"
#include "itask.h"

#include <QHash>
#include <QString>

enum ElementType
{
    ET_CODCr = 0,
    ET_NH3N,
    ET_TP,
    ET_TN,
    ET_CODMN,
    ET_TPb
};

class ElementFactory
{
public:
    ElementFactory(ElementType elementName);

    ITask *getTask(TaskType type);
    IProtocol *getProtocol();

private:
    ElementType element;
};

#endif // ELEMENTFACTORY_H

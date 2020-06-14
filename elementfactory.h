#ifndef ELEMENTFACTORY_H
#define ELEMENTFACTORY_H

#include "iprotocol.h"
#include "itask.h"

#include <QHash>
#include <QString>

class ElementFactory
{
public:
    ElementFactory(QString elementName);

    ITask *getTask(TaskType type);
    IProtocol *getProtocol();

    QString getElementName();
    QString getElementUnit();
    QString getDeviceName();

private:
    QString element;
};

#endif // ELEMENTFACTORY_H

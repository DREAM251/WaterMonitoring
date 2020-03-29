#ifndef ELEMENTFACTORY_H
#define ELEMENTFACTORY_H

#include "iprotocol.h"
#include "itask.h"

#include <QHash>
#include <QString>

class ElementFactory
{
public:
    ElementFactory(const QString &elementName = "NH3N");

    QHash<TaskType, ITask *> getFlowTable();
    IProtocol *getProtocol();
    ITask * errorProc;

private:
    QString element;
};

#endif // ELEMENTFACTORY_H

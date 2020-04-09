#ifndef PROTOCOLV1_H
#define PROTOCOLV1_H

#include "iprotocol.h"

class ProtocolV1 : public IProtocol
{
public:
    ProtocolV1(const QString &portArgs, QObject *parent = 0);
};

#endif // PROTOCOLV1_H

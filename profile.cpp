#include "profile.h"
#include <QDebug>

#if !defined (NO_PROFILE)
QSqlDatabase *Profile::profileDB = NULL;


void Profile::initProfile(const QString &dbname)
{
    qDebug() << "initProfile" << dbname;
}

void Profile::saveValue(char *_class, char *_value, const QVariant &value)
{
    qDebug() << "saveValue" << _class << _value << value;
}

QVariant Profile::loadValue(char *_class, char *_value)
{
    return QVariant();
}
#endif

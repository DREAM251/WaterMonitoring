#include "profile.h"
#include <QDebug>


Profile::Profile(const QString &name)
{

}

void Profile::initProfile(const QString &dbname)
{
    qDebug() << "initProfile" << dbname;
}

void Profile::beginSection(const QString &section)
{

}

void Profile::endSection()
{

}

void Profile::saveValue(const QString &name, const QVariant &value)
{
    qDebug() << "saveValue" << name << value;
}

QVariant Profile::loadValue(const QString &name)
{
    return QVariant(name);
}

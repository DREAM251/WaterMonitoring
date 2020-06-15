﻿#ifndef PROFILE_H
#define PROFILE_H

#include <QSqlDatabase>
#include <QVariant>
#include "globelvalues.h"
#include "common.h"
#include <QString>



class DatabaseProfile
{
public:

    DatabaseProfile();

    bool beginSection(const QString &section);
    bool setValue(const QString &key, const QVariant &value, const QString &keyName = QString());
    bool setValue(const QString &section, const QString &key, const QVariant &value);
    QVariant value(const QString &key, const QVariant &defaultValue = QVariant());


protected:
    bool updateValue(const QString &section, const QString &name, const QVariant &value);

private:
    QString sectionName;
    QSqlDatabase database;
};

#endif // PROFILE_H

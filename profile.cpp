#include "profile.h"
#include <QDebug>
#include <QFile>
#include <QSqlQuery>
#include <QStringList>
#include <QSqlError>


DatabaseProfile::DatabaseProfile()
{
    QString name = elementPath + "/config.db";
    QSqlDatabase sqlitedb = QSqlDatabase::database(name);

    if (!sqlitedb.isValid()) {
        sqlitedb = QSqlDatabase::addDatabase("QSQLITE", name);
        sqlitedb.setDatabaseName(name);
    }

    if (!sqlitedb.isOpen())
    {
        if (!sqlitedb.open())
        {
            qDebug() << sqlitedb.lastError() << name;
            return;
        }
    }
    database = sqlitedb;
}


bool DatabaseProfile::beginSection(const QString &section)
{
    QSqlDatabase sqlitedb = database;
    if (!sqlitedb.isValid())
    {
        qDebug() << "sqldatabase is invalid!" << section;
        return false;
    }

    if (!sqlitedb.tables().contains(section))
    {
        QSqlQuery query(sqlitedb);
        QString sqlstring = QString("create table if not exists %1(key text unique, value text);").arg(section);
        if (!query.exec(sqlstring))
        {
            qDebug() << query.lastError() << sqlstring;
            return false;
        }
    }
    sectionName = section;
    return true;
}

bool DatabaseProfile::setValue(const QString &section, const QString &name, const QVariant &value)
{
    if (beginSection(section))
    {
        return setValue(name, value);
    }
    return false;
}

bool DatabaseProfile::setValue(const QString &name, const QVariant &value, const QString &keyName)
{
    QVariant thisValue = this->value(name);
    if (thisValue == value)
        return true;

    if (!keyName.isEmpty())
        addLogger(QString("%1%2->%3").arg(keyName).arg(thisValue.toString()).arg(value.toString()), LoggerTypeSettingsChanged);

    return updateValue(sectionName, name, value);
}

bool DatabaseProfile::updateValue(const QString &section, const QString &name, const QVariant &value)
{
    QSqlQuery query(database);
    QString sqlstr = QString("replace into %1 (key, value) values ('%2', '%3');").arg(section).arg(name).arg(value.toString());
    if (!query.exec(sqlstr))
    {
        qDebug() << query.lastError() << sqlstr;
        return false;
    }
    return true;
}


QVariant DatabaseProfile::value(const QString &key, const QVariant &defaultValue)
{
    QVariant var = defaultValue;
    QString strTable = sectionName;
    QString strKey = key;

    /*int index = key.indexOf('/');
    if (index >= 0)
    {
        strTable = key.left(index);
        strKey = key.mid(index);
    }*/

    QSqlQuery query(database);
    QString select = QString("select value from %1 where key is '%2';").arg(strTable).arg(strKey);
    if (query.exec(select))
    {
        if (query.next())
            var = query.value(0);
    }
    return var;
}

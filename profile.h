#ifndef PROFILE_H
#define PROFILE_H

#include <QSqlDatabase>
#include <QVariant>

class DatabaseProfile
{
public:
    DatabaseProfile(const QString &name = "config.db");

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

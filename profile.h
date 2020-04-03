#ifndef PROFILE_H
#define PROFILE_H

#include <QSqlDataBase>
#include <QVariant>

class DatabaseProfile
{
public:
    DatabaseProfile(const QString &name);

    bool beginSection(const QString &section);
    bool setValue(const QString &key, const QVariant &value);
    bool setValue(const QString &section, const QString &key, const QVariant &value);
    QVariant value(const QString &key, const QVariant &defaultValue = QVariant());

protected:
    bool updateValue(const QString &section, const QString &name, const QVariant &value);

private:
    QString sectionName;
    QSqlDatabase database;
};

#endif // PROFILE_H

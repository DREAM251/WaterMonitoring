#ifndef PROFILE_H
#define PROFILE_H

#include <QSqlDataBase>
#include <QVariant>

#if defined (NO_PROFILE)
#define PROFILE_INIT(dbname)
#define SAVE_VARIANT(_class, _value)
#define LOAD_VARIANT(_class, _value) QVariant(_value)
#else
#define PROFILE_INIT(dbname) Profile::initProfile(dbname)
#define SAVE_VARIANT(_class, _value) Profile::saveValue(#_class, #_value, _value)
#define LOAD_VARIANT(_class, _value) Profile::loadValue(#_class, #_value)

class Profile
{
public:
    Profile(const QString &name);

    void initProfile(const QString &dbname);

    void beginSection(const QString &section);
    void endSection();
    void saveValue(const QString &name, const QVariant &value);
    QVariant loadValue(const QString &name);

private:
    QString sectionName;
    QSqlDatabase *profileDB;
};


#endif

#endif // PROFILE_H

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
    static void initProfile(const QString &dbname);
    static void saveValue(char *_class, char *_value, const QVariant &value);
    static QVariant loadValue(char *_class, char *_value);

private:
    static QSqlDatabase *profileDB;
};


#endif

#endif // PROFILE_H

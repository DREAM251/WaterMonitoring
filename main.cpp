#include "systemwindow.h"
#include "login/userdlg.h"
#include "qfmain.h"
#include "keyboard/keyboardenter.h"
#include "profile.h"
#include "screensaver.h"
#include "common.h"
#include <QApplication>
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlError>
#include <QTextCodec>

#if defined (_MSC_VER)
#define CODE  "GB2312"
#else
#define CODE  "UTF-8"
#endif

void initDataBase();
ScreenSaver *screenSaver = NULL;

class Application : public QApplication
{
public:
    Application(int argc, char *argv[]) :
        QApplication(argc,argv){
//        setWindowIcon(QIcon(APPLICATION_ICON));
    }

    virtual bool notify(QObject *obj, QEvent *ev)
    {
        if(ev->type() == QEvent::MouseButtonPress ||
               ev->type() ==  QEvent::ZOrderChange )
        {
            if(screenSaver != NULL && screenSaver->isVisible())
                screenSaver->hideMe();
        }
        return QApplication::notify(obj,ev);
    }
};

int judge(int argc , char **)
{
    if(argc==1)
    {
        return 99;
    }
    return 0;
}

int main(int argc, char *argv[])
{
#ifdef Q_WS_QWS
    int ret = judge(argc , argv);
    if( ret )
        return ret;
#endif

    Application a(argc, argv);
    QTextCodec::setCodecForTr(QTextCodec::codecForName(CODE));
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName(CODE));
    QTextCodec::setCodecForLocale(QTextCodec::codecForName(CODE));

    screenSaver = new ScreenSaver();

#if defined (Q_WS_QWS)
    KeyBoardEnter *kb =  new KeyBoardEnter();
    a.setInputContext(kb);
#endif

    QFont font;
    font.setFamily("SimSun");
    QApplication::setFont(font);

    initDataBase();
    addLogger(QObject::tr("系统启动"));

    QFMain *f = new QFMain;
    UserDlg *user = new UserDlg;
    SystemWindow *systemw = new SystemWindow;
#if defined (Q_WS_QWS)
    f->showFullScreen();
    QObject::connect(f, SIGNAL(userTrigger()), user, SLOT(showFullScreen()));
    QObject::connect(f, SIGNAL(systemTrigger()), user, SLOT(showFullScreen()));
#else
    f->show();
    QObject::connect(f, SIGNAL(userTrigger()), user, SLOT(show()));
    QObject::connect(f, SIGNAL(systemTrigger()), user, SLOT(show()));
#endif
    user->hide();
    systemw->hide();

    QObject::connect(user, SIGNAL(login(int)), f, SLOT(login(int)));

    return a.exec();
}

void initDataBase()
{
    QSqlDatabase dbuserdata;
    if (!getUserDataBase(dbuserdata)){
        QMessageBox::warning (0, QObject::tr("Database userdata error"), dbuserdata.lastError().text());
        return;
    }

    QSqlQuery query(dbuserdata);
    query.exec("CREATE TABLE if not exists Data (ID INTEGER PRIMARY KEY AUTOINCREMENT,TimeID TEXT,"
               "A1 TEXT,A2 TEXT,A3 TEXT,A4 TEXT,A5 TEXT,A6 TEXT,A7 TEXT,A8 TEXT,A9 TEXT,"
               "B1 TEXT,B2 TEXT,B3 TEXT,B4 TEXT,B5 TEXT,B6 TEXT,B7 TEXT,B8 TEXT,B9 TEXT);");
    query.exec("CREATE TABLE if not exists Calibration(ID INTEGER PRIMARY KEY AUTOINCREMENT,TimeID TEXT,"
               "A1 TEXT,A2 TEXT,A3 TEXT,A4 TEXT,A5 TEXT,A6 TEXT,A7 TEXT,A8 TEXT,A9 TEXT,"
               "B1 TEXT,B2 TEXT,B3 TEXT,B4 TEXT,B5 TEXT,B6 TEXT,B7 TEXT,B8 TEXT,B9 TEXT);");
    query.exec("CREATE TABLE if not exists Error(ID INTEGER PRIMARY KEY AUTOINCREMENT,TimeID TEXT,"
               "A1 TEXT,A2 TEXT,A3 TEXT,A4 TEXT,A5 TEXT,A6 TEXT,A7 TEXT,A8 TEXT,A9 TEXT,"
               "B1 TEXT,B2 TEXT,B3 TEXT,B4 TEXT,B5 TEXT,B6 TEXT,B7 TEXT,B8 TEXT,B9 TEXT);");
    query.exec("CREATE TABLE if not exists Log(ID INTEGER PRIMARY KEY AUTOINCREMENT,TimeID TEXT,"
               "A1 TEXT,A2 TEXT,A3 TEXT,A4 TEXT,A5 TEXT,A6 TEXT,A7 TEXT,A8 TEXT,A9 TEXT,"
               "B1 TEXT,B2 TEXT,B3 TEXT,B4 TEXT,B5 TEXT,B6 TEXT,B7 TEXT,B8 TEXT,B9 TEXT);");
    query.exec("CREATE TABLE if not exists QC(ID INTEGER PRIMARY KEY AUTOINCREMENT,TimeID TEXT,"
               "A1 TEXT,A2 TEXT,A3 TEXT,A4 TEXT,A5 TEXT,A6 TEXT,A7 TEXT,A8 TEXT,A9 TEXT,"
               "B1 TEXT,B2 TEXT,B3 TEXT,B4 TEXT,B5 TEXT,B6 TEXT,B7 TEXT,B8 TEXT,B9 TEXT);");
    query.clear();
}

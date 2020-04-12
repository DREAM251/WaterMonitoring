#include "systemwindow.h"
#include "login/userdlg.h"
#include "qfmain.h"
#include "keyboard/keyboardenter.h"
#include <QApplication>
#include "profile.h"
#include "screensaver.h"
#include <QTextCodec>

#if defined (_MSC_VER)
#define CODE  "GB2312"
#else
#define CODE  "UTF-8"
#endif

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

    KeyBoardEnter *kb =  new KeyBoardEnter();
    a.setInputContext(kb);

    QFont font;
    font.setFamily("SimSun");
    QApplication::setFont(font);

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

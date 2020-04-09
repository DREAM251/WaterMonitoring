#include "systemwindow.h"
#include "login/userdlg.h"
#include "qfmain.h"
#include <QApplication>
#include "profile.h"
#include <QTextCodec>

#if defined (_MSC_VER)
#define CODE  "GB2312"
#else
#define CODE  "UTF-8"
#endif

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QTextCodec::setCodecForTr(QTextCodec::codecForName(CODE));

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

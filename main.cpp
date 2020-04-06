#include "funwindow.h"
#include "systemwindow.h"
#include "loginwindow.h"
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

    QFMain f;
#if defined (Q_WS_QWS)
    f.showFullScreen();
#else
    f.show();
#endif
    return a.exec();
}

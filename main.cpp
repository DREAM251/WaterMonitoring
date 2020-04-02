#include "funwindow.h"
#include "systemwindow.h"
#include "loginwindow.h"
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

    FunWindow w;
    w.show();

    return a.exec();
}

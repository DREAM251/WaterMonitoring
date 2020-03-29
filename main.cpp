#include "funwindow.h"
#include "systemwindow.h"
#include "loginwindow.h"
#include <QApplication>
#include "profile.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    PROFILE_INIT("profile.db");

    FunWindow w;
    w.show();

    return a.exec();
}

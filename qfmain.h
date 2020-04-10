#ifndef QFMAIN_H
#define QFMAIN_H

#include <QWidget>
#include <QSignalMapper>
#include <QTimer>
#include <QDateTime>
#include "elementinterface.h"
#include "querydata.h"
#include "calibframe.h"

namespace Ui {
class QFMain;
class SetUI;
}
class QFMain : public QWidget
{
    Q_OBJECT

public:
    explicit QFMain(QWidget *parent = 0);
    ~QFMain();

public slots:
    void menuClicked(int i);
    void updateStatus();
    void login(int level);

    void loadSettings();
    void saveSettings();

Q_SIGNALS:
    void systemTrigger();
    void userTrigger();

private:
    Ui::QFMain *ui;
    Ui::SetUI *setui;
    QSignalMapper *signalMapper;
    QTimer *timer;
    ElementInterface *element;
    int loginLevel;

    QSqlDatabase *userDB;
    QueryData *queryData;
    QueryData *queryCalib;
    QueryData *queryError;
    QueryData *queryLog;
    CalibFrameUser *calibframe;
};

#endif // QFMAIN_H

#ifndef QFMAIN_H
#define QFMAIN_H

#include <QWidget>
#include <QSignalMapper>
#include <QTimer>
#include <QDateTime>
#include "elementinterface.h"

namespace Ui {class QFMain;}
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

Q_SIGNALS:
    void systemTrigger();
    void userTrigger();

private:
    Ui::QFMain *ui;
    QSignalMapper *signalMapper;
    QTimer *timer;
    ElementInterface *element;
    int loginLevel;
};

#endif // QFMAIN_H

#ifndef QFMAIN_H
#define QFMAIN_H

#include <QWidget>
#include <QSignalMapper>
#include <QTimer>
#include <QDateTime>
#include "funwindow.h"

namespace Ui {class QFMain;}
class QFMain : public QWidget
{
    Q_OBJECT

public:
    explicit QFMain(QWidget *parent = 0);
    ~QFMain();

    void addChemicalModule(const QString &name);

public slots:
    void menuClicked(int i);
    void updateStatus();

private:
    Ui::QFMain *ui;
    QSignalMapper *signalMapper;
    QTimer *timer;
    FunWindow *element;
};

#endif // QFMAIN_H

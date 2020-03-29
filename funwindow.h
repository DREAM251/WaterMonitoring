#ifndef FUNWINDOW_H
#define FUNWINDOW_H

#include <QWidget>
#include "elementinterface.h"

namespace Ui {
class FunWindow;
}

class FunWindow : public QWidget
{
    Q_OBJECT

public:
    explicit FunWindow(QWidget *parent = 0);
    ~FunWindow();

private slots:
    void on_pushButton_clicked();

private:
    Ui::FunWindow *ui;
    ElementInterface *ei;
};

#endif // FUNWINDOW_H

#include "funwindow.h"
#include "ui_funwindow.h"
#include <QDebug>

FunWindow::FunWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FunWindow),
    ei(new ElementInterface(this))
{
    ui->setupUi(this);
}

FunWindow::~FunWindow()
{
    delete ui;
}

void FunWindow::on_pushButton_clicked()
{
    qDebug() <<  ei->startTask(TT_Measure);
}

#include "funwindow.h"
#include "ui_funwindow.h"
#include <QDebug>

FunWindow::FunWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FunWindow),
    ei(new ElementInterface(ET_CODCr, this))
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

void FunWindow::on_pushButton_2_clicked()
{
    qDebug() <<  ei->startTask(TT_CLEAN);

}

void FunWindow::on_pushButton_3_clicked()
{
    ei->stopTasks();
}

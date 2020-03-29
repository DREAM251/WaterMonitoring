#include "systemwindow.h"
#include "ui_systemwindow.h"

SystemWindow::SystemWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SystemWindow)
{
    ui->setupUi(this);
}

SystemWindow::~SystemWindow()
{
    delete ui;
}

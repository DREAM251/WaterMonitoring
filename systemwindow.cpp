#include "systemwindow.h"
#include "ui_systemwindow.h"
#include "common.h"
#include <QMessageBox>
#include <QProcess>

SystemWindow::SystemWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SystemWindow)
{
    ui->setupUi(this);

    ui->DateTimeEdit->setDateTime(QDateTime::currentDateTime());

    connect(ui->ScreenCalibration, SIGNAL(clicked()), this, SLOT(screenCalibration()));
    connect(ui->PlatformSelect, SIGNAL(clicked()), this, SLOT(platformSelect()));
    connect(ui->UpdateProgram, SIGNAL(clicked()), this, SLOT(updateProgram()));
    connect(ui->SetTime, SIGNAL(clicked()), this, SLOT(setTime()));
}

SystemWindow::~SystemWindow()
{
    delete ui;
}

void SystemWindow::screenCalibration()
{
}

void SystemWindow::platformSelect()
{
}

void SystemWindow::updateProgram()
{
#if !defined(Q_WS_WIN)
    DriverSelectionDialog dsd;
    dsd.addExclusiveDriver("/dev/root");
    dsd.addExclusiveDriver("/dev/mmcblk0p3");
    dsd.addExclusiveDriver("/dev/mmcblk0p7");

    if(dsd.showModule())
    {
        QString strTargetDir = dsd.getSelectedDriver();

        if(QFile::exists(strTargetDir))
        {
            QString strSource = strTargetDir + "/cmplat";
            system("mv cmplat cmplat1");
            system(QString("cp %1 .").arg(strSource).toLatin1().data());
            QString program = "./cmplat";
            QProcess *myProcess = new QProcess();

            system("chmod 777 cmplat");
            int execCode = myProcess->execute(program);
            switch(execCode)
            {
            case 99:
                QMessageBox::information(NULL, tr("提示"),tr("更新程序成功,设备自动重启！"));
                system("reboot");
                break;
            case -1:
                if (QMessageBox::question(NULL,tr("提示"),tr("警告：\n\n更新的程序为老版本程序，更新后可能会导致不可知的问题，是否强制更新？"),
                                          QMessageBox::Yes| QMessageBox::No)
                        == QMessageBox::No)
                {
                    system("mv cmplat1 cmplat");
                }
                else
                {
                    QMessageBox::information(NULL, tr("提示"),tr("更新程序成功,设备自动重启！"));
                    system("reboot");
                }
                break;
            default:
                system("mv cmplat1 cmplat");
                QMessageBox::information(NULL, tr("提示"),tr("未找到可执行程序或者可执行程序已损坏，烧写失败！"));
                break;
            }
            delete myProcess;
        }
        else
        {
            QMessageBox::warning(NULL, tr("错误") ,tr("请确认插上SD卡或U盘！"));
        }
    }
#endif
}

void SystemWindow::setTime()
{
    QDateTime dt = ui->DateTimeEdit->dateTime();
    if (setSystemTime(dt.toString("yyyy-MM-dd hh:mm:ss").toAscii().data()) == 0)
       QMessageBox::information(NULL,tr("提示"),tr("设置成功！"));
    else
       QMessageBox::warning(NULL,tr("提示"),tr("设置失败，需要以管理员身份运行程序！"));
}

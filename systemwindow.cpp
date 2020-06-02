#include "systemwindow.h"
#include "ui_systemwindow.h"
#include "common.h"
#include <QMessageBox>
#include <QProcess>
#include "iprotocol.h"

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
            QString strPipe = strTargetDir + "/pipedef.txt";
            system("mv cmplat cmplat1");
            system(QString("cp %1 .").arg(strSource).toLatin1().data());
            system(QString("cp %1 .").arg(strPipe).toLatin1().data());
            QString program = "./cmplat";
            QProcess *myProcess = new QProcess();

            system("chmod 777 cmplat");
            int execCode = myProcess->execute(program);
            switch(execCode)
            {
            case 99:
                system("sync");
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

    Sender::initPipe();
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

void SystemWindow::on_pushButton_clicked()
{
    DriverSelectionDialog dsd;
    dsd.addExclusiveDriver("/dev/root");
    dsd.addExclusiveDriver("/dev/mmcblk0p3");
    dsd.addExclusiveDriver("/dev/mmcblk0p7");

    if(dsd.showModule())
    {
        QString strTargetDir = dsd.getSelectedDriver();
        qDebug()<<QString("%1").arg(strTargetDir);

        if(QFile::exists(strTargetDir))
        {
            QString strSource1 = strTargetDir + "/zsfile";
            QString strSource2 = strTargetDir + "/zsfile/logs";

            system(QString("cp /dist/measure.txt %1").arg(strSource1).toLatin1().data());
            system(QString("cp /dist/drain.txt %1").arg(strSource1).toLatin1().data());
            system(QString("cp /dist/error.txt %1").arg(strSource1).toLatin1().data());
            system(QString("cp /dist/stop.txt %1").arg(strSource1).toLatin1().data());
            system(QString("cp /dist/initialize.txt %1").arg(strSource1).toLatin1().data());
            system(QString("cp /dist/poweron.txt %1").arg(strSource1).toLatin1().data());
            system(QString("cp /dist/wash.txt %1").arg(strSource1).toLatin1().data());
            system(QString("cp /dist/UserData.db %1").arg(strSource1).toLatin1().data());
            system(QString("cp /dist/config.db %1").arg(strSource1).toLatin1().data());
            system(QString("cp /dist/logs/mcu.logs %1").arg(strSource2).toLatin1().data());
            for(int i=1;i<=3;i++)
            {
               system(QString("cp /dist/logs/mcu%1.logs %2").arg(i).arg(strSource2).toLatin1().data());
            }


            QMessageBox::information(NULL, tr("提示"),tr("导出成功！"));
        }
    }
}

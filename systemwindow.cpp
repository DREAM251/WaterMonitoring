#include "systemwindow.h"
#include "ui_systemwindow.h"
#include "common.h"
#include <QMessageBox>
#include <QProcess>
#include <QSettings>
#include "iprotocol.h"
#include "globelvalues.h"

SystemWindow::SystemWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SystemWindow)
{
    ui->setupUi(this);

    QSettings set("platform.ini", QSettings::IniFormat);
    ui->PlatformSelect->setCurrentIndex(set.value("index", 0).toInt());
    ui->DateTimeEdit->setDateTime(QDateTime::currentDateTime());

    connect(ui->ScreenCalibration, SIGNAL(clicked()), this, SLOT(screenCalibration()));
    connect(ui->PlatformSelect, SIGNAL(currentIndexChanged(int)), this, SLOT(platformSelect(int)));
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

void SystemWindow::platformSelect(int i)
{
    QSettings set("platform.ini", QSettings::IniFormat);

    set.setValue("index", i);
    switch (i)
    {
    case 0: set.setValue("path", "CODcr/");break;
    case 1: set.setValue("path", "CODcrHCl/");break;
    case 2: set.setValue("path", "NH3N/");break;
    case 3: set.setValue("path", "TP/");break;
    case 4: set.setValue("path", "TN/");break;
    }

    if (QMessageBox::question(NULL, tr("需要重启"), tr("平台变更，需要重启才能生效，是否立即重启？"),
                          QMessageBox::No | QMessageBox::Yes, QMessageBox::No)
            == QMessageBox::Yes)
        system("reboot");
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

        if(QFile::exists(strTargetDir))
        {
            QString strSource1 = strTargetDir + "/zsfile";
            QString strSource2 = strTargetDir + "/zsfile/logs";

            system(QString("cp /dist/%1/measure.txt %2").arg(elementPath).arg(strSource1).toLatin1().data());
            system(QString("cp /dist/%1/drain.txt %2").arg(elementPath).arg(strSource1).toLatin1().data());
            system(QString("cp /dist/%1/error.txt %2").arg(elementPath).arg(strSource1).toLatin1().data());
            system(QString("cp /dist/%1/stop.txt %2").arg(elementPath).arg(strSource1).toLatin1().data());
            system(QString("cp /dist/%1/initialize.txt %2").arg(elementPath).arg(strSource1).toLatin1().data());
            system(QString("cp /dist/%1/poweron.txt %2").arg(elementPath).arg(strSource1).toLatin1().data());
            system(QString("cp /dist/%1/wash.txt %2").arg(elementPath).arg(strSource1).toLatin1().data());
            system(QString("cp /dist/%1/UserData.db %2").arg(elementPath).arg(strSource1).toLatin1().data());
            system(QString("cp /dist/%1/config.db %2").arg(elementPath).arg(strSource1).toLatin1().data());
            system(QString("cp /dist/%1/logs/mcu.log %2").arg(elementPath).arg(strSource2).toLatin1().data());
            for(int i=1;i<=3;i++)
            {
               system(QString("cp /dist/%1/logs/mcu%2.log %3").arg(elementPath).arg(i).arg(strSource2).toLatin1().data());
            }


            QMessageBox::information(NULL, tr("提示"),tr("导出成功！"));
        }
    }
}

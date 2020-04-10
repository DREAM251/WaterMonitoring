#include "modbusmodule.h"
#include "ui_modbusmodule.h"
#include <QSettings>
#include <QDebug>
#include <QDateTime>
#include <QTextCodec>
#include <QMessageBox>
#include "defines.h"
#include "profile.h"

ModbusProcesser::ModbusProcesser(QObject *parent) :
    NTModbusSlave(parent)
{
}

//打开端口
bool ModbusProcesser::OpenPort()
{
    DatabaseProfile profile;
    profile.beginSection("modbus");
    setPortName(EXT_PORT);
    switch(profile.value("baud", 0).toInt()){
    case 1:setBaudRate("38400");break;
    case 2:setBaudRate("115200");break;
    default:setBaudRate("9600");break;
    }
    switch(profile.value("data", 0).toInt()){
    case 1:setDataBits("Data7");break;
    case 2:setDataBits("Data6");break;
    case 3:setDataBits("Data5");break;
    default:setDataBits("Data8");break;
    }
    switch(profile.value("parity", 0).toInt()){
    case 1:setParity("OddParity");break;
    case 2:setParity("EvenParity");break;
    default:setParity("NoParity");break;
    }
    switch(profile.value("stop", 0).toInt()){
    case 1:setStopBits("TwoStop");break;
    default:setStopBits("OneStop");break;
    }
    setFlowControl("NoFlowControl");
    setTimeout(0, 50);
    SetDeviceAddress(profile.value("addr", "1").toInt());

    if (!Start())
    {
        QVector<QString> vs;
        vs << tr("警告") << tr("对外通信串口(%1)不存在或者被其他应用程序占用").arg(EXT_PORT);
        return false;
    }
    return true;
}

//关闭端口
void ModbusProcesser::ClosePort()
{
    Stop();
}

//读取系统参数
unsigned char ModbusProcesser::GetSystemHoldRegister(unsigned short first, int counts, std::vector<unsigned short> &values)
{
    unsigned short address = getSlaveAddress();
    if(address == GetDeviceAddress())
    {
        DatabaseProfile profile;
        for (int i = 0; i < counts; i++)
        {
            unsigned short ret = 0;
            switch (first + i)
            {
            case 0: break;
            default:
                break;
            }

            values.push_back(ret);
        }
        return NO_EXCEPTION;
    }
    else
    {
        return SLAVE_DEVICE_FAILURE;
    }
}

//设置系统参数
unsigned char ModbusProcesser::SetSystemHoldRegister(unsigned short first , std::vector<unsigned short> &values)
{
    unsigned short address = getSlaveAddress();
    if (address == GetDeviceAddress())
    {
        for (size_t i = 0; i < values.size(); i++)
        {
            switch (first + i){
            case 0: break;
            default:
                break;
            }
        }
    }
    return NO_EXCEPTION;
}

/////////////////////////////////////////////////////////////

ModbusModule::ModbusModule(QFrame *parent) : QFrame(parent) ,
    ui(new Ui::ModbusModule)
{
    ui->setupUi(this);
    processer = new ModbusProcesser;
    LoadParameter();
    showCommand(-1);

    connect(processer,SIGNAL(RecvFinish()),this,SLOT(slotRecv()));
    connect(processer,SIGNAL(SendFinish()),this,SLOT(slotSend()));
    connect(ui->pbSave,SIGNAL(clicked()),this,SLOT(slotSave()));
    connect(processer,SIGNAL(valueChange()),this,SIGNAL(valueChange()));
    connect(ui->cbShowCommands,SIGNAL(clicked()),this,SLOT(slotShowCMD()));
    //on_checkBox_clicked();
    processer->OpenPort();

    ui->lbInfo->setText(tr("等待中..."));
}

ModbusModule::~ModbusModule()
{
    delete ui;
}

//读取配置
void ModbusModule::LoadParameter()
{
    DatabaseProfile profile;
    profile.beginSection("modbus");
    ui->sbAddr->setValue(profile.value("addr", "1").toInt());
    ui->cbBuad->setCurrentIndex(profile.value("baud", 0).toInt());
    ui->cbData->setCurrentIndex(profile.value("data", 0).toInt());
    ui->cbParity->setCurrentIndex(profile.value("parity", 0).toInt());
    ui->cbStop->setCurrentIndex(profile.value("stop", 0).toInt());
}

//保存配置
void ModbusModule::SaveParameter()
{
    DatabaseProfile profile;
    profile.beginSection("modbus");
    profile.setValue("addr", ui->sbAddr->value());
    profile.setValue("baud", ui->cbBuad->currentIndex());
    profile.setValue("data", ui->cbData->currentIndex());
    profile.setValue("parity", ui->cbParity->currentIndex());
    profile.setValue("stop", ui->cbStop->currentIndex());
}

void ModbusModule::slotRecv()
{
    int code = processer->comErrorCode;
    if(code == NO_EXCEPTION)
        ui->lbInfo->setText(tr("已连接"));
    else
        ui->lbInfo->setText(tr("接收数据异常，异常代码%1").arg(code));
    showCommand(0);
}

void ModbusModule::slotSend()
{
    showCommand(1);
}

//打开关闭端口
void ModbusModule::slotSave()
{
    if(processer->IsReady())
        processer->ClosePort();

    SaveParameter();
    if(processer->OpenPort())
        ui->lbInfo->setText(tr("打开端口成功"));
    else
        ui->lbInfo->setText(tr("打开端口失败"));
}

void ModbusModule::slotShowCMD()
{
    showCommand(0);
}

void ModbusModule::showCommand(int i)
{
    if(i == 1)
    {
        modbusLogger()->info(QString::fromLocal8Bit(processer->bySend.toHex()));
    }
    else if (i == 0)
    {
        modbusLogger()->info(QString::fromLocal8Bit(processer->byRecv.toHex()));
    }

    if (ui->cbShowCommands->isChecked())
    {
        ui->lbInfo_2->setVisible(true);
        ui->lbInfo_3->setVisible(true);
        ui->lbSend->setVisible(true);
        ui->lbRecv->setVisible(true);

        if(i == 1)
        {
            ui->lbSend->setText("Send:" + QDateTime::currentDateTime().toString("hhmmss.zzz ") + processer->bySend.toHex());
        }
        else if (i == 0)
            ui->lbRecv->setText("Recv:" + QDateTime::currentDateTime().toString("hhmmss.zzz ") + processer->byRecv.toHex());
    }
    else
    {
        ui->lbInfo_2->setVisible(false);
        ui->lbInfo_3->setVisible(false);
        ui->lbSend->setVisible(false);
        ui->lbRecv->setVisible(false);
    }
}


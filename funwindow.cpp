#include "funwindow.h"
#include "ui_funwindow.h"
#include <QDebug>

FunWindow::FunWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FunWindow),
    ei(new ElementInterface(ET_CODCr, this))
{
    ui->setupUi(this);

    struct ColumnInfo aa[] = {
    {QObject::tr("执行步骤"),4,"1000"},
    {QObject::tr("单步时间"),4,"0003"},
    {QObject::tr("时间关联"),2,"00",ColumnInfo::CDT_Combox,tr("无,时间1,时间2,时间3,时间4")},
    {QObject::tr("温度关联"),2,"00",ColumnInfo::CDT_Combox,tr("无,温度1,温度2,温度3,温度4")},
    {QObject::tr("循环关联"),2,"00",ColumnInfo::CDT_Combox,tr("无,循环1开始,循环1结束,循环2开始,循环2结束")},
    {QObject::tr("前门磁"),1,"0"},
    {QObject::tr("后门磁"),1,"0"},
    {QObject::tr("清洗液位1"),1,"0"},
    {QObject::tr("清洗液位2"),1,"0"},
    {QObject::tr("废液液位1"),1,"0"},
    {QObject::tr("废液液位2"),1,"0"},
    {QObject::tr("五参数液位"),1,"0"},
    {QObject::tr("输入备留1"),1,"0"},
    {QObject::tr("输入备留2"),1,"0"},
    {QObject::tr("输入备留3"),1,"0"},
    {QObject::tr("水泵2"),1,"0"},
    {QObject::tr("五参数"),1,"0"},
    {QObject::tr("风机1"),1,"0"},
    {QObject::tr("风机2"),1,"0"},
    {QObject::tr("水样阀"),1,"0"},
    {QObject::tr("清洗阀"),1,"0"},
    {QObject::tr("气吹阀1"),1,"0"},
    {QObject::tr("气吹阀2"),1,"0"},
    {QObject::tr("气吹阀3"),1,"0"},
    {QObject::tr("气吹阀4"),1,"0"},
    {QObject::tr("输出开关量1"),1,"0"},
    {QObject::tr("输出开关量2"),1,"0"},
    {QObject::tr("输出备留"),1,"0"},
    {QObject::tr("留样排阀"),1,"0"},
    {QObject::tr("留样蠕动泵"),1,"0"},
    {QObject::tr("保留"),34,"0000000000000000000000000000000000"},
    {QObject::tr("单步时间"),4,"0000"},
    {QObject::tr("温度"),4,"0000"},
    {QObject::tr("湿度"),2,"00"},
    {QObject::tr("校验"),2,"00"},
    {QObject::tr("分隔符"),1,";"},
    {QObject::tr("注释代码"),16,"0000000000000000"}};

    QList<ColumnInfo> ci ;
    int lines = sizeof(aa)/sizeof(struct ColumnInfo);
    for(int i=0;i<lines;i++){
        ci << aa[i];
    }

    CommondFileInfo bb[] = {{tr("测量"), "measure.txt"},
                            {tr("清洗"),  "clean.txt"},
                            {tr("初始化"),  "initial.txt"},
                            {tr("停止"),  "stop.txt"}};
    QList<CommondFileInfo> cfi;
    lines = sizeof(bb)/sizeof(struct CommondFileInfo);
    for(int i=0;i<lines;i++){
        cfi << bb[i];
    }

    instructionEditor = new InstructionEditor(ci, cfi);
    ui->tabWidget->addTab(instructionEditor, tr("命令编辑"));
}

FunWindow::~FunWindow()
{
    delete ui;
    delete instructionEditor;
}

void FunWindow::on_pushButton_clicked()
{
    qDebug() <<  ei->startTask(TT_Measure);
}

void FunWindow::on_pushButton_2_clicked()
{
    qDebug() <<  ei->startTask(TT_Clean);

}

void FunWindow::on_pushButton_3_clicked()
{
    ei->stopTasks();
}

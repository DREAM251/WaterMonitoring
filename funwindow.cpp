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
    {QObject::tr("指令编号"),4,"1001"},
    {QObject::tr("执行时间"),4,"0003"},
    {QObject::tr("蠕动泵"),1,"0"},
    {QObject::tr("转速"),2,"20"},
    {QObject::tr("泵2"),1,"0"},
    {QObject::tr("十通阀1"),1,"0"},
    {QObject::tr("十通阀2"),1,"0"},
    {QObject::tr("阀1"),1,"0"},
    {QObject::tr("阀2"),1,"0"},
    {QObject::tr("阀3"),1,"0"},
    {QObject::tr("阀4"),1,"0"},
    {QObject::tr("阀5"),1,"0"},
    {QObject::tr("阀6"),1,"0"},
    {QObject::tr("阀7"),1,"0"},
    {QObject::tr("外置泵"),1,"0"},
    {QObject::tr("外控1"),1,"0"},
    {QObject::tr("外控2"),1,"0"},
    {QObject::tr("外控3"),1,"0"},
    {QObject::tr("风扇"),1,"0"},
    {QObject::tr("设置液位"),1,"0"},
    {QObject::tr("加热温度"),4,"0000"},
    {QObject::tr("预留"),4,"0000"},
    {QObject::tr("分隔符"),1,":"},
    {QObject::tr("时间关联"),2,"00",ColumnInfo::CDT_Combox,tr("无,时间1,时间2,时间3,时间4")},
    {QObject::tr("额外时间"),4,"0000"},
    {QObject::tr("温度关联"),2,"00",ColumnInfo::CDT_Combox,tr("无,温度1,温度2,温度3,温度4")},
    {QObject::tr("循环关联"),2,"00",ColumnInfo::CDT_Combox,tr("无,循环1开始,循环1结束,循环2开始,循环2结束")},
    {QObject::tr("判定步骤"),1,"0",ColumnInfo::CDT_Combox,tr("无,液位到达判定,液位异常判定,升温到达判定,升温异常判定,降温到达判定,升温异常判定")},
    {QObject::tr("采样"),1,"0",ColumnInfo::CDT_Combox,tr("无,采集入射光强,采集出射光强")},
    {QObject::tr("注释代码"),2,"00",ColumnInfo::CDT_Combox,tr("无,空闲,**润洗,进**,消解,降温,排空反应室,流路排空,"
                                                          "流路清洗,开启外置水泵,空白检测,显色检测,排空水样管,回抽,"
                                                          "温度检测,多阀清洗,进短**,进长**,光电调节")}};

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

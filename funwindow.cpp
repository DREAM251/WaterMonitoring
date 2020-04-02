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
    {QObject::tr("ִ�в���"),4,"1000"},
    {QObject::tr("����ʱ��"),4,"0003"},
    {QObject::tr("ʱ�����"),2,"00",ColumnInfo::CDT_Combox,tr("��,ʱ��1,ʱ��2,ʱ��3,ʱ��4")},
    {QObject::tr("�¶ȹ���"),2,"00",ColumnInfo::CDT_Combox,tr("��,�¶�1,�¶�2,�¶�3,�¶�4")},
    {QObject::tr("ѭ������"),2,"00",ColumnInfo::CDT_Combox,tr("��,ѭ��1��ʼ,ѭ��1����,ѭ��2��ʼ,ѭ��2����")},
    {QObject::tr("ǰ�Ŵ�"),1,"0"},
    {QObject::tr("���Ŵ�"),1,"0"},
    {QObject::tr("��ϴҺλ1"),1,"0"},
    {QObject::tr("��ϴҺλ2"),1,"0"},
    {QObject::tr("��ҺҺλ1"),1,"0"},
    {QObject::tr("��ҺҺλ2"),1,"0"},
    {QObject::tr("�����Һλ"),1,"0"},
    {QObject::tr("���뱸��1"),1,"0"},
    {QObject::tr("���뱸��2"),1,"0"},
    {QObject::tr("���뱸��3"),1,"0"},
    {QObject::tr("ˮ��2"),1,"0"},
    {QObject::tr("�����"),1,"0"},
    {QObject::tr("���1"),1,"0"},
    {QObject::tr("���2"),1,"0"},
    {QObject::tr("ˮ����"),1,"0"},
    {QObject::tr("��ϴ��"),1,"0"},
    {QObject::tr("������1"),1,"0"},
    {QObject::tr("������2"),1,"0"},
    {QObject::tr("������3"),1,"0"},
    {QObject::tr("������4"),1,"0"},
    {QObject::tr("���������1"),1,"0"},
    {QObject::tr("���������2"),1,"0"},
    {QObject::tr("�������"),1,"0"},
    {QObject::tr("�����ŷ�"),1,"0"},
    {QObject::tr("�����䶯��"),1,"0"},
    {QObject::tr("����"),34,"0000000000000000000000000000000000"},
    {QObject::tr("����ʱ��"),4,"0000"},
    {QObject::tr("�¶�"),4,"0000"},
    {QObject::tr("ʪ��"),2,"00"},
    {QObject::tr("У��"),2,"00"},
    {QObject::tr("�ָ���"),1,";"},
    {QObject::tr("ע�ʹ���"),16,"0000000000000000"}};

    QList<ColumnInfo> ci ;
    int lines = sizeof(aa)/sizeof(struct ColumnInfo);
    for(int i=0;i<lines;i++){
        ci << aa[i];
    }

    CommondFileInfo bb[] = {{tr("����"), "measure.txt"},
                            {tr("��ϴ"),  "clean.txt"},
                            {tr("��ʼ��"),  "initial.txt"},
                            {tr("ֹͣ"),  "stop.txt"}};
    QList<CommondFileInfo> cfi;
    lines = sizeof(bb)/sizeof(struct CommondFileInfo);
    for(int i=0;i<lines;i++){
        cfi << bb[i];
    }

    instructionEditor = new InstructionEditor(ci, cfi);
    ui->tabWidget->addTab(instructionEditor, tr("����༭"));
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

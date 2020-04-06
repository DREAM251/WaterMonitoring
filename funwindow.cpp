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
    {QObject::tr("ָ����"),4,"1001"},
    {QObject::tr("ִ��ʱ��"),4,"0003"},
    {QObject::tr("�䶯��"),1,"0"},
    {QObject::tr("ת��"),2,"20"},
    {QObject::tr("��2"),1,"0"},
    {QObject::tr("ʮͨ��1"),1,"0"},
    {QObject::tr("ʮͨ��2"),1,"0"},
    {QObject::tr("��1"),1,"0"},
    {QObject::tr("��2"),1,"0"},
    {QObject::tr("��3"),1,"0"},
    {QObject::tr("��4"),1,"0"},
    {QObject::tr("��5"),1,"0"},
    {QObject::tr("��6"),1,"0"},
    {QObject::tr("��7"),1,"0"},
    {QObject::tr("���ñ�"),1,"0"},
    {QObject::tr("���1"),1,"0"},
    {QObject::tr("���2"),1,"0"},
    {QObject::tr("���3"),1,"0"},
    {QObject::tr("����"),1,"0"},
    {QObject::tr("����Һλ"),1,"0"},
    {QObject::tr("�����¶�"),4,"0000"},
    {QObject::tr("Ԥ��"),4,"0000"},
    {QObject::tr("�ָ���"),1,":"},
    {QObject::tr("ʱ�����"),2,"00",ColumnInfo::CDT_Combox,tr("��,ʱ��1,ʱ��2,ʱ��3,ʱ��4")},
    {QObject::tr("����ʱ��"),4,"0000"},
    {QObject::tr("�¶ȹ���"),2,"00",ColumnInfo::CDT_Combox,tr("��,�¶�1,�¶�2,�¶�3,�¶�4")},
    {QObject::tr("ѭ������"),2,"00",ColumnInfo::CDT_Combox,tr("��,ѭ��1��ʼ,ѭ��1����,ѭ��2��ʼ,ѭ��2����")},
    {QObject::tr("�ж�����"),1,"0",ColumnInfo::CDT_Combox,tr("��,Һλ�����ж�,Һλ�쳣�ж�,���µ����ж�,�����쳣�ж�,���µ����ж�,�����쳣�ж�")},
    {QObject::tr("����"),1,"0",ColumnInfo::CDT_Combox,tr("��,�ɼ������ǿ,�ɼ������ǿ")},
    {QObject::tr("ע�ʹ���"),2,"00",ColumnInfo::CDT_Combox,tr("��,����,**��ϴ,��**,����,����,�ſշ�Ӧ��,��·�ſ�,"
                                                          "��·��ϴ,��������ˮ��,�հ׼��,��ɫ���,�ſ�ˮ����,�س�,"
                                                          "�¶ȼ��,�෧��ϴ,����**,����**,������")}};

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

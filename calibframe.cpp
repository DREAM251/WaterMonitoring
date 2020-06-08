#include "calibframe.h"
#include "ui_calibframe.h"
#include <QSettings>
#include <QDebug>
#include <QMessageBox>

#include <QSqlQuery>
#include <math.h>
#include "datafit.h"
#include "defines.h"
#include "profile.h"

CalibFrame::CalibFrame(const QString &profile, QWidget *parent) :
    QFrame(parent),
    ui(new Ui::CalibFrame),
    num(-1),
    profile(profile)
{
    ui->setupUi(this);
    QDoubleSpinBox *pConc[] = {ui->dsbConc0,ui->dsbConc1,ui->dsbConc2,ui->dsbConc3,ui->dsbConc4,ui->dsbConc5};
    QLineEdit *pAbs[] = {ui->leAbs0,ui->leAbs1,ui->leAbs2,ui->leAbs3,ui->leAbs4,ui->leAbs5};
    QComboBox *pRange[] = {ui->cbRange0,ui->cbRange1,ui->cbRange2,ui->cbRange3,ui->cbRange4,ui->cbRange5};
    QComboBox *pPipe[] = {ui->cbPipe0,ui->cbPipe1,ui->cbPipe2,ui->cbPipe3,ui->cbPipe4,ui->cbPipe5};
    QSpinBox *pSample[] = {ui->sbSample0,ui->sbSample1,ui->sbSample2,ui->sbSample3,ui->sbSample4,ui->sbSample5};
    QSpinBox *pWater[] = {ui->sbWater0,ui->sbWater1,ui->sbWater2,ui->sbWater3,ui->sbWater4,ui->sbWater5};
    QComboBox *pSelect[] = {ui->cbSelect0,ui->cbSelect1,ui->cbSelect2,ui->cbSelect3,ui->cbSelect4,ui->cbSelect5};
    for(int i=0;i<SAMPLE_COUNT;i++){
        pdsbConc[i] = pConc[i];
        pleAbs[i] = pAbs[i];
        pcbRange[i] = pRange[i];
        pcbPipe[i] = pPipe[i];
        psbSample[i] = pSample[i];
        psbWater[i] = pWater[i];
        pcbSelect[i]  =pSelect[i];
        samples[i].no = i;
    }

    samplelow=waterlow=samplehigh=waterhigh=3;
    current=-1;

    connect(ui->pbSave,SIGNAL(clicked()),this,SLOT(slot_save()));
    connect(ui->pbDo,SIGNAL(clicked()),this,SLOT(slot_do()));
    connect(ui->pbTrain,SIGNAL(clicked()),this,SLOT(slot_train()));
    connect(ui->pbReset,SIGNAL(clicked()),this,SLOT(slot_reset()));
    for(int i=0;i<SAMPLE_COUNT;i++)
        connect(pcbRange[i],SIGNAL(currentIndexChanged(int)),this,SLOT(slot_range(int)));
}

CalibFrame::~CalibFrame()
{
    delete ui;
}

void CalibFrame::enablelevel(int level)
{
    if(level == 0)
    {
        ui->pbReset->setEnabled(false);
        ui->pbTrain->setEnabled(false);
        ui->pbSave->setEnabled(false);
        ui->pbDo->setEnabled(false);
    }

    else if(level == 1)
    {
        ui->pbReset->setEnabled(false);
        ui->pbTrain->setEnabled(false);
        ui->pbSave->setEnabled(true);
        ui->pbDo->setEnabled(true);
    }

    else
    {
        ui->pbReset->setEnabled(true);
        ui->pbTrain->setEnabled(true);
        ui->pbSave->setEnabled(true);
        ui->pbDo->setEnabled(true);
    }
}

void CalibFrame::renewParamsFromUI()
{
    for(int i(0);i<SAMPLE_COUNT;i++)
    {
        samples[i].conc = pdsbConc[i]->value();
//        samples[i].abs = pleAbs[i]->text().toFloat();
        samples[i].range = pcbRange[i]->currentIndex();
        samples[i].pipe = pcbPipe[i]->currentIndex();
        samples[i].ratio[0] = psbSample[i]->value();
        samples[i].ratio[1] = psbWater[i]->value();
        samples[i].mode = pcbSelect[i]->currentIndex();
    }
}

void CalibFrame::loadParams()
{
    DatabaseProfile dbprofile;
    if (dbprofile.beginSection(profile))
    {
        for(int i(0);i<SAMPLE_COUNT;i++)
        {
            samples[i].conc = dbprofile.value(QString("%1/conc").arg(i)).toFloat();
            samples[i].abs = dbprofile.value(QString("%1/abs").arg(i)).toFloat();
            samples[i].range = dbprofile.value(QString("%1/range").arg(i)).toInt();
            samples[i].pipe = dbprofile.value(QString("%1/pipe").arg(i)).toInt();
            samples[i].ratio[0] = dbprofile.value(QString("%1/sample").arg(i)).toInt();
            samples[i].ratio[1] = dbprofile.value(QString("%1/water").arg(i)).toInt();
            samples[i].mode = dbprofile.value(QString("%1/mode").arg(i)).toInt();
        }
    }
}

void CalibFrame::saveParams()
{
    DatabaseProfile dbprofile;
    if (dbprofile.beginSection(profile))
    {
        for(int i(0);i<SAMPLE_COUNT;i++)
        {
            dbprofile.setValue(QString("%1/conc").arg(i), samples[i].conc);
            dbprofile.setValue(QString("%1/abs").arg(i), samples[i].abs);
            dbprofile.setValue(QString("%1/range").arg(i), samples[i].range);
            dbprofile.setValue(QString("%1/pipe").arg(i), samples[i].pipe);
            dbprofile.setValue(QString("%1/sample").arg(i), samples[i].ratio[0]);
            dbprofile.setValue(QString("%1/water").arg(i), samples[i].ratio[1]);
            dbprofile.setValue(QString("%1/mode").arg(i), samples[i].mode);
        }
    }
}

//计算某个标样的稀释比例
float CalibFrame::getDilutionRatio(int i)
{
    float ratio0=samples[i].ratio[0];
    float ratio1=samples[i].ratio[1];
    return ratio0/(ratio0+ratio1);
}

void CalibFrame::renewUI()
{
    for(int i(0);i<SAMPLE_COUNT;i++)
    {
        pdsbConc[i]->setValue(samples[i].conc);
        pleAbs[i]->setText(QString("%1").arg(samples[i].abs,0,'f',4));
        pcbRange[i]->setCurrentIndex(samples[i].range);
        pcbPipe[i]->setCurrentIndex(samples[i].pipe);
        if(samples[i].range == 2)
        {
            psbSample[i]->setReadOnly(false);
            psbWater[i]->setReadOnly(false);
        }else{
            psbSample[i]->setReadOnly(true);
            psbWater[i]->setReadOnly(true);
        }
        psbSample[i]->setValue(samples[i].ratio[0]);
        psbWater[i]->setValue(samples[i].ratio[1]);
        pcbSelect[i]->setCurrentIndex(samples[i].mode);
    }
}

void CalibFrame::reset()
{

}
void CalibFrame::on_pushButton_2_clicked()
{
    num=2;
}

void CalibFrame::on_pushButton_clicked()
{
    num=1;
}

void CalibFrame::setRange(int sel, QString name)
{
    if(sel<0 || sel>=3){
        return ;
    }
    rangeName[sel] = name;
    for(int i=0;i<SAMPLE_COUNT;i++)
    {
        pcbRange[i]->setItemText(i, rangeName[sel]);
    }
}

void CalibFrame::addPipeName(QString name)
{
    if(name.isEmpty())
        return;

    for(int i=0;i<SAMPLE_COUNT;i++)
    {
        pcbPipe[i]->addItem(name);
    }
}

void CalibFrame::setSampleLow(int s1, int s2)
{
    samplelow=s1,waterlow=s2;
}

void CalibFrame::setSampleHigh(int s1, int s2)
{
    samplehigh=s1,waterhigh=s2;
}

void CalibFrame::slot_save()
{
    renewParamsFromUI();
    saveParams();
    renewUI();

    QMessageBox::information(NULL,tr("提示"),tr("保存成功！"));
}


void CalibFrame::slot_do()
{
    renewUI();
    emit signal_do();
}

void CalibFrame::slot_train()
{
    if(!calc())
        QMessageBox::warning(NULL,tr("警告"),tr("样本异常，拟合失败！"));

    saveParams();
    renewUI();
}

void CalibFrame::slot_reset()
{
    if(QMessageBox::question(NULL, tr("提示"), tr("重置后，拟合参数会设定为默认值，但标定数据不会受到影响，是否确定？"),
                             QMessageBox::Yes|QMessageBox::No)
            == QMessageBox::No)
        return;
    reset();
    saveParams();
    renewUI();
}

void CalibFrame::slot_range(int range)
{
    if(!psbSample[0]->isVisible())
        return;
    int choose = QObject::sender()->objectName().right(1).toInt();
    if(choose<0||choose>=SAMPLE_COUNT)
    {
        logger()->info(QString("error in slot_range():%1").arg(choose));
        return;
    }
    //使能
    psbSample[choose]->setReadOnly(true);
    psbWater[choose]->setReadOnly(true);
    if(range == 0)
    {
        if(choose==0){
            psbSample[choose]->setValue(0);
            psbWater[choose]->setValue(samplelow+waterlow);
        }else{
            psbSample[choose]->setValue(samplelow);
            psbWater[choose]->setValue(waterlow);
        }
    }
    else if(range == 1)
    {
        if(choose==0){
            psbSample[choose]->setValue(0);
            psbWater[choose]->setValue(samplehigh+waterhigh);
        }else{
            psbSample[choose]->setValue(samplehigh);
            psbWater[choose]->setValue(waterhigh);
        }
    }
    else if(range == 2)
    {
        psbSample[choose]->setReadOnly(false);
        psbWater[choose]->setReadOnly(false);
    }
}


int CalibFrame::getNextCalib()
{
    current = -1;
    for(int i(0);i<SAMPLE_COUNT;i++)
    {
        if(samples[i].mode==WAIT_CALIB)
        {
            current = i;
            break;
        }
    }
    return current;
}

void CalibFrame::setVLight(int A1, int A2)
{
    if(current<0){
        return ;
    }
    if(A1<=0)A1=1;
    if(A2<=0)A2=1;
    double fA1 = (double)A1;
    double fA2 = (double)A2;

    samples[current].mode = HAVE_CALIB;
    samples[current].abs = log10(fA1/fA2);

    //检测是否标定结束，如果结束则自动拟合数据
    for(int i(0);i<SAMPLE_COUNT;i++)
        if(samples[i].mode==WAIT_CALIB)
            goto CALC_END;
    calc();

CALC_END:
    saveParams();
    renewUI();
}
void CalibFrame::setHaveCalib()
{
    for(int i(0);i<SAMPLE_COUNT;i++)
    {
        if(samples[i].mode==HAVE_CALIB)
        {
            pcbSelect[i]->setCurrentIndex(WAIT_CALIB);
        }
    }
    renewParamsFromUI();
    saveParams();
    renewUI();
}
/**
 * @brief CalibFrame::setWaitToHaveCalib
 *设置待标定为已标定
 */
void CalibFrame::setWaitToHaveCalib()
{
    for(int i(0);i<SAMPLE_COUNT;i++)
    {
        if(samples[i].mode==WAIT_CALIB)
        {
            pcbSelect[i]->setCurrentIndex(HAVE_CALIB);
        }
    }
    renewParamsFromUI();
    saveParams();
    renewUI();
}

float CalibFrame::getConc(int select)
{
    if(select<0||select>=SAMPLE_COUNT){
        return -1.0;
    }
    return samples[select].conc;
}

float CalibFrame::getAbs(int select)
{
    if(select<0||select>=SAMPLE_COUNT){
        return -1.0;
    }
    return samples[select].abs;
}

int CalibFrame::getPipe(int select)
{
    if(select<0||select>=SAMPLE_COUNT){
        return 0;
    }
    return samples[select].pipe;
}
/**
 * @brief CalibFrame::getRange
 * @param select
 * @return
 */
int CalibFrame::getRange(int select)
{
    if(select<0||select>=SAMPLE_COUNT){
        return 0;
    }
    return samples[select].range;
}

int CalibFrame::getSample(int select)
{
    if(select<0||select>=SAMPLE_COUNT){
        return -1.0;
    }
    return samples[select].ratio[0];
}

int CalibFrame::getWater(int select)
{
    if(select<0||select>=SAMPLE_COUNT){
        return -1.0;
    }
    return samples[select].ratio[1];
}

QString CalibFrame::getPipeName(int select)
{
    if(select<0||select>=SAMPLE_COUNT){
        return QString("error");
    }
    int i = samples[select].pipe;
    return pcbPipe[select]->itemText(i);
}

QString CalibFrame::getRangeName(int select)
{
    if(select<0||select>=SAMPLE_COUNT){
        return QString("error");
    }
    int i = samples[select].range;
    return pcbRange[select]->itemText(i);
}

CalibFrameUser::CalibFrameUser(QWidget *parent) :
    CalibFrame("usercalibration", parent)
{
}

bool CalibFrameUser::calc()
{
    loadParams();
    //提取已标定的项目
    QList<struct oneSample*> unit;
    unit.clear();
    for(int i(0);i<SAMPLE_COUNT;i++)
        if(samples[i].mode==HAVE_CALIB)
            unit.push_back(&samples[i]);

    //计算稀释后的浓度
    int count = unit.count();
    double realconc[SAMPLE_COUNT];
    double theoryconc[SAMPLE_COUNT];
    for(int i=0;i<count;i++)
    {
        //实际浓度
        double orgConc=unit.at(i)->conc;
        realconc[i]=orgConc*getDilutionRatio(unit.at(i)->no);
        //理论浓度
        double abs=unit.at(i)->abs;
        theoryconc[i]=qfitA*abs*abs + qfitB*abs + qfitC;
    }
    //拟合
    if(Linefit(theoryconc,realconc,count,lfitA,lfitB)){
        RLinefit(theoryconc,realconc,count,lfitA,lfitB,lfitR);
        return true;
    }
    else
        return false;

}

void CalibFrameUser::saveParams()
{
    CalibFrame::saveParams();

    DatabaseProfile dbprofile;
    if (dbprofile.beginSection("measure"))
    {
        dbprofile.setValue("lineark",lfitA);
        dbprofile.setValue("linearb",lfitB);
        dbprofile.setValue("linearr",lfitR);
    }
}

void CalibFrameUser::loadParams()
{
    CalibFrame::loadParams();

    DatabaseProfile dbprofile;
    if (dbprofile.beginSection("measure"))
    {
        lfitA=dbprofile.value("lineark",1).toDouble();
        lfitB=dbprofile.value("linearb",0).toDouble();
        lfitR=dbprofile.value("linearr",1).toDouble();

        qfitA=dbprofile.value("quada",0).toDouble();
        qfitB=dbprofile.value("quadb",1).toDouble();
        qfitC=dbprofile.value("quadc",0).toDouble();
        qfitR=dbprofile.value("quadr",1).toDouble();
    }
}

void CalibFrameUser::renewUI()
{
    CalibFrame::renewUI();
    ui->lbResult->setText(tr("拟合结果：k=%1  b=%2  R=%3").arg(lfitA).arg(lfitB).arg(lfitR));
}

void CalibFrameUser::reset()
{
    lfitA = 1;
    lfitB = 0;
    lfitR = 1;
}

CalibFrameFactory::CalibFrameFactory(QWidget *parent):
    CalibFrame(("factorycalib"), parent)
{
}

bool CalibFrameFactory::calc()
{
    //提取已标定的项目
    QList<struct oneSample*> unit;
    unit.clear();
    for(int i(0);i<SAMPLE_COUNT;i++)
        if(samples[i].mode==HAVE_CALIB)
            unit.push_back(&samples[i]);

    //计算稀释后的浓度
    int count = unit.count();
    double conc[SAMPLE_COUNT];
    double abs[SAMPLE_COUNT];
    for(int i=0;i<count;i++)
    {
        double orgConc=unit.at(i)->conc;
        abs[i]=unit.at(i)->abs;
        conc[i]=orgConc*getDilutionRatio(unit.at(i)->no);
    }

    //当标定数小于4时一次拟合否则二次拟合
   if(num%2==1)
    {
        if(count > 3)
        {
            if(Quadfit(abs,conc,count,qfitA,qfitB,qfitC))
            {
                RQuadfit(abs,conc,count,qfitA,qfitB,qfitC,qfitR);
                return true;
            }
            else
            {
                return false;
            }
        }
        else
        {
            QMessageBox::warning(NULL, tr("错误") ,tr("请选择3个以上的数据进行拟合！"));
            return false;
        }
    }
    if(num%2==0)
    {



            qfitA = 0.0;
            if(Linefit(abs,conc,count,qfitB,qfitC))
            {
                RLinefit(abs,conc,count,qfitB,qfitC,qfitR);
                return true;
            }
            else
            {
                return false;
            }
     }
}

void CalibFrameFactory::saveParams()
{
    CalibFrame::saveParams();

    DatabaseProfile dbprofile;
    if (dbprofile.beginSection("measure"))
    {
        dbprofile.setValue("quada",qfitA);
        dbprofile.setValue("quadb",qfitB);
        dbprofile.setValue("quadc",qfitC);
        dbprofile.setValue("quadr",qfitR);
    }
}

void CalibFrameFactory::loadParams()
{
    CalibFrame::loadParams();
    DatabaseProfile dbprofile;
    if (dbprofile.beginSection("measure"))
    {
        qfitA=dbprofile.value("quada",0).toDouble();
        qfitB=dbprofile.value("quadb",1).toDouble();
        qfitC=dbprofile.value("quadc",0).toDouble();
        qfitR=dbprofile.value("quadr",1).toDouble();
    }
}

void CalibFrameFactory::renewUI()
{
    CalibFrame::renewUI();
    ui->lbResult->setText(tr("拟合结果：A=%1, B=%2, C=%3, R=%4").arg(qfitA).arg(qfitB).arg(qfitC).arg(qfitR));
}

void CalibFrameFactory::reset()
{
    qfitA = 0;
    qfitB = 1;
    qfitC = 0;
    qfitR = 1;
}

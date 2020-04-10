#include "calibframe.h"
#include "common.h"
#include "datafit.h"
#include "profile.h"
#include <QSettings>
#include <QDebug>
#include <QMessageBox>
#include <QSqlQuery>
#include <math.h>
#include <QDateTime>

CalibFrame::CalibFrame(QWidget *parent) :
    QFrame(parent) ,
    Ui_CalibFrame()
{
    setupUi(this);
    QDoubleSpinBox *pConc[] = {dsbConc0,dsbConc1,dsbConc2,dsbConc3,dsbConc4,dsbConc5};
    QLineEdit *pAbs[] = {leAbs0,leAbs1,leAbs2,leAbs3,leAbs4,leAbs5};
    QComboBox *pRange[] = {cbRange0,cbRange1,cbRange2,cbRange3,cbRange4,cbRange5};
    QComboBox *pPipe[] = {cbPipe0,cbPipe1,cbPipe2,cbPipe3,cbPipe4,cbPipe5};
    QSpinBox *pSample[] = {sbSample0,sbSample1,sbSample2,sbSample3,sbSample4,sbSample5};
    QSpinBox *pWater[] = {sbWater0,sbWater1,sbWater2,sbWater3,sbWater4,sbWater5};
    QComboBox *pSelect[] = {cbSelect0,cbSelect1,cbSelect2,cbSelect3,cbSelect4,cbSelect5};
    QCheckBox *pEnable[] = {cbEnable0,cbEnable1,cbEnable2,cbEnable3,cbEnable4,cbEnable5};
    QPushButton *pExec[] = {pbExec0,pbExec1,pbExec2,pbExec3,pbExec4,pbExec5};
    for (int i = 0; i < SAMPLE_COUNT; i++){
        pdsbConc[i] = pConc[i];
        pleAbs[i] = pAbs[i];
        pcbRange[i] = pRange[i];
        pcbPipe[i] = pPipe[i];
        psbSample[i] = pSample[i];
        psbWater[i] = pWater[i];
        pcbSelect[i] = pSelect[i];
        pcbEnables[i] = pEnable[i];
        pcbExec[i] = pExec[i];
    }

    setDisplayType(0);

    algorithm = 0;
    samplelow=waterlow=samplehigh=waterhigh=3;
    diluratioL=diluratioN=diluratioH=1.0;//稀释系数-双定量环
    current=-1;

    connect(pbSave,SIGNAL(clicked()),this,SLOT(slot_save()));
    connect(pbDo,SIGNAL(clicked()),this,SLOT(slot_do()));
    connect(pbTrain,SIGNAL(clicked()),this,SLOT(slot_train()));
    connect(pbReset,SIGNAL(clicked()),this,SLOT(slot_reset()));
    connect(pbSet, SIGNAL(clicked()), this, SLOT(slot_set()));
    connect(pbCalcFitBack, SIGNAL(clicked()), this, SLOT(slot_CalcFitBack()));
    for(int i=0;i<SAMPLE_COUNT;i++) {
        connect(pcbRange[i],SIGNAL(currentIndexChanged(int)),this,SLOT(slot_range(int)));
        connect(pcbExec[i],SIGNAL(clicked()),this,SLOT(slot_Exec()));
    }
    stackedWidget->setCurrentIndex( 0 );
    setAbsReadonly(true);

#if defined (Q_WS_QWS)
    lbResult->setStyleSheet("font: 9pt;");
#endif
}

void CalibFrame::setAbsReadonly(bool b)
{
    QLineEdit *pAbs[] = {leAbs0,leAbs1,leAbs2,leAbs3,leAbs4,leAbs5};
    for(int i=0;i<SAMPLE_COUNT;i++)
        pAbs[i]->setReadOnly(b);
}

void CalibFrame::setDisplayType(int t)
{
    displayType = t;
    if (t == 0)
    {
        lbWork->hide();
        for(int i=0;i<SAMPLE_COUNT;i++){
            pcbEnables[i]->hide();
            pcbExec[i]->hide();
        }
    }
    else
    {
        lbDilu->hide();
        lbSelection->hide();
        lbWork->show();
        pbDo->hide();

        for(int i=0;i<SAMPLE_COUNT;i++){
//            pcbRange[i]->setEnabled(false);
            psbSample[i]->hide();
            psbWater[i]->hide();
            pcbSelect[i]->hide();
            pcbEnables[i]->show();
            pcbExec[i]->show();
        }
    }
}

void CalibFrame::loadParams()
{
    for(int i(0);i<SAMPLE_COUNT;i++){
        samples[i] = F_MAINCPROFILE->getCalibrationSamples(i ,profileId , calcType);
    }
}

void CalibFrame::enablelevel(int level)
{
    if(level == 0)
    {
        pbReset->setEnabled(false);
        pbTrain->setEnabled(false);
        pbSave->setEnabled(false);
        pbDo->setEnabled(false);
        pbSet->setEnabled(false);
    }

    else if(level == 1)
    {
        pbReset->setEnabled(false);
        pbTrain->setEnabled(false);
        pbSet->setEnabled(true);
        pbSave->setEnabled(true);
        pbDo->setEnabled(true);
    }

    else
    {
        pbReset->setEnabled(true);
        pbTrain->setEnabled(true);
        pbSave->setEnabled(true);
        pbDo->setEnabled(true);
        pbSet->setEnabled(true);
    }
}

void CalibFrame::renewParamsFromUI()
{
    for(int i(0);i<SAMPLE_COUNT;i++)
    {
        samples[i].conc = pdsbConc[i]->value();
        if(!pleAbs[i]->isReadOnly())
            samples[i].abs = pleAbs[i]->text().toFloat();
        samples[i].range = pcbRange[i]->currentIndex();
        samples[i].pipe = pcbPipe[i]->currentIndex();
        samples[i].ratio[0] = psbSample[i]->value();
        samples[i].ratio[1] = psbWater[i]->value();
        samples[i].mode = pcbSelect[i]->currentIndex();
        debugLogInfo( 1, QString("标定类型为:[%1] = 标定从界面到变量:[%2]{conc=[%3] abs = [%4] range = [%5] pipe = [%6] ratio0 = [%7] ratio1 = [%8] mode = [%9]}")
                      .arg(calcType)
                      .arg(i)
                      .arg(samples[i].conc)
                      .arg(samples[i].abs)
                      .arg(samples[i].range)
                      .arg(samples[i].pipe)
                      .arg(samples[i].ratio[0])
                      .arg(samples[i].ratio[1])
                      .arg(samples[i].mode).toStdString().data());
    }
}

void CalibFrame::saveParams()
{
    for(int i(0);i<SAMPLE_COUNT;i++)
    {
        F_MAINCPROFILE->setCalibrationSamples(i,profileId,calcType,samples[i]);
        debugLogInfo( 1, QString("标定类型为:[%1] = 标定从变量到配置文件:[%2]{conc=[%3] abs = [%4] range = [%5] pipe = [%6] ratio0 = [%7] ratio1 = [%8] mode = [%9]}")
                      .arg(calcType)
                      .arg(i)
                      .arg(samples[i].conc)
                      .arg(samples[i].abs)
                      .arg(samples[i].range)
                      .arg(samples[i].pipe)
                      .arg(samples[i].ratio[0])
                      .arg(samples[i].ratio[1])
                      .arg(samples[i].mode).toStdString().data());


        F_MAINCPROFILE->setQuadFitA(profileId,qfitA);
        F_MAINCPROFILE->setQuadFitB(profileId,qfitB);
        F_MAINCPROFILE->setQuadFitC(profileId,qfitC);
        F_MAINCPROFILE->setQuadFitR(profileId,qfitR);
    }
}

//计算某个标样的稀释比例
float CalibFrame::getDilutionRatio(int i)
{
    int range = samples[i].range;
    if(range == 0){
        diluratioL = F_MAINCPROFILE->getDilutionRatio0();
        return 1.0/diluratioL;
    }else if(range == 1){
        diluratioN = F_MAINCPROFILE->getDilutionRatio();
        return 1.0/diluratioN;
    }else if(range == 2){
        diluratioH = F_MAINCPROFILE->getDilutionRatioh();
        return 1.0/diluratioH;
    }
    return 0.0;
}

void CalibFrame::renewUI()
{
    for(int i(0);i<SAMPLE_COUNT;i++)
    {
        pdsbConc[i]->setValue(samples[i].conc);
        pleAbs[i]->setText(QString("%1").arg(samples[i].abs,0,'f',4));
        pcbRange[i]->setCurrentIndex(samples[i].range);
        pcbPipe[i]->setCurrentIndex(samples[i].pipe);
//        if(samples[i].range == 2)
//        {
//            psbSample[i]->setReadOnly(false);
//            psbWater[i]->setReadOnly(false);
//        }else{
//            psbSample[i]->setReadOnly(true);
//            psbWater[i]->setReadOnly(true);
//        }
        psbSample[i]->setValue(samples[i].ratio[0]);
        psbWater[i]->setValue(samples[i].ratio[1]);
        pcbSelect[i]->setCurrentIndex(samples[i].mode);
        pcbEnables[i]->setChecked(samples[i].mode != 0);
        debugLogInfo( 1, QString("标定类型为:[%1] = 标定从变量到界面:[%2]{conc=[%3] abs = [%4] range = [%5] pipe = [%6] ratio0 = [%7] ratio1 = [%8] mode = [%9]}")
                      .arg(calcType)
                      .arg(i)
                      .arg(samples[i].conc)
                      .arg(samples[i].abs)
                      .arg(samples[i].range)
                      .arg(samples[i].pipe)
                      .arg(samples[i].ratio[0])
                      .arg(samples[i].ratio[1])
                      .arg(samples[i].mode).toStdString().data());
    }

    QString strText = getResult();
    lbResult->setText(strText);
}

void CalibFrame::setRange(int sel, QString name)
{
    if(sel<0 || sel>=3)
    {
        return ;
    }
    rangeName[sel] = name;
    if (!name.isEmpty())
    {
        for(int i=0;i<SAMPLE_COUNT;i++)
            pcbRange[i]->addItem(name);
    }
}

void CalibFrame::addPipeName( QString name)
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
    debugLogInfo(1, QString("标定保存按钮按下").toStdString().data() );
    for(int i=0;i<SAMPLE_COUNT;i++)
    {
        if (pcbEnables[i]->isVisible()) {
            if (pcbEnables[i]->isChecked())
                pcbSelect[i]->setCurrentIndex(2);
            else
                pcbSelect[i]->setCurrentIndex(0);
        }
    }
    renewParamsFromUI();
    saveParams();
    renewUI();
    setAbsReadonly(true);
    QMessageBox::information(NULL,tr("提示"),tr("保存成功！"));
}

void CalibFrame::slot_do()
{
    debugLogInfo(1, QString("标定设置执行按钮按下").toStdString().data() );
    renewUI();
    emit signalDo();
}

void CalibFrame::slot_train()
{
    for(int i(0);i<SAMPLE_COUNT;i++)
        if(samples[i].mode==HAVE_CALIB)
            calcAbs(i);
    if(!calc())
        QMessageBox::warning(NULL,tr("警告"),tr("样本异常，拟合失败！"));
    debugLogInfo(1, QString("标定设置训练按钮按下").toStdString().data() );
    saveParams();
    renewUI();
}

void CalibFrame::slot_reset()
{
    if(QMessageBox::question(NULL,
                             tr("提示"),
                             tr("重置后，拟合参数会设定为默认值，但标定数据不会受到影响，是否确定？")
                             ,QMessageBox::Yes|QMessageBox::No)
            == QMessageBox::No)
        return;
    reset();
    debugLogInfo(1, QString("标定设置重置按钮按下").toStdString().data() );
    renewUI();
}

void CalibFrame::slot_range(int range)
{
    if(!psbSample[0]->isVisible())
        return;
    int choose = QObject::sender()->objectName().right(1).toInt();
    if(choose<0||choose>=SAMPLE_COUNT)
    {
        return;
    }
    //使能
//    psbSample[choose]->setReadOnly(true);
//    psbWater[choose]->setReadOnly(true);
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
//    else if(range == 2)
//    {
//        psbSample[choose]->setReadOnly(false);
//        psbWater[choose]->setReadOnly(false);
//    }
}

/**
 * @brief CalibFrame::slot_CalcFitBack
 * 返回第一页
 */
void CalibFrame::slot_CalcFitBack()
{
    F_MAINCPROFILE->setCalibTrainBtn(profileId, rBQuadCalc->isChecked());
    stackedWidget->setCurrentIndex( 0 );
}

void CalibFrame::slot_set()
{
    F_MAINCPROFILE->getCalibTrainBtn(profileId)?( rBQuadCalc->setChecked(true) )
                                              :( rBLineCalc->setChecked(true) );
    stackedWidget->setCurrentIndex( 1 );
}

void CalibFrame::slot_Exec()
{
    int choose = QObject::sender()->objectName().right(1).toInt();
    if(choose<0||choose>=SAMPLE_COUNT)
    {
        return;
    }

    for(int i=0;i<SAMPLE_COUNT;i++)
    {
        if (pcbEnables[i]->isChecked())
            pcbSelect[i]->setCurrentIndex(2);
        else
            pcbSelect[i]->setCurrentIndex(0);
    }
    pcbSelect[choose]->setCurrentIndex(1);
    renewParamsFromUI();
    saveParams();
    renewUI();
    setAbsReadonly(true);
    slot_do();
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

float CalibFrame::calcAbs(int current)
{
    if(current<0 || current >= SAMPLE_COUNT)
        return -1.0;
    float fA1 = samples[current].A1<1?1:samples[current].A1;
    float fA2 = samples[current].B1<1?1:samples[current].B1;
    float A11 = samples[current].A2<1?1:samples[current].A2;
    float A22 = samples[current].B2<1?1:samples[current].B2;
    float A_11 = samples[current].A3<1?1:samples[current].A3;
    float A_12 = samples[current].B3<1?1:samples[current].B3;

    samples[current].abs = ELEMENT->calcAbsValue(fA1, fA2, A11, A22, A_11, A_12);

    return samples[current].abs;
}

float CalibFrame::setVLight(int A1, int B1, int A2, int B2, int A3, int B3)
{
    if(current < 0)
        return -1.0;
    if ( isAutoCalibData() == 1 )
        autoSamples[current].abs =  samples[current].abs;

    samples[current].A1 = A1<=0?1:A1;
    samples[current].B1 = B1<=0?1:B1;
    samples[current].A2 = A2<=0?1:A2;
    samples[current].B2 = B2<=0?1:B2;
    samples[current].A3 = A3<=0?1:A3;
    samples[current].B3 = B3<=0?1:B3;
    samples[current].mode = HAVE_CALIB;

    float currentRealAbs = calcAbs(current);

    //检测是否标定结束，如果结束则自动拟合数据
    for(int i(0);i<SAMPLE_COUNT;i++)
        if(samples[i].mode==WAIT_CALIB)
            goto CALC_END;
    calc();
CALC_END:
    saveParams();
    renewUI();
    return currentRealAbs;
}

void CalibFrame::setHaveCalib()
{
    for(int i(0);i<SAMPLE_COUNT;i++)
    {
        if (displayType == 0) {
            if ( samples[i].mode == HAVE_CALIB )
            {
                samples[i].mode = WAIT_CALIB;
            }
        } else {
            if ( samples[i].mode == WAIT_CALIB )
            {
                samples[i].mode = HAVE_CALIB;
            }
            if (i == 1)
                samples[i].mode = WAIT_CALIB; //固定标定标样一
        }
    }
}

/**
 * @brief CalibFrame::setWaitToHaveCalib
 * 设置待标定为已标定
 */
void CalibFrame::setWaitToHaveCalib()
{
    for(int i(0);i<SAMPLE_COUNT;i++)
    {
        if(samples[i].mode==WAIT_CALIB)
        {
            samples[i].mode = HAVE_CALIB;
        }
    }
}
void CalibFrame::setDLDilution(int range, float dilutionrate)
{
    if(range == 0)
        diluratioL = dilutionrate;
    else if(range == 1)
        diluratioN = dilutionrate;
    else if(range == 2)
        diluratioH = dilutionrate;
}

void CalibFrame::setAlgorithm(int i)
{
    algorithm = i;
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
        return -1;
    }
    return samples[select].ratio[0];
}

int CalibFrame::getWater(int select)
{
     if(select<0||select>=SAMPLE_COUNT){
        return -1;
    }
    return samples[select].ratio[1];
}
int CalibFrame::getMode(int select)
{
     if(select<0||select>=SAMPLE_COUNT){
        return -1;
    }
    return samples[select].mode;
}

QString CalibFrame::getPipeName(int select)
{
     if(select<0||select>=SAMPLE_COUNT){
         return "error";
    }
    int i = samples[select].pipe;
    return pcbPipe[select]->itemText(i);
}

QString CalibFrame::getRangeName(int select)
{
     if(select<0||select>=SAMPLE_COUNT){
         return "error";
    }
    int i = samples[select].range;
    return pcbRange[select]->itemText(i);
}

void CalibFrame::save()
{

}

////////////////////////////////////////////////////////////////////////////////////

CalibFrameFactory::CalibFrameFactory(int profileNumber, QWidget *parent) :
    CalibFrame(parent , loop)
{
    profileId = profileNumber;
    calcType = FactoryCali;
    loadParams();
    renewUI();
    pbSet->setVisible( true );
    getCalibTrainBtn(profileId)?( rBQuadCalc->setChecked(true) ):
                                                ( rBLineCalc->setChecked(true) );
}

bool CalibFrameFactory::calc()
{
    //提取已标定的项目
    QList<oneSample*> unit;
    unit.clear();
    for(int i(0);i<SAMPLE_COUNT;i++)
        if(samples[i].mode==HAVE_CALIB)
            unit.push_back(&samples[i]);

    //计算稀释后的浓度
    int count = unit.count();
    double conc[32],abs[32];
    for(int i=0;i<count && i < 32; i++)
    {
        double orgConc = unit.at(i)->conc;
        abs[i]=unit.at(i)->abs;
        conc[i]=orgConc*getDilutionRatio(unit.at(i)->no);
    }

    /*
    ** @DateTime:2018/3/21
    ** @Method:新增一个设置界面
    ** @Function:当选择二次拟合时出厂标定按二次拟合计算
    ** @Author:yangtq
    */
    if( rBQuadCalc->isChecked() )
    {
        if(Quadfit(abs,conc,count,qfitA,qfitB,qfitC))
        {
            RQuadfit(abs,conc,count,qfitA,qfitB,qfitC,qfitR);

            saveParams();
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        qfitA = 0.0;
        if(Linefit(abs,conc,count,qfitB,qfitC))
        {
            RLinefit(abs,conc,count,qfitB,qfitC,qfitR);

            saveParams();
            return true;
        }
        else
        {
            return false;
        }
    }
}


QString CalibFrameFactory::getResult()
{
    return tr("拟合结果%1:").arg(sampleTypeName)+tr("A=%1  B=%2  C=%3  <html>R<sup>2</sup><html>=%4")
            .arg(F_MAINCPROFILE->getQuadFitA(profileId))
            .arg(F_MAINCPROFILE->getQuadFitB(profileId))
            .arg(F_MAINCPROFILE->getQuadFitC(profileId))
            .arg(F_MAINCPROFILE->getQuadFitR(profileId));
}

void CalibFrameFactory::reset()
{
    F_MAINCPROFILE->setQuadFitA(profileId,0.0);
    F_MAINCPROFILE->setQuadFitB(profileId,1.0);
    F_MAINCPROFILE->setQuadFitC(profileId,0.0);
    F_MAINCPROFILE->setQuadFitR(profileId,1.0);
}

void CalibFrameFactory::renewUI()
{
    CalibFrame::renewUI();
    stackedWidget->setCurrentIndex(0);
}

/////////////////////////////////////////////////////////////////////

CalibFrameUser::CalibFrameUser(int profileNumber, QWidget *parent) :
    CalibFrame(parent)
{
    profileId = profileNumber;
    calcType = UserCali;
    loadParams();
    renewUI();
    pbSet->setVisible( false );
}

bool CalibFrameUser::calc()
{
    //提取已标定的项目
    QList<oneSample*> unit;
    unit.clear();
    for(int i(0);i<SAMPLE_COUNT;i++)
    {
        if(samples[i].mode==HAVE_CALIB)
            unit.push_back(&samples[i]);
    }

    //计算稀释后的浓度
    int count = unit.count();
    double realconc[32],theoryconc[32];
    for(int i=0;i<count;i++)
    {
        //实际浓度
        double orgConc=unit.at(i)->conc;
        realconc[i]=orgConc*getDilutionRatio(unit.at(i)->no);
        //理论浓度
        double abs=unit.at(i)->abs;
        theoryconc[i] = F_MAINCPROFILE->getQuadFitA(profileId)*abs*abs
                + F_MAINCPROFILE->getQuadFitB(profileId)*abs
                + F_MAINCPROFILE->getQuadFitC(profileId);
    }
    if ( F_MAINCPROFILE->getIsAutoCalibData() == 1 )
    {
        if (!autoCalibLineFit(theoryconc,realconc,count))
        {
            resetAutoCalibWarnAbs();
            return false;
        }
    }
    //拟合
    if( count >= 2 )
    {
        double lfitA=1.0,lfitB=0.0,lfitR=1.0;
        if(Linefit(theoryconc,realconc,count,lfitA,lfitB))
        {
            RLinefit(theoryconc,realconc,count,lfitA,lfitB,lfitR);

            F_MAINCPROFILE->setLineFitK(profileId,lfitA);
            F_MAINCPROFILE->setLineFitB(profileId,lfitB);
            F_MAINCPROFILE->setLineFitR(profileId,lfitR);
            return true;
        }
        else
            return false;
    }
    else if( count == 1 && samples[0].mode==HAVE_CALIB)
    {
        F_MAINCPROFILE->setLineFitB(profileId,theoryconc[0]);
        return true;
    }
    return false;
}

QString CalibFrameUser::getResult()
{
    return tr("拟合结果%1：").arg(sampleTypeName)+tr("k=%1  b=%2  <html>R<sup>2</sup><html>=%3")
            .arg(F_MAINCPROFILE->getLineFitK(profileId))
            .arg(F_MAINCPROFILE->getLineFitB(profileId))
            .arg(F_MAINCPROFILE->getLineFitR(profileId));
}

void CalibFrameUser::reset()
{
    F_MAINCPROFILE->setLineFitK(profileId,1.0);
    F_MAINCPROFILE->setLineFitB(profileId,0.0);
    F_MAINCPROFILE->setLineFitR(profileId,1.0);
}

bool CalibFrameUser::autoCalibLineFit(double *x, double *y, int count)
{
    double av_x, av_y;
    double L_xx, L_xy, L_yy;
    //变量初始化
    av_x = 0;                             //X的平均值
    av_y = 0;                             //Y的平均值
    L_xx = 0;
    L_yy = 0;
    L_xy = 0;
    for(int i=0; i<count; i++)           //计算XY的平均值
    {
        av_x += x[i]/count;
        av_y += y[i]/count;
    }
    for (int i=0; i<count; i++)               //计算Lxy Lyy和Lxy
    {
        L_xx += (x[i]-av_x) * (x[i]-av_x);
        L_yy += (y[i]-av_y) * (y[i]-av_y);
        L_xy += (x[i]-av_x) * (y[i]-av_y);
    }
    if(L_xx == 0.0)
        return false;
    if((fabs((L_xy / L_xx - getFitA())/getFitA()))>=(F_MAINCPROFILE->getAutoCalibRag()*0.01))
    {
        QString str = QString("自动标定结果超出范围%1%,k=%2，k值未更新")
                .arg(fabs((L_xy / L_xx - getFitA())/getFitA())*100)
                .arg(L_xy/L_xx);
        addErrorMsg(str,0);
        return false;
    }
    return true;
}

/*
 ** @DateTime:2017/12/13 13:29
 ** @Update:修正当自动标定执行时，
 ** 把用户标定中的待标定改成已标定,
 ** 然后保存点击训练时弹出拟合失败但吸光度为随机值问题
 ** @Method:把autoSamples结构体由局部变量转为静态全局变量
 ** @Author:yangtq
 */
/**
 * @brief CalibFrame::resetAutoCalibWarnAbs
 * 自动标定时拟合失败时吸光度重置
 */
void CalibFrameUser::resetAutoCalibWarnAbs()
{
    for(int i(0);i<SAMPLE_COUNT;i++)
    {
        if(samples[i].mode==HAVE_CALIB)
        {
            if ( autoSamples[i].abs != 0 )
                samples[i].abs = autoSamples[i].abs;
        }
    }
    saveParams();
    renewUI();
}

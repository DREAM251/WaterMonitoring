#ifndef _CALIBFRAME_H_
#define _CALIBFRAME_H_

#include <QFrame>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QSpinBox>

#include "ui_calibframe.h"

#define NOT_CALIB   0
#define WAIT_CALIB  1
#define HAVE_CALIB  2

struct oneSample
{
    int no;
    float conc;
    float abs;
    int range;
    int ratio[2];
    int pipe;
    int mode;
    int calibcnt;
    int A1;
    int B1;
    int A2;
    int B2;
    int A3;
    int B3;
};

struct autoCalibSample{
    float abs;
};

#define SAMPLE_COUNT    6

namespace Ui {
class CalibFrame;
}

//标定类型
enum CaliType {
    FactoryCali = 0,
    UserCali = 1
};


class CalibFrame : public QFrame , public Ui_CalibFrame
{
    Q_OBJECT
    
public:
    explicit CalibFrame(QWidget *parent = 0);

    void setAbsReadonly(bool);
    void setDisplayType(int t);

    void renewParamsFromUI();
    virtual void saveParams();
    virtual void loadParams();

    virtual void renewUI();
    void enablelevel(int level);
    void setRange(int sel,QString name);
    void addPipeName(QString name);
    void setSampleLow(int s1,int s2);
    void setSampleHigh(int s1,int s2);
    float setVLight(int A1, int B1, int A2, int B2, int A3, int B3);
    void setDLDilution(int range,float dilutionrate);// 设置双定量环稀释比例
    void setAlgorithm(int i);//设置出厂标定的算法

    float getDilutionRatio(int sample);
    int getNextCalib();
    void setHaveCalib();
    void setWaitToHaveCalib(); // 自动标定专用
    float getConc(int select);
    float getAbs(int select);
    int getPipe(int select);
    int getRange(int select);
    QString getPipeName(int select);
    QString getRangeName(int select);
    int getSample(int select);
    int getWater(int select);
    int getMode(int select);

protected :
    float calcAbs(int index);
    virtual bool calc()=0;
    virtual QString getResult()=0;
    virtual void reset()=0;
    void save();

protected Q_SLOTS:
    void slot_save();
    void slot_do();
    void slot_train();
    void slot_reset();
    void slot_range(int);
    void slot_CalcFitBack();
    void slot_set();
    void slot_Exec();

Q_SIGNALS:
    void signalDo();

private:
    bool displayType;
    QString rangeName[3];
    int samplelow,waterlow,samplehigh,waterhigh;//进样比例-单定量环
    float diluratioL,diluratioN,diluratioH;//稀释系数-双定量环
    oneSample samples[SAMPLE_COUNT];
    int algorithm;

    QDoubleSpinBox *pdsbConc[SAMPLE_COUNT];
    QLineEdit *pleAbs[SAMPLE_COUNT];
    QComboBox *pcbRange[SAMPLE_COUNT];
    QSpinBox *psbSample[SAMPLE_COUNT];
    QSpinBox *psbWater[SAMPLE_COUNT];
    QComboBox *pcbPipe[SAMPLE_COUNT];
    QComboBox *pcbSelect[SAMPLE_COUNT];

    QCheckBox *pcbEnables[SAMPLE_COUNT];
    QPushButton *pcbExec[SAMPLE_COUNT];


    int current;//当前的标样
    CaliType calcType;//出厂标定|用户标定
    int profileId;//当前配置文件编号
    QString sampleTypeName; //

    static struct autoCalibSample autoSamples[SAMPLE_COUNT]; // 自动标定专用
};

class CalibFrameFactory : public CalibFrame
{
public:
    explicit CalibFrameFactory(int profileNumber = 0, QWidget *parent= NULL);


    virtual QString getResult();
    virtual void reset();
    virtual void renewUI();

protected:
    bool calc();

    //当标定数小于4时一次拟合否则二次拟合
    double qfitA;
    double qfitB;
    double qfitC;
    double qfitR;
};

class CalibFrameUser : public CalibFrame
{
public:
    explicit CalibFrameUser(int profileNumber = 0, QWidget *parent= NULL);

    virtual QString getResult();
    virtual void reset();

protected:
    bool calc();

    /** 自动标定专用 */
    bool autoCalibLineFit(double *x, double *y, int count);
    void resetAutoCalibWarnAbs();

    double lfitK;
    double lfitB;
    double lfitR;
    double qfitA;
    double qfitB;
    double qfitC;
    double qfitR;
};

#endif // CalibFRAME_H

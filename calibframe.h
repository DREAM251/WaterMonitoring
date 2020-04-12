#ifndef _CALIBFRAME_H_
#define _CALIBFRAME_H_

#include <QFrame>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QSpinBox>

#define NOT_CALIB   0
#define WAIT_CALIB  1
#define HAVE_CALIB  2
#define SAMPLE_COUNT    6

struct oneSample{
    int no;
    float conc;
    float abs;
    int range;
    int ratio[2];
    int pipe;
    int mode;
};
struct autoCalibSample{
    float abs;
};



namespace Ui {
class CalibFrame;
}

class CalibFrame : public QFrame
{
    Q_OBJECT
    
public:
    explicit CalibFrame(const QString &profile, QWidget *parent = 0);
    ~CalibFrame();

    void renewParamsFromUI();
    float getDilutionRatio(int sample);

    virtual bool calc() = 0;
    virtual void loadParams();
    virtual void saveParams();
    virtual void renewUI();
    virtual void reset();
    bool train();

    void enablelevel(int level);
    void addPipeName(QString name);
    void setRange(int sel, QString name);
    void setSampleLow(int s1,int s2);
    void setSampleHigh(int s1,int s2);
    void setVLight(int A1,int A2);

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

public Q_SLOTS:
    void slot_save();
    void slot_do();
    void slot_train();
    void slot_reset();
    void slot_range(int);

Q_SIGNALS:
    void signal_do();

protected:
    Ui::CalibFrame *ui;

    int current;//当前的标样
    QString profile;
    QString rangeName[3];
    int samplelow,waterlow,samplehigh,waterhigh;//进样比例-单定量环

    struct oneSample samples[SAMPLE_COUNT];
    QDoubleSpinBox *pdsbConc[SAMPLE_COUNT];
    QLineEdit *pleAbs[SAMPLE_COUNT];
    QComboBox *pcbRange[SAMPLE_COUNT];
    QSpinBox *psbSample[SAMPLE_COUNT];
    QSpinBox *psbWater[SAMPLE_COUNT];
    QComboBox *pcbPipe[SAMPLE_COUNT];
    QComboBox *pcbSelect[SAMPLE_COUNT];
};

class CalibFrameUser : public CalibFrame
{
public:
    explicit CalibFrameUser(QWidget *parent=NULL);

    virtual void saveParams();
    virtual void loadParams();
    virtual void renewUI();
    virtual void reset();

protected:
    bool calc();

protected:
    double lfitA,lfitB,lfitR;
    double qfitA,qfitB,qfitC,qfitR;
};

class CalibFrameFactory : public CalibFrame
{
public:
    explicit CalibFrameFactory(QWidget *parent=NULL);

    virtual void saveParams();
    virtual void loadParams();
    virtual void renewUI();
    virtual void reset();

protected:
    bool calc();

    double qfitA,qfitB,qfitC,qfitR;
};

#endif // CalibFRAME_H

#ifndef IPROTOCOL_H
#define IPROTOCOL_H

#include <QObject>
#include <QString>
#include <QTimer>
#include <qcoreevent.h>
#include <qextserialport.h>

class Sender
{
public:
    Sender(const QByteArray &src) :sent(src) {}
    Sender(){}

    // data section
    QByteArray data();
    QByteArray rawData() {return sent;}
    void clear() {sent.clear();}
    QString translateExplainCode();
    QString getTCValve1Name(int i);

    int step();
    int stepTime();
    int peristalticPump();
    int peristalticPumpSpeed();
    int pump2();
    int TCValve1();
    int TCValve2();
    int valve1();
    int valve2();
    int valve3();
    int valve4();
    int valve5();
    int valve6();
    int valve7();
    int valve8();
    int extValve();
    int extControl1();
    int extControl2();
    int extControl3();
    int _420mA();
    int fun();
    int waterLevel();
    float heatTemp();
    int waterLedControl();
    int measureLedControl();

    int timeFix();
    int timeAddFix();
    int tempFix();
    int loopFix();
    int judgeStep();
    bool waterLevelReachStep();
    bool waterLevelReachStep2();
    bool waterLevelReachStep3();
    bool heatReachStep();
    bool heatJudgeStep();
    bool coolReachStep();
    bool coolJudgeStep();
    bool blankStep();
    bool colorStep();
    int explainCode();

    void setStep(int i);
    void setStepTime(int i);
    void setPeristalticPump(int i);
    void setPeristalticPumpSpeed(int i);
    void setPump2(int i);
    void setTCValve1(int i);
    void setTCValve2(int i);
    void setValve1(int i);
    void setValve2(int i);
    void setValve3(int i);
    void setValve4(int i);
    void setValve5(int i);
    void setValve6(int i);
    void setValve7(int i);
    void setValve8(int i);
    void setExtValve(int i);
    void setExtControl1(int i);
    void setExtControl2(int i);
    void setExtControl3(int i);
    void set420mA(int i);
    void setFun(int i);
    void setWaterLevel(int i);
    void setHeatTemp(float i);

private:
    // sent data format
    //   header              data               gap  control
    //           ---------------------------------------------
    //           | 100 ......................... | : |  xxxx |
    //           ----------------------------------- ---------
    QByteArray sent;
};

class ConfigSender
{
public:
    ConfigSender(const QByteArray &src);
    ConfigSender();

    // data section
    QByteArray data();
    QByteArray rawData() {return sent;}

//    void setWaterLevelAlwaysOn(int);
//    void setLedAlwaysOn(int);
//    void setWaterLevelRealTimeCheck(int);
//    void setWasteWaterRealTimeCheck(int);
    void setWaterLevelLed1Current(int);
    void setWaterLevelLed23Current(int);
    void setLed1Current(int);
    void setLed2Current(int);
    void setPD1Incred(int);
    void setPD2Incred(int);

//    void set420mA1(int);
//    void set420mA2(int);
//    void setTempFixBit(int);
//    void setTempFixValue(int);
    void setWaterLevel1Threshold(int);
    void setWaterLevel2Threshold(int);
    void setWaterLevel3Threshold(int);

    int step();

private:
    // sent data format
    //   header              data               gap  control
    //           -------------------------------
    //           |0100 ........................|
    //           -------------------------------
    QByteArray sent;
};

class Receiver
{
public:
    Receiver(const QByteArray &src) : recv(src){}
    Receiver(){}

    // return value
    //  -1 : error, 0 : ok, 1 : missing
    int check();
    QByteArray data();

    int step();
    int stepTime();
    int extControl1();
    int extControl2();
    int extControl3();
    int _420mA();
    int pumpStatus();
    int waterLevel();
    float heatTemp();
    int mcu1Temp();
    int mcu2Temp();
    int lightVoltage1();
    int lightVoltage2();
    int lightVoltage3();
    int refLightSignal();
    int measureSignal();

    void setStep(int i);
private:
    //recv data format
    //  header              data               gap  control
    //----------------------------------------
    //|#xxxcc1 | 100 ........................|
    //----------------------------------------
    QByteArray recv;
};


// ¼ÆÊ±Æ÷
class ProtocolCounter : public QObject
{
    Q_OBJECT

public:
    ProtocolCounter(QObject *parent = NULL);

    void start(int seconds);
    void stop();

    void lock(){islock = true;}
    void unlock(){islock = false;}
    bool locked() {return islock;}

    bool isIdle() {return counts >= max;}
    int getCounts() {return counts;}

Q_SIGNALS:
    void timing();

protected:
    void timerEvent(QTimerEvent *event);

    int timerid;
    int counts;
    int max;
    bool islock;
};


class IProtocol : public QObject
{
    Q_OBJECT

public:
    IProtocol(const QString &portParamter = "com5,9600,n,8,1", QObject *parent = 0);
    ~IProtocol();

    bool recvNewData();
    bool isTimeOut(){return timeoutFlag;}
    bool isIdle(){return counter->isIdle();}
    void reset();
    bool portIsOpened();

    void sendData(const QString &cmd);
    void sendConfig(const ConfigSender &sender);
    void skipCurrentStep();

    Sender getSender(){return dataSender;}
    Receiver getReceiver(){return dataReceiver;}

public Q_SLOTS:
    void onReadyRead();
    void onCounterTimeout();

protected:
    bool timeoutFlag;
    bool newDataFlag;
    int timeCount;
    int sendType;

    QByteArray recvTemp;
    QTimer *timer;
    ProtocolCounter *counter;
    QextSerialPort *port;

    Sender dataSender;
    ConfigSender configSender;
    Receiver dataReceiver;
};

#endif // IPROTOCOL_H

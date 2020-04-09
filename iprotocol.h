#ifndef IPROTOCOL_H
#define IPROTOCOL_H

#include <QObject>
#include <QString>
#include <QTimer>
#include <qcoreevent.h>
#include <qextserialport.h>

// ¼ÆÊ±Æ÷
class ProtocolCounter : public QObject
{
    Q_OBJECT

public:
    ProtocolCounter(QObject *parent = NULL) :
        QObject(parent),
        timerid(0),
        counts(0),
        max(0),
        islock(false)
    {}

    void start(int seconds)
    {
        stop();
        timerid = startTimer(1000);
        counts = 0;
        max = seconds;
    }

    void stop()
    {
        islock = false;
        if(timerid)
            killTimer(timerid);
        timerid = 0;
        counts = max;
    }

    void lock(){islock = true;}
    void unlock(){islock = false;}
    bool locked() {return islock;}

    bool isIdle() {return counts >= max;}
    int getCounts() {return counts;}

Q_SIGNALS:
    void timing();

protected:
    void timerEvent(QTimerEvent *event)
    {
        if(event->timerId() == timerid && !islock) {
            counts++;
            if (counts >= max)
                stop();
        }
        emit timing();
    }

    int timerid;
    int counts;
    int max;
    bool islock;
};

class Sender
{
public:
    Sender(const QByteArray &src) :sent(src) {}
    Sender(){}

    // data section
    QByteArray data();
    QByteArray rawData() {return sent;}

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
    int extValve();
    int extControl1();
    int extControl2();
    int extControl3();
    int fun();
    int waterLevel();
    int heatTemp();

    int timeFix();
    int timeAddFix();
    int tempFix();
    int loopFix();
    bool waterLevelReachStep();
    bool waterLevelJudgeStep();
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
    void setExtValve(int i);
    void setExtControl1(int i);
    void setExtControl2(int i);
    void setExtControl3(int i);
    void setFun(int i);
    void setWaterLevel(int i);
    void setHeatTemp(int i);

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
    ConfigSender(const QByteArray &src) :sent(src) {}
    ConfigSender(){}

    // data section
    QByteArray data();
    QByteArray rawData() {return sent;}

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
    int waterLevel();
    int heatTemp();
    int mcu1Temp();
    int mcu2Temp();
    int lightVoltage1();
    int lightVoltage2();
    int lightVoltage3();
    int measureSignal1();
    int measureSignal2();

private:
    //recv data format
    //  header              data               gap  control
    //----------------------------------------
    //|#xxxcc1 | 100 ........................|
    //----------------------------------------
    QByteArray recv;
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

    void sendData(const QString &cmd);
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

    QByteArray recvTemp;
    QTimer *timer;
    ProtocolCounter *counter;
    ProtocolCounter *timeoutCounter;
    QextSerialPort *port;

    Sender dataSender;
    ConfigSender configSender;
    Receiver dataReceiver;
};

#endif // IPROTOCOL_H

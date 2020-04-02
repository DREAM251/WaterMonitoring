#ifndef IPROTOCOL_H
#define IPROTOCOL_H

#include <QObject>
#include <QString>
#include <QTimer>
#include <qcoreevent.h>
#include <qextserialport/qextserialport.h>

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
    Sender(const QByteArray &src);
    Sender();

    // data section
    QByteArray data();
    QByteArray rawData() {return sent;}
    int getStep();
    int getStepTime();
    int getHeatTemp();
    int getLightVoltage();

    void setTime(int seconds);
    void setTemp(int temp);

    // control section
    int loopFix();
    int timeFix();
    int tempFix();

    bool isBlankStep();
    bool isColorStep();
    bool isHeatStep();
    bool isBlankJudgeStep();
    bool isHeatJudgeStep();
    bool isWaterLevelJudgeStep();

private:
    // sent data format
    //   header              data               gap  control
    //           ---------------------------------------------
    //           | 100 ......................... | : |  xxxx |
    //           ----------------------------------- ---------
    QByteArray sent;
};

class Receiver
{
public:
    Receiver(const QByteArray &src);
    Receiver();

    // return value
    //  -1 : error, 0 : ok, 1 : missing
    int check();
    QByteArray data();

    int getStep();
    int getStepTime();

    int getHeatTemp();
    int getWaterLevel();
    int getLightVoltage();

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
    IProtocol(const QString &portParamter = "com5,9600,n,8,1");
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
    Receiver dataReceiver;
};

#endif // IPROTOCOL_H

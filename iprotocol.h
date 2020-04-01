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
    int getStep(){return 0;}
    int getStepTime(){return 10;}
    int getHeatTemp(){return 0;}

    bool isBlankStep(){return false;}
    bool isColorStep(){return false;}
    bool isHeatStep(){return false;}


    bool getLightVoltage(){return false;}

    bool isBlankJudgeStep(){return false;}
    bool isHeatJudgeStep(){return false;}
    bool isWaterLevelJudgeStep(){return false;}
    QByteArray sent;
};

class Receiver
{
public:
    int getStep(){return 0;}
    int getStepTime(){return 10;}

    int getHeatTemp(){return 0;}
    int getWaterLevel(){return 0;}
    int getLightVoltage() {return 0;}

    QByteArray recv;
};

class IProtocol : public QObject
{
    Q_OBJECT

public:
    IProtocol(const QString &portParamter = "com1,9600,n,8,1");
    ~IProtocol();

    bool recvNewData();
    bool isTimeOut(){return timeoutFlag;}
    bool isIdle(){return counter->isIdle();}
    void reset();

    QByteArray addHeader(const QByteArray &src);
    void sendData(const QString &cmd);
    void skipCurrentStep();

    Sender *getSender(){return sender;}
    Receiver *getReceiver(){return receiver;}

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

    Sender *sender;
    Receiver *receiver;
};

#endif // IPROTOCOL_H

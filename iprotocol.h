#ifndef IPROTOCOL_H
#define IPROTOCOL_H

#include <QObject>
#include <QString>
#include <QTimer>
#include <qextserialport/qextserialport.h>

// ¼ÆÊ±Æ÷
class Counter : public QObject
{
    Q_OBJECT

public:
    Counter(QObject *parent = NULL) : QObject(parent),
        counts(0),
        max(0)
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
        if(timerid)
            killTimer(timerid);
        timerid = 0;
    }

    bool isIdle() {return counts >= max;}


protected:
    void timerEvent(QTimerEvent *event)
    {
        if(event->timerId() == timerid) {
            counts++;
            if (counts >= max)
                stop();
        }
    }

private:
    int timerid;
    int counts;
    int max;
};

class IProtocol : public QObject
{
    Q_OBJECT

public:
    IProtocol(const QString &portParamter = "/dev/ttySAC0,9600,n,8,1");

    void recvNewData();
    void isTimeOut(){return isTimeout;}
    void isIdle();

    void sendData(const QString &cmd);
    void skipCurrentStep();

    void isBlankStep(){return false;}
    void isColorStep(){return false;}
    void isHeatStep(){return false;}

    void isBlankJudgeStep(){return false;}
    void isHeatJudgeStep(){return false;}
    void isWaterLevelJudgeStep(){return false;}

    void getRecvHeatTemp(){return false;}
    void getSentHeatTemp(){return false;}
    void getRecvWaterLevel(){return false;}
    void getLightVoltage(){return false;}

    QByteArray checkSum(const QByteArray &by);

public Q_SLOTS:
    void onReadyRead();

protected:
    bool isTimeout;
    bool newDataFlag;
    int timeCount;

    QByteArray sent;
    QByteArray recv, recvTemp;

    QTimer *timer;
    Counter *counter;
    Counter *timeoutCounter;
    QextSerialPort *port;
};

#endif // IPROTOCOL_H

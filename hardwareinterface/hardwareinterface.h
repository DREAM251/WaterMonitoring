#ifndef HARDWAREINTERFACE_H
#define HARDWAREINTERFACE_H

#include <QObject>
#include <QAbstractSocket>
//#include <QtSerialPort/QSerialPort>

class QextSerialPort;
//class ManageSerialPort;
class QTcpSocket;

class VHardwareInterface : public QObject
{
    Q_OBJECT
public:
    explicit VHardwareInterface(){;}
    virtual ~VHardwareInterface(){;}
    virtual bool openHardwareInterface()=0;
    virtual bool closeHardwareInterface()=0;
    virtual void sendData(QByteArray &) = 0;
signals:
    void newDataReceived(const QByteArray &dataReceived);
    void sigTcpStateChanged(QAbstractSocket::SocketState);
};

//////////////////串行接口类///////////////////////////////////
class VSerialPort:public VHardwareInterface
{
    Q_OBJECT
public:

    enum Direction  {
        Input = 1,
        Output = 2,
        AllDirections = Input | Output
    };

    enum BaudRate {
        Baud1200 = 1200,
        Baud2400 = 2400,
        Baud4800 = 4800,
        Baud9600 = 9600,
        Baud19200 = 19200,
        Baud38400 = 38400,
        Baud57600 = 57600,
        Baud115200 = 115200
    };

    enum DataBits {
        Data5 = 5,
        Data6 = 6,
        Data7 = 7,
        Data8 = 8
    };

    enum Parity {
        NoParity = 'N',
        EvenParity = 'E',
        OddParity = 'O'
    };

    enum StopBits {
        OneStop = 1,
        OneAndHalfStop = 3,
        TwoStop = 2
    };

    enum FlowControl {
        NoFlowControl,
        HardwareControl,
        SoftwareControl
    };

    enum SerialPortError {
        NoError,
        DeviceNotFoundError,
        PermissionError,
        OpenError,
        ParityError,
        FramingError,
        BreakConditionError,
        WriteError,
        ReadError,
        ResourceError,
        UnsupportedOperationError,
        UnknownError,
        TimeoutError,
        NotOpenError
    };

//    explicit VSerialPort(QString portName,quint32 baudrate,quint16 databit, quint8 parity, quint16 stopbit);
    explicit VSerialPort();
    virtual ~VSerialPort();
    virtual bool openHardwareInterface();
    virtual bool closeHardwareInterface();
    virtual void sendData(QByteArray &);

    void setPortName(const QString &name);
    void setBaudRate(BaudRate baud);
    void setDataBits(DataBits data);
    void setParity(Parity parity);
    void setStopBits(StopBits stop);
    void setFlowControl(FlowControl flowControl);
    inline bool open(){return openHardwareInterface();}
    inline bool close(){return closeHardwareInterface();}
    bool isOpen();
    qint64 write(const QByteArray &data);
    qint64 read(char *data, qint64 max);
    bool bytesAvailable();
    QByteArray readAll();

Q_SIGNALS:
    void readyRead();

private:
    struct VSerialPortPar{
        QString port;
        quint32 baud;
        quint16 databit;
        quint8 parity;
        quint16 stopbit;
    }VSerialPortPar;
   //ManageSerialPort *serialPort;
   QextSerialPort *myCom;
   QByteArray byData;
private slots:
    void readMyCom();
};

//////////////////以太网接口类///////////////////////////////////
class VTcpPort:public VHardwareInterface
{
    Q_OBJECT
public:
    explicit VTcpPort(QString hostName,quint16 port);
    virtual ~VTcpPort();
    virtual bool openHardwareInterface();
    virtual bool closeHardwareInterface();
    virtual void sendData(QByteArray &);
private:
    QTcpSocket *mTcpSocket;
    uchar *mBufferRecv;
    QString hostIpAddr;
    quint16 hostPort;
private slots:
    void slotTcpNewDataReceived();
};
#endif // HARDWAREINTERFACE_H

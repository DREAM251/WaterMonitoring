#ifndef MODBUSMODULE_H
#define MODBUSMODULE_H

#include <QFrame>
#include <QTimer>
#include "modbus/ntmodbus.h"

namespace Ui {
class ModbusModule;
}

class ModbusProcesser : public NTModbusSlave
{
    Q_OBJECT

public:
    explicit ModbusProcesser(QObject *parent = 0);

private:
    bool OpenPort();
    void ClosePort();

Q_SIGNALS:
    void valueChange();

protected:
    //根据实际的应用重写这些功能
    virtual unsigned char GetSystemHoldRegister(unsigned short first,int counts,std::vector<unsigned short> &values);
    virtual unsigned char SetSystemHoldRegister(unsigned short first,std::vector<unsigned short> &values);

    friend class ModbusModule;
};


class ModbusModule : public QFrame
{
    Q_OBJECT
    
public:
    explicit ModbusModule(QFrame *parent = 0);
    ~ModbusModule();
    
    void showCommand(int i);

private:
    Ui::ModbusModule *ui;
    ModbusProcesser *processer;

    void LoadParameter();
    void SaveParameter();

private slots:
    void slotRecv();
    void slotSend();
    void slotSave();
    void slotShowCMD();

Q_SIGNALS:
    void valueChange();
};

#endif // MODBUSMODULE_H

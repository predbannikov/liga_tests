#ifndef DEVICELIGA_H
#define DEVICELIGA_H

#include <QObject>
#include <QDataStream>
#include <QVariant>
#include <QDebug>


enum PDU_COMMAND {
    PDU_COMMAND_0x03 = 0x03,
    PDU_COMMAND_0x10 = 0x10
};


enum FunctionCode {
    ActBase   = 0,
    ActVol0   = 0,
    ActVol1   = 1,
    ActLoad0  = 2,
    ActLoad1  = 3,
    ActShear0 = 3,

    SensBase  = 4,
    SensPrs0  = 4,
    SensPrs1  = 5,
    SensPrs2  = 6,
    SensLoad0 = 7,
    SensLoad1 = 8,
    SensDef0  = 9,
    SensDef1  = 10,

    CtrlBase   = 11,
    CtrlVol0   = 11,
    CtrlVol1   = 12,
    CtrlLoad0  = 13,
    CtrlLoad1  = 14,
    CtrlShear0 = 14,

    FuncEnd    = 15,
};

enum CommandRegisterAddr : quint16 {
    CmdAddr         = 0,
    IDataAddr       = 1,
    Calib0Addr      = 2,
    Calib1Addr      = 4,

    FuncBaseAddr    = 6,
    ActBaseAddr     = 6,
    ActVol0Addr     = 6,
    ActVol1Addr     = 8,
    ActLoad0Addr    = 10,
    ActLoad1Addr    = 12,
    ActShear0Addr   = 12,

    SensBaseAddr    = 14,
    SensPress0Addr  = 14,
    SensPress1Addr  = 16,
    SensPress2Addr  = 18,
    SensLoad0Addr   = 20,
    SensLoad1Addr   = 22,
    SensDeform0Addr = 24,
    SensDeform1Addr = 26,

    CtrlBaseAddr    = 28,
    CtrlVol0Addr    = 28,
    CtrlVol1Addr    = 30,
    CtrlLoad0Addr   = 32,
    CtrlLoad1Addr   = 34,
    CtrlShear0Addr  = 34,

    RegisterEndAddr = 36,
};

enum OpCode : quint16 {
    OpCodeMask     = 0xff00,
    GetID          = 0xf100,
    GetNumFunc     = 0xf200,
    GetFuncType    = 0xf300,

    SoftReset      = 0xfd00,
    HardReset      = 0xfe00,
    Nop            = 0xff00,

    SensorRead     = 0x1000,
    SensorReadRaw  = 0x1100,
    SensorCalRead  = 0x1200,
    SensorCalStore = 0x1300,
    SensorNull     = 0x1400,
    SensorReset    = 0x1500,

    StepperMove    = 0x2000,
    StepperMoveTo  = 0x2100,
    StepperPos     = 0x2200,
    StepperStatus  = 0x2300,
    StepperSpeed   = 0x2400,
    StepperStop    = 0x2500,
    StepperNull    = 0x2600,

    ControllerSet    = 0x3000,
    ControllerLock   = 0x3100,
    ControllerSetKp  = 0x3200,
    ControllerSetKi  = 0x3300,
    ControllerSetKd  = 0x3400,
    ControllerStatus = 0x3500,
};

class Sensor {
    quint32 value;
public:
    Sensor();
    float getValue() {
        return value / 1000.;
    }

    quint32 getValueACP() {
        return value;
    }

    void changeValue(qint32 k) {

    }
};

class DeviceLiga : public QObject
{
    Q_OBJECT


public:
    explicit DeviceLiga(QObject *parent = nullptr);

    float deform = 0.0;
    float pressure = 0.0;
    quint32 stepper = 0;
    qint16 address_reg = -1;
    qint8 liga_cmd = -1;
    FunctionCode liga_func = ActBase;
    QVector <quint16> registers;

    void write(QByteArray &data);

    void read(QByteArray &data);

    static QByteArray write(quint16 cmd, quint8 funcNum, quint16 addr);

    static QByteArray read(quint16 addr_, quint16 countReg_);

    static QByteArray write(quint16 addr, QVariant data);

    static QVector<quint16> modbusData(const QVariant &data);

signals:

};

#endif // DEVICELIGA_H

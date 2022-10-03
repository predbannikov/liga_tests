#include "deviceliga.h"

DeviceLiga::DeviceLiga(QObject *parent) : QObject(parent)
{
    registers.resize(256);
}

void DeviceLiga::write(QByteArray &data)
{
    QDataStream str(&data, QIODevice::ReadOnly);
    str.setVersion(QDataStream::Qt_5_11);
    str.setByteOrder(QDataStream::BigEndian);
    qint8 modbus_addr = -1;
    qint8 modbus_cmd = -1;
    address_reg = -1;
    liga_cmd = -1;
    liga_func = ActBase;
    qint16 countReg = -1;
    qint8 countBytes = -1;
    str >> modbus_addr;
    str >> modbus_cmd;
    str >> address_reg;
    str >> countReg;
    str >> countBytes;

    qDebug() << address_reg << countReg << countBytes << liga_cmd << liga_func;
    str >> registers[address_reg];
    data.remove(6, 3);
}

void DeviceLiga::read(QByteArray &data)
{
    QByteArray arr;
    QDataStream str(&arr, QIODevice::ReadWrite);
    str.setVersion(QDataStream::Qt_5_11);
    str.setByteOrder(QDataStream::BigEndian);
    str << static_cast<quint8>(PDU_COMMAND_0x03);
//    str << static_cast<quint16>(addr_);
//    str << static_cast<quint16>(countReg_);
    if (address_reg == -1)
        return ;

    OpCode st = static_cast<OpCode>(registers[address_reg] >> 8);
    switch (st) {
    case GetID:
        break;
    case GetNumFunc:
        break;
    case GetFuncType:
        break;
    case SoftReset:
        break;
    case HardReset:
        break;
    case Nop:
        break;
    case SensorRead:
        break;
    case SensorReadRaw:
        break;
    case SensorCalRead:
        break;
    case SensorCalStore:
        break;
    case SensorNull:
        break;
    case SensorReset:
        break;
    case StepperMove:
        break;
    case StepperMoveTo:
        break;
    case StepperPos:
        break;
    case StepperStatus:
        break;
    case StepperSpeed:
        break;
    case StepperStop:
        break;
    case StepperNull:
        break;
    case ControllerSet:
        break;
    case ControllerLock:
        break;
    case ControllerSetKp:
        break;
    case ControllerSetKi:
        break;
    case ControllerSetKd:
        break;
    case ControllerStatus:
        break;

    }
}

QByteArray DeviceLiga::write(quint16 cmd, quint8 funcNum, quint16 addr) {
    QByteArray arr;
    QDataStream str(&arr, QIODevice::ReadWrite);
    str.setVersion(QDataStream::Qt_5_11);
    str.setByteOrder(QDataStream::BigEndian);
    str << static_cast<quint8>(PDU_COMMAND_0x10);
    str << static_cast<quint16>(0x00);                  // TODO
    str << static_cast<quint16>(0x01);                  // TODO
    str << static_cast<quint8>(sizeof (cmd));
    str << static_cast<quint16>(cmd | funcNum);
    return arr.toHex();
}

QByteArray DeviceLiga::write(quint16 addr, QVariant data) {
    QByteArray arr;
    QDataStream str(&arr, QIODevice::ReadWrite);
    str.setVersion(QDataStream::Qt_5_11);
    str.setByteOrder(QDataStream::BigEndian);
    str << static_cast<quint8>(PDU_COMMAND_0x10);
    str << static_cast<quint16>(addr);
    QVector<quint16> tmp = modbusData(data);
    int size = tmp.size();
    str << static_cast<quint16>(size);
    str << static_cast<quint8>(size * 2);
    for (auto value: tmp)
        str << static_cast<quint16>(value);
    return arr.toHex();
}

QByteArray DeviceLiga::read(quint16 addr_, quint16 countReg_) {
    QByteArray arr;
    QDataStream str(&arr, QIODevice::ReadWrite);
    str.setVersion(QDataStream::Qt_5_11);
    str.setByteOrder(QDataStream::BigEndian);
    str << static_cast<quint8>(PDU_COMMAND_0x03);
    str << static_cast<quint16>(addr_);
    str << static_cast<quint16>(countReg_);
    return arr.toHex();
}

QVector<quint16> DeviceLiga::modbusData(const QVariant &data) {
    QVector<quint16> result;
    const QString dataType = data.typeName();
    /* TODO: Find a better way to convert these values...? */
    if(dataType == "QVector<quint16>") {
        result = data.value<QVector<quint16>>();
    } else if((dataType == "float") || (dataType == "double")) {
        const auto value = data.toFloat();
        auto *ptr = reinterpret_cast<const quint16*>(&value);
        for(unsigned i = 0; i < sizeof(float) / sizeof(quint16); ++i) {
            result.append(ptr[i]);
        }
    } else if((dataType == "int") || (dataType == "unsigned int")) {
        const auto value = data.toInt();
        auto *ptr = reinterpret_cast<const quint16*>(&value);
        for(unsigned i = 0; i < sizeof(int) / sizeof(quint16); ++i) {
            result.append(ptr[i]);
        }
    }  else {
        qDebug() << "undefined type " << Q_FUNC_INFO;
    }
    return result;
}

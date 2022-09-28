#include "test_open_serialport_in_process.h"
#include <QTimer>
#include <QDebug>
#include <QDateTime>
#include <iostream>
#include <QCoreApplication>

tester_device_in_proccess::tester_device_in_proccess(QObject *parent)
    : QObject{parent}
{
    std::cout << "\nconstruct test\n";
    port_init();
    on_master_writer();
}

tester_device_in_proccess::~tester_device_in_proccess()
{
    port->close();
    std::cout << "~destruct_test\n";
}

void tester_device_in_proccess::onSerialReadyRead()
{
    QString tmp = port->readAll().toHex();
    buffer.append(tmp);
    std::cout  << "readyRead " << tmp.toStdString() << QDateTime::currentDateTime().toString(" ss:zzz").toStdString() << "\n";

    switch (state) {
    case STATE_1:
        if(buffer == "01100000000101c9")
            state = STATE_2;
        else
            return;
        break;
    case STATE_2:
        if(buffer == "01030200017984")
            state = STATE_3;
        else
            return;
        break;
    case STATE_3:
//        port->write(QByteArray::fromHex("02100000000102f100f730"));
        break;
    }

    QTimer::singleShot(500, this, &tester_device_in_proccess::on_master_writer);
}

void tester_device_in_proccess::onSerialBytesWritten()
{
    std::cout  << "bytesWritten " << QDateTime::currentDateTime().toString("ss:zzz").toStdString() << "\n";
}

void tester_device_in_proccess::onSerialError()
{
    if(port->error() != 0) {
        std::cout  << "#onSerialError " << port->errorString().toStdString() << " " << port->error() << "\n";
    } else {
        std::cout  << "onSerialError OK.\n";
    }
}

void tester_device_in_proccess::on_master_writer()
{
    buffer.clear();
    switch (state) {
    case STATE_1:
        port->write(QByteArray::fromHex("01100000000102f100e3c0"));
        std::cout  << "write 01100000000102f100e3c0\n";
        break;
    case STATE_2:
        port->write(QByteArray::fromHex("010300010001d5ca"));
        std::cout  << "write 010300010001d5ca\n";
        break;
    case STATE_3:
        port->close();
        port->deleteLater();
//        port_init();
        QTimer::singleShot(10, QCoreApplication::instance(), QCoreApplication::quit);
//        port->write(QByteArray::fromHex("02100000000102f100f730"));
        state = STATE_1;
        break;
    }
}

void tester_device_in_proccess::port_init()
{
//    port = nullptr;
    port = new QSerialPort("COM3", this);
    connect (port, &QSerialPort::readyRead    , this, &tester_device_in_proccess::onSerialReadyRead   );
    connect (port, &QSerialPort::bytesWritten , this, &tester_device_in_proccess::onSerialBytesWritten);
    connect (port, &QSerialPort::errorOccurred, this, &tester_device_in_proccess::onSerialError       );

    port->setFlowControl(QSerialPort::NoFlowControl);
    port->setBaudRate   (QSerialPort::Baud115200   );
    port->setParity     (QSerialPort::EvenParity   );
    port->setStopBits   (QSerialPort::OneStop      );
    port->setDataBits   (QSerialPort::Data8        );
    if (port->isOpen()) {
        std::cout << "port already opened";
    } else {
        if (!port->open(QIODevice::ReadWrite)) {
            std::cout  << "port not open\n";
            return;
        } else {
            std::cout  << "port opened\n";
        }
    }

    fflush (stderr);
}

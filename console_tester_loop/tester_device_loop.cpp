#include "tester_device_loop.h"


tester_device_loop::tester_device_loop(QObject *parent)
    : QObject{parent}
{
    m_timeout.setSingleShot(true);
    m_timeout.setInterval(500);

}

tester_device_loop::~tester_device_loop()
{
    port->close();
    port->deleteLater();

    std::cout << "~destruct_test\n";
}

void tester_device_loop::onSerialReadyRead()
{
    m_timeout.stop();
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
        if(buffer == "01030400000000fa33")
            state = STATE_3;
        else
            return;
        break;
    case STATE_3:
        if(buffer == "01100000000101c9") {
            std::cout << std::endl;
            state = STATE_1;
        }
        break;
    }
    QTimer::singleShot(1000, this, &tester_device_loop::on_master_writer);

}

void tester_device_loop::onSerialBytesWritten()
{
    std::cout  << "bytesWritten " << QDateTime::currentDateTime().toString("ss:zzz").toStdString() << "\n";
}

void tester_device_loop::onSerialError()
{
    if(port->error() != 0) {
        std::cout  << "#onSerialError " << port->errorString().toStdString() << " " << port->error() << "\n";
    } else {
        std::cout  << "onSerialError OK.\n";
    }
}

void tester_device_loop::on_master_writer()
{
    buffer.clear();
    switch (state) {
    case STATE_1:
        port->write(QByteArray::fromHex("0110000000010222017f30"));
        std::cout  << "write 0110000000010222017f30\n";
        break;
    case STATE_2:
        port->write(QByteArray::fromHex("01030008000245c9"));
        std::cout  << "write 01030008000245c9\n";
        break;
    case STATE_3:
        port->write(QByteArray::fromHex("0110000000010223017ea0"));
        std::cout  << "write 0110000000010223017ea0\n";
        break;
    }
    m_timeout.start(3000);
}

void tester_device_loop::port_init()
{
    port = new QSerialPort("COM3", this);
    connect (port, &QSerialPort::readyRead    , this, &tester_device_loop::onSerialReadyRead   );
    connect (port, &QSerialPort::bytesWritten , this, &tester_device_loop::onSerialBytesWritten);
    connect (port, &QSerialPort::errorOccurred, this, &tester_device_loop::onSerialError       );

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
    QTimer::singleShot(10, this, &tester_device_loop::on_master_writer);
    connect(&m_timeout, &QTimer::timeout, this, &tester_device_loop::on_master_writer);
    fflush (stderr);
}


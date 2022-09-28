#include <QtSerialPort/QSerialPortInfo>
#include "devicemanager.h"
#include <QTime>
#include <QDebug>
#include <QDataStream>


DeviceManager::DeviceManager(QStringList port_for_scan, QObject *parent)
    : QObject{parent}
{
    current_device = new Device;
    m_portForScans = port_for_scan;
}

DeviceManager::~DeviceManager()
{
    if (current_device->port->isOpen())
        current_device->port->close();
}

void DeviceManager::scan_com()
{
    m_portNameRegisters.clear();
    auto availablePorts = QSerialPortInfo::availablePorts();
    for (const auto &port: availablePorts) {
        qDebug() << port.portName();
        if (m_portForScans.contains(port.portName()))
            m_portNameRegisters.append(port.portName());
    }
}

void DeviceManager::scan_device()
{
    try {
        scan_com();
        for (const auto &name: qAsConst(m_portNameRegisters)) {
            if (create_port(name)) {
                if (probeDevice(name)) {
                    connect(current_device->port, &QSerialPort::readyRead, this, &DeviceManager::receiveData);
                }
            }
            
        }
    }  catch (...) {
        
    }

}

bool DeviceManager::create_port(const QString &name)
{
    current_device->port = nullptr;
    current_device->port_name.clear();
    current_device->port = new QSerialPort(name, this);
    if (!current_device->port)
        return false;
    current_device->port_name = name;
    current_device->port->setFlowControl(QSerialPort::NoFlowControl);
    current_device->port->setBaudRate   (QSerialPort::Baud115200   );
    current_device->port->setParity     (QSerialPort::EvenParity   );
    current_device->port->setStopBits   (QSerialPort::OneStop      );
    current_device->port->setDataBits   (QSerialPort::Data8        );
    return true;
}

void DeviceManager::receiveData()
{
    current_device->receive_buff = current_device->port->readAll();
    if (!parse_response(current_device, 1)) {
        qDebug() << "another address" << current_device->receive_buff;
        return;
    }
    QByteArray message;
    message += "<--";
    message += current_device->receive_buff.toHex() + "\n";

    if(current_device->response_buff.isEmpty()) {
        message += "--> command not found";
    } else {
        message += "-->" + current_device->response_buff.toHex();

    }
    qDebug() << qPrintable(message);
    current_device->port->write(current_device->response_buff);
    current_device->port->flush();
}

bool DeviceManager::probeDevice(const QString &port_name)
{
    if(current_device->port != nullptr) {
        if(current_device->port->isOpen()) {
            qDebug() << "port:" << port_name << "opening";
            return true;
        } else {
            if (current_device->port->open(QIODevice::ReadWrite)) {
                qDebug() << "port:" << port_name << "opening";
                return true;
            }
            qDebug() << "port:" << port_name << "not open";
            return false;
        }
    } else {
        qDebug() << "Error: DeviceManager::probeDevice -> current_device == nullptr";
    }
    return false;
}

bool DeviceManager::parse_response(Device *device, int address = 1)
{
    device->response_buff.clear();
    if (device->receive_buff.data()[0] == address) {
        if (device->receive_buff == QByteArray::fromHex(QString("01100000000102f100e3c0").toUtf8())) {
            device->port->clear(QSerialPort::AllDirections);
            device->response_buff = QByteArray::fromHex(QString("01100000000101c9").toUtf8());
            qDebug() << "";
        } else if (device->receive_buff == QByteArray::fromHex(QString("010300010001d5ca").toUtf8())) {
            device->port->clear(QSerialPort::AllDirections);
            device->response_buff = QByteArray::fromHex(QString("01030200017984").toUtf8());
        } else if (device->receive_buff == QByteArray::fromHex(QString("0110000000010222017f30").toUtf8())) {
            device->port->clear(QSerialPort::AllDirections);
            device->response_buff = QByteArray::fromHex(QString("01100000000101c9").toUtf8());
        } else if (device->receive_buff == QByteArray::fromHex(QString("010300010001d5ca").toUtf8())) {
            device->port->clear(QSerialPort::AllDirections);
            device->response_buff = QByteArray::fromHex(QString("01030200017984").toUtf8());
        } else if (device->receive_buff == QByteArray::fromHex(QString("0110000000010222017f30").toUtf8())) {
            device->port->clear(QSerialPort::AllDirections);
            device->response_buff = QByteArray::fromHex(QString("01100000000101c9").toUtf8());
        } else if (device->receive_buff == QByteArray::fromHex(QString("01030008000245c9").toUtf8())) {
            device->port->clear(QSerialPort::AllDirections);
            device->response_buff = QByteArray::fromHex(QString("01030400000000fa33").toUtf8());
        } else if (device->receive_buff == QByteArray::fromHex(QString("0110000000010223017ea0").toUtf8())) {
            device->port->clear(QSerialPort::AllDirections);
            device->response_buff = QByteArray::fromHex(QString("01100000000101c9").toUtf8());
        } else if (device->receive_buff == QByteArray::fromHex(QString("011000000001022200bef0").toUtf8())) {
            device->port->clear(QSerialPort::AllDirections);
            device->response_buff = QByteArray::fromHex(QString("01100000000101c9").toUtf8());
        } else if (device->receive_buff == QByteArray::fromHex(QString("011000000001022300bf60").toUtf8())) {
            device->port->clear(QSerialPort::AllDirections);
            device->response_buff = QByteArray::fromHex(QString("01100000000101c9").toUtf8());
        } else if (device->receive_buff == QByteArray::fromHex(QString("010300060002240a").toUtf8())) {
            device->port->clear(QSerialPort::AllDirections);
            device->response_buff = QByteArray::fromHex(QString("010304000200005bf3").toUtf8());
        }
        return true;
    }
    return false;
}

//*********************************************************************************************

ExperimentThread::ExperimentThread(QObject *parent) :
    QThread(parent)
{
}

ExperimentThread::~ExperimentThread()
{
    m_mutex.lock();
    m_quit = true;
    m_cond.wakeOne();
    m_mutex.unlock();
    wait();
}

void ExperimentThread::transaction(const QString &portName, int waitTimeout, const QString &request)
{
    const QMutexLocker locker(&m_mutex);
    m_portName = portName;
    m_waitTimeout = waitTimeout;
    m_request = request;
    if (!isRunning())
        start();
    else
        m_cond.wakeOne();
}

void ExperimentThread::run()
{
    bool currentPortNameChanged = false;

    m_mutex.lock();
    QString currentPortName;
    if (currentPortName != m_portName) {
        currentPortName = m_portName;
        currentPortNameChanged = true;
    }

    int currentWaitTimeout = m_waitTimeout;
    QString currentRequest = m_request;
    m_mutex.unlock();
    QSerialPort serial;

    if (currentPortName.isEmpty()) {
        emit error(tr("No port name specified"));
        return;
    }

    while (!m_quit) {
        if (currentPortNameChanged) {
            serial.close();
            serial.setPortName(currentPortName);

            if (!serial.open(QIODevice::ReadWrite)) {
                emit error(tr("Can't open %1, error code %2")
                           .arg(m_portName).arg(serial.error()));
                return;
            }
        }
        // write request
        const QByteArray requestData = currentRequest.toUtf8();
        serial.write(requestData);
        if (serial.waitForBytesWritten(m_waitTimeout)) {
            // read response
            if (serial.waitForReadyRead(currentWaitTimeout)) {
                QByteArray responseData = serial.readAll();
                while (serial.waitForReadyRead(10))
                    responseData += serial.readAll();

                const QString response = QString::fromUtf8(responseData);
                emit this->response(response);
            } else {
                emit timeout(tr("Wait read response timeout %1")
                             .arg(QTime::currentTime().toString()));
            }
        } else {
            emit timeout(tr("Wait write request timeout %1")
                         .arg(QTime::currentTime().toString()));
        }
        m_mutex.lock();
        m_cond.wait(&m_mutex);
        if (currentPortName != m_portName) {
            currentPortName = m_portName;
            currentPortNameChanged = true;
        } else {
            currentPortNameChanged = false;
        }
        currentWaitTimeout = m_waitTimeout;
        currentRequest = m_request;
        m_mutex.unlock();
    }
}

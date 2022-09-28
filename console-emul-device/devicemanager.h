#ifndef DEVICEMANAGER_H
#define DEVICEMANAGER_H

#include <QObject>
#include <QMutex>
#include <QThread>
#include <QWaitCondition>
#include <QtSerialPort/QSerialPort>
#include <QMap>


class ExperimentThread : public QThread
{
    Q_OBJECT

public:
    explicit ExperimentThread(QObject *parent = nullptr);
    ~ExperimentThread();

    void transaction(const QString &portName, int waitTimeout, const QString &request);

signals:
    void response(const QString &s);
    void error(const QString &s);
    void timeout(const QString &s);

private:
    void run() override;

    QString m_portName;
    QString m_request;
    int m_waitTimeout = 0;
    QMutex m_mutex;
    QWaitCondition m_cond;
    bool m_quit = false;
};

/**
 * @brief The DeviceManager class   Поиск устройств
 */
class DeviceManager : public QObject
{
    Q_OBJECT


    enum STATE {STATE_OPEN, STATE_PROBE} state;

    struct Device {
        QSerialPort *port;
        STATE state;
        QString port_name;
        QByteArray receive_buff;
        QByteArray response_buff;
    };


public:
    explicit DeviceManager(QStringList port_for_scan, QObject *parent = nullptr);
    ~DeviceManager();
    /**
     * @brief scan_com      найти COM порты
     */
    void scan_com();

    /**
     * @brief scan_device   найти устройства
     */
    void scan_device();

    bool create_port(const QString &);
signals:

private slots:
    void receiveData();

private:
    bool probeDevice(const QString& port);
    bool parse_response(Device *device, int address);

    QList<QString> m_portForScans;
    QList<QString> m_portNameRegisters;

    Device *current_device;

};

#endif // DEVICEMANAGER_H

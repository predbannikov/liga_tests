#ifndef TEST_OPEN_SERIALPORT_IN_PROCESS_H
#define TEST_OPEN_SERIALPORT_IN_PROCESS_H

#include <QObject>
#include <QSerialPort>

class tester_device_in_proccess : public QObject
{
    Q_OBJECT
    enum STATE {STATE_1, STATE_2, STATE_3} state = STATE_1;
public:
    explicit tester_device_in_proccess(QObject *parent = nullptr);
    ~tester_device_in_proccess();
public slots:
    void onSerialReadyRead();
    void onSerialBytesWritten();
    void onSerialError();

private slots:

    void on_master_writer();

private:

    void port_init();
    QSerialPort *port;


    QString buffer;

signals:

};


#endif // TEST_OPEN_SERIALPORT_IN_PROCESS_H

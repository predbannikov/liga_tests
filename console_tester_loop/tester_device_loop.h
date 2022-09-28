#ifndef TESTER_DEVICE_LOOP_H
#define TESTER_DEVICE_LOOP_H

#include <QObject>
#include <QSerialPort>
#include <QTimer>
#include <QDebug>
#include <QDateTime>
#include <iostream>
#include <QCoreApplication>


class tester_device_loop : public QObject
{
    Q_OBJECT
    enum STATE {STATE_1, STATE_2, STATE_3} state = STATE_1;
public:
    explicit tester_device_loop(QObject *parent = nullptr);
    ~tester_device_loop();
    void port_init();
public slots:
    void onSerialReadyRead();
    void onSerialBytesWritten();
    void onSerialError();

private slots:

    void on_master_writer();

private:

    QSerialPort *port;


    QString buffer;
    QTimer m_timeout;

signals:

};


#endif // TESTER_DEVICE_LOOP_H

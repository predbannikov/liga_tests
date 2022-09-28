#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QTimer>
#include <QProcess>
#include <QDir>
#include <QThread>

ReaderWindow::ReaderWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
//    port_init();

}

ReaderWindow::~ReaderWindow()
{
    port->close();
    delete ui;
}

void ReaderWindow::onSerialReadyRead()
{
    QString tmp = port->readAll().toHex();
    buffer.append(tmp);
    qDebug() << "readyRead" << tmp;

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
        port->write(QByteArray::fromHex("02100000000102f100f730"));
        break;
    }

    QTimer::singleShot(300, this, &ReaderWindow::on_test);
}

void ReaderWindow::onSerialBytesWritten()
{
    qDebug() << "bytesWritten";
}

void ReaderWindow::onSerialError()
{
    if(port->error() != 0) {
        qDebug() << "onSerialBytesWritten" << port->errorString() << port->error();
    } else {
        qDebug() << "onSerialError OK.";
    }
}


void ReaderWindow::on_pushButton_clicked()
{
    port->write(QByteArray::fromHex(ui->lineEdit->text().toUtf8()));


}


void ReaderWindow::on_pushButton_2_clicked()
{
    on_test();
}

void ReaderWindow::on_test()
{
    buffer.clear();
    switch (state) {
    case STATE_1:
        port->write(QByteArray::fromHex("01100000000102f100e3c0"));
        qDebug() << "write 01100000000102f100e3c0";
        break;
    case STATE_2:
        port->write(QByteArray::fromHex("010300010001d5ca"));
        qDebug() << "write 010300010001d5ca";
        break;
    case STATE_3:
        port->close();
        port->deleteLater();
        port_init();
        QTimer::singleShot(100, this, &ReaderWindow::on_test);
//        port->write(QByteArray::fromHex("02100000000102f100f730"));
        state = STATE_1;
        break;
    }
}

void ReaderWindow::on_test2()
{
    QProcess proc(this);
    QDir dir = QDir::currentPath();
    QDir path_program = dir.filePath("../console-tester-device/debug");
    QString path_file = path_program.absoluteFilePath("console-tester-device.exe");
    QString path_outlog = path_program.absoluteFilePath("log.txt");
    QString path_logerror = path_program.absoluteFilePath("error_log.txt");
    qDebug() << path_file;
    qint64 pid;
    for (int i = 0; i < 1000; i++) {
        QProcess *process = new QProcess;
        process->setProgram(path_file);
        process->setStandardOutputFile(path_outlog, QIODevice::Append);
        process->setStandardErrorFile(path_logerror, QIODevice::Append);
//        if (process->startDetached(&pid)) {
//            qDebug() << "deatached program" << pid;
//        } else {
//            qDebug() << "not deatached program" << pid;
//        }

        process->start();
        if (process->waitForFinished(-1)) {
            qDebug() << "finish";
        } else {
            qDebug() << "not finish";
        }

        process->deleteLater();

//        QThread::msleep(1300);
//        QThread::usleep(3000);
    }
    qDebug() << "passed test";
//    proc.setProgram("")
}

void ReaderWindow::port_init()
{
//    port = nullptr;
    port = new QSerialPort("COM3", this);
    connect (port, &QSerialPort::readyRead    , this, &ReaderWindow::onSerialReadyRead   );
    connect (port, &QSerialPort::bytesWritten , this, &ReaderWindow::onSerialBytesWritten);
    connect (port, &QSerialPort::errorOccurred, this, &ReaderWindow::onSerialError       );

    port->setFlowControl(QSerialPort::NoFlowControl);
    port->setBaudRate   (QSerialPort::Baud115200   );
    port->setParity     (QSerialPort::EvenParity   );
    port->setStopBits   (QSerialPort::OneStop      );
    port->setDataBits   (QSerialPort::Data8        );
    if (!port->open(QIODevice::ReadWrite)) {
        qDebug() << "port not open";
    } else {
        qDebug() << "port opened";
    }

}


void ReaderWindow::on_pushButton_3_clicked()
{
    on_test2();
}


#include <QCoreApplication>
#include <QDebug>
#include "devicemanager.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    qDebug() << "hello";
    DeviceManager devices({"COM4"}, &a);
    devices.scan_device();


    return a.exec();
}

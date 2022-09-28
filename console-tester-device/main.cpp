#include <QCoreApplication>
#include <QTimer>
#include <QDebug>
#include <iostream>

#include "test_open_serialport_in_process.h"



int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    tester_device_in_proccess *test = new tester_device_in_proccess(&a);


//    QTimer::singleShot(1000, &a, QCoreApplication::quit);
    return a.exec();
}

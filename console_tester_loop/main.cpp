#include <QCoreApplication>
#include "tester_device_loop.h"



int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    tester_device_loop test_loop;
    test_loop.port_init();

    return a.exec();
}

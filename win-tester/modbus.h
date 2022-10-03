#ifndef MODBUS_H_
#define MODBUS_H_

#include <QDebug>
#include <QByteArray>

struct Modbus
{
//	struct USART *usart;
    qint8 address;

    QByteArray data;
//	int dataLength;

//	unsigned char *frame;
//	int frameLength;
//	int maxFrameLength;
};

void Modbus_Init(struct Modbus *modbus, unsigned char address);
unsigned char Modbus_ServerAddress(struct Modbus *modbus);
int Modbus_ProcessFrame(struct Modbus *modbus);

#endif /* MODBUS_H_ */

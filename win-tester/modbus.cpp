//#include "usart.h"
#include "modbus.h"

#define DATA_OFFSET_FC03 3
#define DATA_OFFSET_FC06 4
#define DATA_OFFSET_FC16 7

#define TX_HEADER_SIZE 3
#define RX_HEADER_SIZE 6

#define CRC_SIZE  2
#define WORD_SIZE 2

enum ModbusCommand {
	ReadMultipleReg  = 0x03,
	WriteSingleReg   = 0x06,
	WriteMultipleReg = 0x10
};

enum ModbusOffset {
	Address = 0,
	FuncCode,
	StartAddrHi,
	StartAddrLo,
	CountHi,
	CountLo,
	NumBytes
};

static void processCode03(struct Modbus *modbus);
static void processCode06(struct Modbus *modbus);
static void processCode16(struct Modbus *modbus);

static void throwException(struct Modbus *modbus);
static int checkCRC(unsigned char *frame, int length);
static qint16 makeCRC(unsigned char *data, int len);

void Modbus_Init(struct Modbus *modbus, unsigned char address)
{
	modbus->address = address;

//	USART_Init(modbus->usart);

//	modbus->usart->port->RTOR = 22;
//	modbus->usart->port->CR2 |= USART_CR2_RTOEN;
//	modbus->usart->port->CR1 |= USART_CR1_RTOIE;
}

int Modbus_ProcessFrame(struct Modbus *modbus)
{
//	struct USART *usart = modbus->usart;
    if (!modbus->data.isEmpty()) {
		/* A frame was received */
//		modbus->frameLength = USART_Available(usart);
//		USART_Gets(usart, modbus->frame, modbus->frameLength);
        if((qint8)modbus->data[Address] != modbus->address)
			return 0;
//		else if(checkCRC(modbus->frame, modbus->frameLength) == 0)
//			return 0;

		/* Address & CRC checks out */
        switch(modbus->data[FuncCode]) {
		case ReadMultipleReg:
			processCode03(modbus);
			break;
		case WriteSingleReg:
			processCode06(modbus);
			break;
		case WriteMultipleReg:
			processCode16(modbus);
			break;
		default:
			throwException(modbus);
		}

		return 1;
	}
	return 0;
}

unsigned char Modbus_ServerAddress(struct Modbus *modbus)
{
	return modbus->address;
}

static void processCode03(struct Modbus *modbus)
{
    const int startaddr = modbus->data[StartAddrLo];
    const int count = modbus->data[CountLo];

	for(int i = 0; i < count; ++i) {
		const int srcOffset = WORD_SIZE * (i + startaddr);
		const int destOffset = (WORD_SIZE * i) + DATA_OFFSET_FC03;

        modbus->data[destOffset] = modbus->data[srcOffset + 1];
        modbus->data[destOffset + 1] = modbus->data[srcOffset];
	}

    modbus->data[2] = (unsigned char)(count * WORD_SIZE);

    qint16 crc = makeCRC((unsigned char*)modbus->data.data(), TX_HEADER_SIZE + count * WORD_SIZE);

//	USART_Puts(modbus->usart, modbus->frame, TX_HEADER_SIZE + count * WORD_SIZE);
//	USART_Puts(modbus->usart, (unsigned char*)&crc, CRC_SIZE);
}

static void processCode06(struct Modbus *modbus)
{
    const int startaddr = modbus->data[StartAddrLo];
	const int srcOffset = DATA_OFFSET_FC06;
	const int destOffset = WORD_SIZE * startaddr;

    modbus->data[destOffset + 1] = modbus->data[srcOffset];
    modbus->data[destOffset] = modbus->data[srcOffset + 1];

    qint16 crc = makeCRC((unsigned char *)modbus->data.data(), RX_HEADER_SIZE);
//	USART_Puts(modbus->usart, modbus->frame, RX_HEADER_SIZE);
//	USART_Puts(modbus->usart, (unsigned char*)&crc, CRC_SIZE);
}

static void processCode16(struct Modbus *modbus)
{
    const int startaddr = modbus->data[StartAddrLo];
    const int count = modbus->data[CountLo];

	for(int i = 0; i < count; ++i) {
		const int srcOffset = (WORD_SIZE * i) + DATA_OFFSET_FC16;
		const int destOffset = WORD_SIZE * (i + startaddr);

        modbus->data[destOffset + 1] = modbus->data[srcOffset];
        modbus->data[destOffset] = modbus->data[srcOffset + 1];
	}

    qint16 crc = makeCRC((unsigned char *)modbus->data.data(), RX_HEADER_SIZE);
//	USART_Puts(modbus->usart, modbus->frame, RX_HEADER_SIZE);
//	USART_Puts(modbus->usart, (unsigned char*)&crc, CRC_SIZE);
}

static void throwException(struct Modbus *modbus)
{
    unsigned char err[] = {static_cast<unsigned char>(modbus->address), (unsigned char)(modbus->data[FuncCode] + 0x80), 0x01};
    qint16 crc = makeCRC(err, TX_HEADER_SIZE);

//	USART_Puts(modbus->usart, err, TX_HEADER_SIZE);
//	USART_Puts(modbus->usart, (unsigned char*)&crc, CRC_SIZE);
}

static int checkCRC(unsigned char *frame, int length)
{
    const qint16 crcCalculated = makeCRC(frame, length - CRC_SIZE);
    const qint16 crcReceived = *(qint16*)(frame + length - CRC_SIZE);
	return crcCalculated == crcReceived;
}

static qint16 makeCRC(unsigned char *data, int len)
{
    qint16 result = 0xffff;

	if(len > 0) {
		for(int i = 0; i < len; ++i) {
            result = (qint16)(result ^ data[i]);
			for(int j = 0; j < 8; ++j) {
				unsigned char msb = result & 0x0001;
				result >>= 1;
				if(msb)
					result ^= 0xa001;
			}
		}
	}

	return result;
}

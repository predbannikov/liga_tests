#ifndef READERWINDOW_H
#define READERWINDOW_H

#include <QMainWindow>
#include <QFile>
#include <QDateTime>
#include <QDebug>
#include <QDir>
#include <QMap>
#include <QVector>
#include <QFileDialog>



QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class ReaderWindow : public QMainWindow
{
    Q_OBJECT
    /**
     * @brief The STATE_ADU enum    Application Data Unit
     */
    enum STATE_ADU {STATE_ADU_ADDRESS, STATE_ADU_PDU, STATE_ADU_CRC};

    enum STATE_PDU {STATE_PDU_FUNC_CODE, STATE_PDU_0x10_START_ADDRESS, STATE_PDU_0x10_COUNT_REGISTERS, STATE_PDU_0x10_COUNT_BYTES, STATE_PDU_0x10_DATA,
                   STATE_PDU_0x03_START_ADDRESS, STATE_PDU_0x03_COUNT_REGISTERS, STATE_PDU_0x03_COUNT_BYTES, STATE_PDU_0x03_DATA};
//                    STATE_PDU_FUNC_CODE_0x03 = 0x03, STATE_PDU_FUNC_CODE_0x10 = 0x10};

    enum STATE_DIRECT {STATE_DIRECT_MASTER, STATE_DIRECT_SLAVE};

    const int length_start_address = 2;

    struct TwoBytesData {
        quint16 value;
        const quint8 length = 2;
        quint8 idx;
    };

    struct STATE {
        STATE_ADU adu = STATE_ADU_ADDRESS;
        STATE_PDU pdu =STATE_PDU_FUNC_CODE;
        STATE_DIRECT direct;
    };

    struct PDU{
        quint8 func_code = 0;
//            quint16 start_address = 0;
        TwoBytesData start_addr;
        TwoBytesData registers_count;
        quint8  bytes_count;
        QByteArray data;
    };

    struct ADU {
        ADU(quint8 val = 0) : addr_device(val) {}
        quint8 addr_device;
        PDU pdu;
        TwoBytesData crc16;
    };

    struct Device {
        const Device& operator=(const Device &obj) {
            this->adu.pdu.data = obj.adu.pdu.data;
            return obj;
        };
        Device(quint8 val = 0x7F) : number(val) {	adu.addr_device = val; }
        void init(STATE_DIRECT direct) {
            adu.pdu.func_code = 0xff;
            adu.pdu.start_addr.idx = 0;
            adu.pdu.registers_count.idx = 0;
            adu.crc16.idx = 0;
            state.direct = direct;
            adu.pdu.data.clear();
        }
        quint8 number;
        ADU adu;
        STATE state;
        QList <QByteArray*> packages;
    };

    QMap <unsigned char, Device*> devices;
    QVector <Device> mem;
//    ADU adu;
//    STATE *state;
    Device *device;
    Device *unknown_device;
//    StartAddress start_address;
//    CountRegisters count_registers;
//    bool start_address_complate = false;

public:
    ReaderWindow(QWidget *parent = nullptr);
    ~ReaderWindow();

private slots:
    void on_readFileBtn_clicked();

    void on_parseFileBtn_clicked();

    void on_chckbxSaveToFile_clicked(bool checked);

    void on_openFileBtn_clicked();

private:
    bool parse_adu(unsigned char byte);
    bool parse_pdu(unsigned char byte);
    bool parse_two_bytes(TwoBytesData &data, unsigned char byte);
    bool parse_data_value(QByteArray &data, unsigned char count_bytes, unsigned char byte);

    const QString parse_value(const QByteArray &data);
    QString parse_value_device(const quint8 byte);

    Ui::MainWindow *ui;

    QFile file;
    QFile file_out;
    quint64 size_file;

    QTextStream text_stream;
    QByteArray *data;

    QString file_name;
};
#endif // READERWINDOW_H

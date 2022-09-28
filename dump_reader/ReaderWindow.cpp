#include "ReaderWindow.h"
#include "ui_ReaderWindow.h"
#include <QtEndian>
#include <utility>
#include <QElapsedTimer>
#include <QTimer>


ReaderWindow::ReaderWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->parseFileBtn->setEnabled(false);
    unknown_device = new Device(-1);
    device = unknown_device;
//    on_readFileBtn_clicked();
//    on_parseFileBtn_clicked();
    ui->progressBar->setMinimum(0);
    ui->progressBar->setMaximum(100);
    ui->progressBar->setValue(0);


}

ReaderWindow::~ReaderWindow()
{
    file.close();
    file_out.close();
    for (auto item: mem) {
        for (size_t i = 0; i < item.packages.size(); ++i)
            delete item.packages.at(i);
    }
    delete ui;
}


void ReaderWindow::on_readFileBtn_clicked()
{
    if (file.isOpen())
        file.close();
    file.setFileName(file_name);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "file not opened";
    } else {
        ui->parseFileBtn->setEnabled(true);
        ui->chckbxSaveToFile->setEnabled(true);
        size_file = file.size();
        ui->progressBar->setMaximum(size_file);
        qDebug() << "Size file =" << size_file;
    }
    QString file_name_out = file.fileName();
    file_name_out.chop(4);
    file_out.setFileName(file_name_out + "-prep.txt");
}


void ReaderWindow::on_parseFileBtn_clicked()
{
    int i = 0;
    int count_line_str = 0;
    try {

        QElapsedTimer time_elapse;
        time_elapse.start();

        QTextStream str(&file);

        if (ui->chckbxSaveToFile->isChecked())
            text_stream.setDevice(&file_out);

        quint64 current_size_read = 0;
        QTimer timer_progress;
        connect(&timer_progress, &QTimer::timeout, [this, &current_size_read]() {
            this->ui->progressBar->setValue(current_size_read);
        });
        timer_progress.start(1000);

        while (!str.atEnd()) {
            count_line_str++;
            if (count_line_str == 29715) {
                // 13 пакет - мусор, по состоянию device не прошёл весь путь ,
                // необходимо найти способ определять это и сбрасывать device как новое соединение
                //
                qDebug() << "stop";
            }
            QString line = str.readLine();
            QStringList list = line.split(',');
            current_size_read += line.size() + 2;



            if (device->state.adu != STATE_ADU_ADDRESS) {
//                qDebug() << "state adu not reset" << list[2];
            }
            if (device->state.pdu != STATE_PDU_FUNC_CODE) {
//                qDebug() << "state pdu not reset" << list[2];
            }

            if (list[1] == "w")
                unknown_device->state.direct = STATE_DIRECT_MASTER;
            if (list[1] == "r")
                unknown_device->state.direct = STATE_DIRECT_SLAVE;
            if (list[1] == "w" || list[1] == "r") {

                data = new QByteArray(QByteArray::fromHex(list[2].toUtf8()));


                try {
                    for(i = 0; i < data->size(); ++i) {
                        if (!parse_adu(data->at(i))) break;
                    }
                }  catch (STATE_ADU err) {
                    switch (err) {
                    case ReaderWindow::STATE_ADU_PDU:
                        break;
                    case ReaderWindow::STATE_ADU_CRC:
                        break;
                    case ReaderWindow::STATE_ADU_ADDRESS:
                        qDebug() << "Exception: adu address";
                        qDebug() << list[2] << QString::number(quint8(data->at(i)), 16) << "i =" << i << " num_line =" << count_line_str << "\n";
                        continue;
                    default:
                        throw ;
                    }
                }
            }
            qApp->processEvents();
        }
        ui->progressBar->setValue(file.size());
        qDebug() << time_elapse.elapsed() / 1000 << "sec";
    }  catch (...) {
        qDebug() << "Error: exception i =" << i << " count_line_str =" << count_line_str;
    }

}


bool ReaderWindow::parse_adu(unsigned char byte)
{
    switch (device->state.adu) {
    case ReaderWindow::STATE_ADU_ADDRESS:

        if (byte >= 0 && byte <= 127) {
            if (!devices.contains(byte))
                devices.insert(byte, new Device(byte));
            // инициализируем устройства только так, потому как пакеты могут быть рваные
            device = devices.value(byte);
            device->init(unknown_device->state.direct);
            device->state.adu = STATE_ADU_PDU;
        } else {
            qDebug() << QString("WARNING address = 0x%1").arg(device->adu.addr_device);
            throw device->state.adu;
        }
        break;
    case ReaderWindow::STATE_ADU_PDU:
        if (parse_pdu(byte))
            device->state.adu = STATE_ADU_CRC;
        break;
    case ReaderWindow::STATE_ADU_CRC:
        if (parse_two_bytes(device->adu.crc16, byte)) {
//            device->packages.append(data);
//            mem.append(*device);
            device->state.adu = STATE_ADU_ADDRESS;
            device->state.pdu = STATE_PDU_FUNC_CODE;
            QString msg_out;
            return true;
            if (device->state.direct == STATE_DIRECT_MASTER) {
                if (device->adu.pdu.func_code == 0x10)
                    msg_out = QString("MSTR_0x10: Записать по %1 адресу %2 регистров, %3 байт, значение - { %4 }")
                                .arg(device->adu.pdu.start_addr.value)
                                .arg(device->adu.pdu.registers_count.value)
                                .arg(device->adu.pdu.bytes_count)
                                .arg(parse_value(device->adu.pdu.data));
                else
                    msg_out = QString("MSTR_0x03: Читать по %1 адресу %2 регистров")
                                .arg(device->adu.pdu.start_addr.value)
                                .arg(device->adu.pdu.registers_count.value);


            } else {
                if (device->adu.pdu.func_code == 0x10)
                    msg_out = QString("SLV_0x10: Записал по %1 адресу %2 регистров")
                                .arg(device->adu.pdu.start_addr.value)
                                .arg(device->adu.pdu.registers_count.value);
                else
                    msg_out = QString("SLV_0x03: передаю %1 байт. %2")
                                .arg(device->adu.pdu.bytes_count).arg(QString(device->adu.pdu.data.toHex()));


            }
//            if (!ui->chckbxSaveToFile->isChecked())

//                qDebug() << msg_out;
//            else {
//                text_stream << msg_out;
//                file_out.write(msg_out.toUtf8());
//            }

//            if (mem.size() == 4)
//                qDebug() << "mem = N" ;
//            qDebug() << "case STATE_ADU_CRC";
        }

        break;
    default:
        qDebug() << "STATE_ADU_CRC not handled" << device->adu.pdu.func_code;
        throw device->state.adu;
    }
    return true;
}

bool ReaderWindow::parse_pdu(unsigned char byte)
{
    switch (device->state.pdu) {

    case ReaderWindow::STATE_PDU_FUNC_CODE:
        device->adu.pdu.func_code = byte;
        if (device->adu.pdu.func_code == 0x03) {
            if (device->state.direct == STATE_DIRECT_MASTER)
                device->state.pdu = STATE_PDU_0x03_START_ADDRESS;
            else
                device->state.pdu = STATE_PDU_0x03_COUNT_BYTES;
        } else
            device->state.pdu = STATE_PDU_0x10_START_ADDRESS;

        break;
    case ReaderWindow::STATE_PDU_0x10_START_ADDRESS:
        if (parse_two_bytes(device->adu.pdu.start_addr, byte))
            device->state.pdu = STATE_PDU_0x10_COUNT_REGISTERS;

        break;
    case ReaderWindow::STATE_PDU_0x10_COUNT_REGISTERS:
        if (parse_two_bytes(device->adu.pdu.registers_count, byte)) {
            if (device->state.direct == STATE_DIRECT_MASTER)
                device->state.pdu = STATE_PDU_0x10_COUNT_BYTES;
            else
                return true;
        }
        break;
    case ReaderWindow::STATE_PDU_0x10_COUNT_BYTES:
        device->adu.pdu.bytes_count = byte;
        device->state.pdu = STATE_PDU_0x10_DATA;
        break;
    case ReaderWindow::STATE_PDU_0x10_DATA:
        if (parse_data_value(device->adu.pdu.data, device->adu.pdu.bytes_count, byte))
            return true;
        break;
//-------------------------------------------------
        case ReaderWindow::STATE_PDU_0x03_START_ADDRESS:
        if (parse_two_bytes(device->adu.pdu.start_addr, byte))
            device->state.pdu = STATE_PDU_0x03_COUNT_REGISTERS;
        break;
    case ReaderWindow::STATE_PDU_0x03_COUNT_REGISTERS:
        if (parse_two_bytes(device->adu.pdu.registers_count, byte))
            return true;
        break;
    case ReaderWindow::STATE_PDU_0x03_COUNT_BYTES:
        device->adu.pdu.bytes_count = byte;
        device->state.pdu = STATE_PDU_0x03_DATA;
        break;

    case ReaderWindow::STATE_PDU_0x03_DATA:
        if (parse_data_value(device->adu.pdu.data, device->adu.pdu.bytes_count, byte))
            return true;
        break;

    default:
        throw device->state.pdu;
    }
    return false;
}

bool ReaderWindow::parse_two_bytes(TwoBytesData &data, unsigned char byte)
{
    if (++data.idx == 1)
        data.value = byte;
    else if (data.idx == 2) {
        data.value <<= 8;
        data.value |= byte;
        return true;
    }
    return false;
}

bool ReaderWindow::parse_data_value(QByteArray &data, unsigned char count_bytes, unsigned char byte)
{
    data.push_back(byte);
    if (data.size() == count_bytes)
        return true;
    return false;
}

const QString ReaderWindow::parse_value(const QByteArray &data)
{
    quint8 cmd = data[0];
    switch (cmd) {
        case 0xf1: return QString("[0x%1]GetID              Получить адрес устройства (%2)")                                .arg(quint8(data[0]), 0, 16).arg(parse_value_device(data[1]));
        case 0xf2: return QString("[0x%1]GetNumFunc?        Получить количество функций	Не реализовано (%2)")               .arg(quint8(data[0]), 0, 16).arg(parse_value_device(data[1]));
        case 0xf3: return QString("[0x%1]GetFuncType?       Получить тип функции по номеру	Не реализовано (%2)")           .arg(quint8(data[0]), 0, 16).arg(parse_value_device(data[1]));
        case 0xfd: return QString("[0x%1]SoftReset?         Выполнить мягкий сброс устройства	Не реализовано (%2)")       .arg(quint8(data[0]), 0, 16).arg(parse_value_device(data[1]));
        case 0xfe: return QString("[0x%1]HardReset?         Выполнить жесткий сброс устройства (%2)")                       .arg(quint8(data[0]), 0, 16).arg(parse_value_device(data[1]));
        case 0xff: return QString("[0x%1]Nop                Пустая операция (%2)")                                          .arg(quint8(data[0]), 0, 16).arg(parse_value_device(data[1]));
        case 0x10: return QString("[0x%1]SensorRead?        Получить данные с датчика с учетом калибровки (%2)")            .arg(quint8(data[0]), 0, 16).arg(parse_value_device(data[1]));
        case 0x11: return QString("[0x%1]SensorReadRaw?     Получить данные с датчика в отсчетах АЦП (%2)")                 .arg(quint8(data[0]), 0, 16).arg(parse_value_device(data[1]));
        case 0x12: return QString("[0x%1]SensorCalRead?     Получить данные калибровки	Не реализовано (%2)")               .arg(quint8(data[0]), 0, 16).arg(parse_value_device(data[1]));
        case 0x13: return QString("[0x%1]SensorCalStore?    Сохранить данные калибровки (%2)")                              .arg(quint8(data[0]), 0, 16).arg(parse_value_device(data[1]));
        case 0x14: return QString("[0x%1]SensorNull?        Обнулить показания датчика (%2)")                               .arg(quint8(data[0]), 0, 16).arg(parse_value_device(data[1]));
        case 0x15: return QString("[0x%1]SensorReset?       Сбросить ноль (%2)")                                            .arg(quint8(data[0]), 0, 16).arg(parse_value_device(data[1]));
        case 0x20: return QString("[0x%1]StepperMove?       Переместить ШД на заданное число шагов	Не реализовано (%2)")   .arg(quint8(data[0]), 0, 16).arg(parse_value_device(data[1]));
        case 0x21: return QString("[0x%1]StepperMoveTo?     То же, абсолютный режим	Не реализовано (%2)")                   .arg(quint8(data[0]), 0, 16).arg(parse_value_device(data[1]));
        case 0x22: return QString("[0x%1]StepperPos?        Получить значение счетчика шагов ШД (%2)")                      .arg(quint8(data[0]), 0, 16).arg(parse_value_device(data[1]));
        case 0x23: return QString("[0x%1]StepperStatus?     Получить статус ШД (в т.ч. датчиков края) (%2)")                .arg(quint8(data[0]), 0, 16).arg(parse_value_device(data[1]));
        case 0x24: return QString("[0x%1]StepperSpeed?      Задать скорость вращения ШД (%2)")                              .arg(quint8(data[0]), 0, 16).arg(parse_value_device(data[1]));
        case 0x25: return QString("[0x%1]StepperStop?       Остановить ШД (%2)")                                            .arg(quint8(data[0]), 0, 16).arg(parse_value_device(data[1]));
        case 0x26: return QString("[0x%1]StepperNull?       Обнулить счетчик шагов ШД (%2)")                                .arg(quint8(data[0]), 0, 16).arg(parse_value_device(data[1]));
        case 0x30: return QString("[0x%1]ControllerSet?     Задать целевое значение регулятора (%2)")                       .arg(quint8(data[0]), 0, 16).arg(parse_value_device(data[1]));
        case 0x31: return QString("[0x%1]ControllerLock?    Активировать или деактивировать регулятор (%2)")                .arg(quint8(data[0]), 0, 16).arg(parse_value_device(data[1]));
        case 0x32: return QString("[0x%1]ControllerSetKp?   Задать пропорциональный коэффициент (%2)")                      .arg(quint8(data[0]), 0, 16).arg(parse_value_device(data[1]));
        case 0x33: return QString("[0x%1]ControllerSetKi?   Задать интегральный коэффициент (%2)")                          .arg(quint8(data[0]), 0, 16).arg(parse_value_device(data[1]));
        case 0x34: return QString("[0x%1]ControllerSetKd?   Задать дифференциальный коэффициент (%2)")                      .arg(quint8(data[0]), 0, 16).arg(parse_value_device(data[1]));
        case 0x35: return QString("[0x%1]ControllerStatus?  Получить статус регулятора (%2)")                               .arg(quint8(data[0]), 0, 16).arg(parse_value_device(data[1]));
    }
    return QString("Not found CMD");
}

QString ReaderWindow::parse_value_device(const quint8 byte)
{
    switch (byte) {
    case 0  : return QString("Регистр команды - sz=1");
    case 1	: return QString("Регистр общего назначения - sz=1");
    case 2	: return QString("Регистр калибровки - sz=4");
    case 6	: return QString("ШД Волюмометра 1 - sz=2");
    case 8	: return QString("ШД Волюмометра 2 - sz=2");
    case 10	: return QString("ШД Силовой рамы - sz=2");
    case 12	: return QString("ШД Срезной каретки - sz=2");
    case 14	: return QString("Датчик давления (Волюмометр 1) - sz=2");
    case 16	: return QString("Датчик давления (Волюмометр 2) - sz=2");
    case 18	: return QString("Датчик давления дополнительный - sz=2");
    case 22	: return QString("Датчик усилия (Силовая рама) - sz=2");
    case 24	: return QString("Датчик усилия (Срезная каретка) - sz=2");
    case 26	: return QString("Датчик деформации (Силовая рама) - sz=2");
    case 28	: return QString("Датчик деформации (Срезная каретка) - sz=2");
    case 30	: return QString("Регулятор давления (Волюмометр 1) - sz=2");
    case 32	: return QString("Регулятор давления (Волюмометр 2) - sz=2");
    case 34	: return QString("Регулятор усилия (Силовая рама) / Регулятор усилия (Срезная каретка) - sz=2");
    }
    return QString("Not found register device 0x%1").arg(byte, 0, 16);
}

void ReaderWindow::on_chckbxSaveToFile_clicked(bool checked)
{
    if (checked) {
        if (!file_out.open(QIODevice::WriteOnly | QIODevice::Text)) {
            qDebug() << file_out.fileName() << " which file_out not open";
        }
        qDebug() << file_out.fileName() << "which file_out open";
    } else {
        file_out.close();
        qDebug() << file_out.fileName() << "which file_out closed";
    }

}


void ReaderWindow::on_openFileBtn_clicked()
{
    file_name = QFileDialog::getOpenFileName(this, QDir::currentPath());
}


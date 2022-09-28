#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class ReaderWindow : public QMainWindow
{
    Q_OBJECT

    enum STATE {STATE_1, STATE_2, STATE_3} state = STATE_1;


public:
    ReaderWindow(QWidget *parent = nullptr);
    ~ReaderWindow();

public slots:
    void onSerialReadyRead();
    void onSerialBytesWritten();
    void onSerialError();

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_test();
    void on_test2();

    void on_pushButton_3_clicked();

private:

    void port_init();
    QSerialPort *port;

    Ui::MainWindow *ui;

    QString buffer;
};
#endif // MAINWINDOW_H

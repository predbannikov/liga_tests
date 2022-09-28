#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QObject>
#include <QTcpSocket>
#include <QDataStream>
#include <QTimer>

class TcpClient : public QObject
{
    Q_OBJECT
public:
    explicit TcpClient(QObject *parent = nullptr);

private slots:
    void requestNewFortune();
    void readFortune();
    void printError(QAbstractSocket::SocketError socketError);

private:

    QTcpSocket *tcpSocket = nullptr;
    QDataStream in;
    QString host;
    QString port;
};


#endif // TCPCLIENT_H

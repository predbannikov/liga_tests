#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QObject>
#include <QTcpServer>

class TcpServer : public QObject
{
    Q_OBJECT
public:
    explicit TcpServer(QObject *parent = nullptr);

private:
    void initServer();

    QTcpServer *tcpServer = nullptr;

private slots:
    void incomingConnection();

signals:

};

#endif // TCPSERVER_H

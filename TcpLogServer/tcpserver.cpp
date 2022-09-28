#include "tcpserver.h"
#include <QtNetwork>

TcpServer::TcpServer(QObject *parent)
    : QObject{parent}
{
    initServer();
    connect(tcpServer, &QTcpServer::newConnection, this, &TcpServer::incomingConnection);
}

void TcpServer::initServer()
{
    tcpServer = new QTcpServer(this);
    if (!tcpServer->listen()) {
        qDebug() << tr("Unable to start the server: %1.").arg(tcpServer->errorString());
        exit(0);
        return;
    }

    QString ipAddress;
    QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();
    for (int i = 0; i < ipAddressesList.size(); ++i) {
        if (ipAddressesList.at(i) != QHostAddress::LocalHost &&
            ipAddressesList.at(i).toIPv4Address()) {
            ipAddress = ipAddressesList.at(i).toString();
            break;
        }
    }
    if (ipAddress.isEmpty())
        ipAddress = QHostAddress(QHostAddress::LocalHost).toString();
    qDebug() << tr("The server is running on\n\nIP: %1\nport: %2\n\n"
                            "Run the Fortune Client example now.")
                         .arg(ipAddress).arg(tcpServer->serverPort());
}

void TcpServer::incomingConnection()
{
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_10);

    out << "ping";

    QTcpSocket *clientConnection = tcpServer->nextPendingConnection();
    connect(clientConnection, &QAbstractSocket::disconnected,
            clientConnection, &QObject::deleteLater);

    clientConnection->write(block);
    clientConnection->disconnectFromHost();
}

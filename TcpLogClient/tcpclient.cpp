#include <QtNetwork>
#include <QAbstractSocket>
#include "tcpclient.h"

TcpClient::TcpClient(QObject *parent)
    : QObject{parent},
      tcpSocket(new QTcpSocket(this))
{
    QString name = QHostInfo::localHostName();
    if (!name.isEmpty()) {
        QString domain = QHostInfo::localDomainName();
    }

    QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();
    // add non-localhost addresses


    in.setDevice(tcpSocket);
    in.setVersion(QDataStream::Qt_4_0);

    connect(tcpSocket, &QIODevice::readyRead, this, &TcpClient::readFortune);
    connect(tcpSocket, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error), this, &TcpClient::printError);


}

void TcpClient::requestNewFortune()
{
    tcpSocket->abort();
    tcpSocket->connectToHost(host, port.toInt());
}

void TcpClient::readFortune()
{
    in.startTransaction();

    QString nextFortune;
    in >> nextFortune;

    if (!in.commitTransaction())
        return;

}



void TcpClient::printError(QAbstractSocket::SocketError socketError)
{
    switch (socketError) {
    case QAbstractSocket::RemoteHostClosedError:
        break;
    case QAbstractSocket::HostNotFoundError:
        qDebug() << tr("The host was not found. Please check the host name and port settings.");
        break;
    case QAbstractSocket::ConnectionRefusedError:
        qDebug() << tr("The connection was refused by the peer. "
                                    "Make sure the fortune server is running, "
                                    "and check that the host name and port "
                                    "settings are correct.");
        break;
    default:
        qDebug() << tr("The following error occurred: %1.")
                                 .arg(tcpSocket->errorString());
    }

}

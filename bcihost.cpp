#include "bcihost.h"

BciHost::BciHost(QObject *parent) : QObject(parent) {

    socket = new QUdpSocket(this);
    socket->bind(QHostAddress::LocalHost, 12345);

    connect(socket, SIGNAL(readyRead()), this, SLOT(readyRead()));

    qDebug() << "BCI " << socket->state();
}


BciHost::~BciHost() {

}


void BciHost::readyRead() {

    QByteArray buffer;
    buffer.resize( socket->pendingDatagramSize() );

    QHostAddress sender;
    quint16 senderPort;

    // qint64 QUdpSocket::readDatagram(char * data, qint64 maxSize,
    //                 QHostAddress * address = 0, quint16 * port = 0)
    // Receives a datagram no larger than maxSize bytes and stores it in data.
    // The sender's host address and port is stored in *address and *port
    // (unless the pointers are 0).

    socket->readDatagram(buffer.data(), buffer.size(), &sender, &senderPort);

    //qDebug() << sender.toString() << senderPort << buffer;
    qDebug() << buffer;
}

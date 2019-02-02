#include "bcihost.h"

BciHost::BciHost(QObject *parent) : QObject(parent) {

    socket = new QUdpSocket(this);
    socket->bind(QHostAddress::LocalHost, 12345);

    connect(socket, SIGNAL(readyRead()), this, SLOT(readyRead()));

    qDebug() << "BCI " << socket->state();
}


BciHost::~BciHost() {

    delete socket;
}


void BciHost::readyRead() {

    QByteArray buffer;
    buffer.resize( static_cast<int>(socket->pendingDatagramSize()) );

    QHostAddress sender;
    quint16 senderPort;

    // qint64 QUdpSocket::readDatagram(char * data, qint64 maxSize,
    //                 QHostAddress * address = 0, quint16 * port = 0)
    // Receives a datagram no larger than maxSize bytes and stores it in data.
    // The sender's host address and port is stored in *address and *port
    // (unless the pointers are 0).

    socket->readDatagram(buffer.data(), buffer.size(), &sender, &senderPort);

    QJsonDocument json_doc = QJsonDocument::fromJson(buffer);

    if ( (json_doc.isNull()) || (!json_doc.isObject()) )
        return;

    QJsonObject json_obj = json_doc.object();

    if (json_obj.isEmpty())
        return;

    QVariantMap json_map = json_obj.toVariantMap();

    QString data_type = json_map["type"].toString();

    if (data_type == "eeg")
        receiveEeg( json_map["data"].toList() );

    if (data_type == "fft")
        receiveFft( json_map["data"].toList() );

    //qDebug() << qPrintable( json_map["type"].toString() );
}


void BciHost::receiveEeg( QVariantList channels ) {

    QString out = "";

    for( int i=0; i < channels.count(); ++i ) {

        out.append( channels[i].toString() );
        out.append( " " );
    }
    qDebug() << qPrintable( out );
}


void BciHost::receiveFft( QVariantList channels ) {

    for( int i=0; i < channels.count(); ++i ) {

        QVariantList fft = channels[i].toList();

        QString out = "";

        for( int j=0; j < 20; ++j ) {

            out.append( fft[j].toString() );
            out.append( " " );
        }
        qDebug() << qPrintable( out );
    }
}

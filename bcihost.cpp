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

    socket->readDatagram(buffer.data(), buffer.size(), &sender, &senderPort);

    QJsonDocument json_doc = QJsonDocument::fromJson(buffer);

    if ( (json_doc.isNull()) || (!json_doc.isObject()) ) return;

    QJsonObject json_obj = json_doc.object();

    if (json_obj.isEmpty()) return;

    QVariantMap json_map = json_obj.toVariantMap();

    QString data_type = json_map["type"].toString();

    if (data_type == "eeg")
        receiveEeg( json_map["data"].toList() );

    if (data_type == "fft")
        receiveFft( json_map["data"].toList() );
}


void BciHost::receiveEeg( QVariantList channels ) {

    if (channels.count() < 4) return;
    double packet[4];

    for( int i=0; i < 4; ++i ) {

        packet[i] = channels[i].toDouble();
    }
    emit eegEvent( packet );
}


void BciHost::receiveFft( QVariantList channels ) {

    if (channels.count() < 4) return;

    int selected = 2;
    QVariantList fft = channels[selected].toList();

    if (fft.count() < 125) return;
    double  packet[125];

    for( int i=0; i < 125; ++i ) {

        packet[i] = fft[i].toDouble();
    }
    emit fftEvent( packet );
}

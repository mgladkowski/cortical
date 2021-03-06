#ifndef BCIHOST_H
#define BCIHOST_H

#include <QObject>
#include <QCursor>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QUdpSocket>
#include <windows.h>


class BciHost : public QObject {

    Q_OBJECT


public:

    explicit    BciHost(QObject *parent = nullptr);
                ~BciHost();

    enum State { Initializing,
                 Connected,
                 Disconnected,
                 Failed };

    enum Mouse { Off,
                 Gaze,
                 Control };

    Mouse       mouse;


private:

    QUdpSocket  *socket;

    void        receiveEeg( QVariantList channels );
    void        receiveFft( QVariantList channels );

signals:

    void        eegEvent(double packet[4]);
    void        fftEvent(double packet[125]);

public slots:

    void        readyRead();

};

#endif // BCIHOST_H

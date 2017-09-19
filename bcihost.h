#ifndef BCIHOST_H
#define BCIHOST_H

#include <QObject>
#include <QCursor>
#include <windows.h>


class BciHost : public QObject {

    Q_OBJECT


public:

    explicit    BciHost(QObject *parent = nullptr);
                ~BciHost();

    enum State {
                Initializing,
                Connected,
                Disconnected,
                Failed
    };

    enum Mouse {
                Off,
                Gaze,
                Control
    };

    Mouse       mouse;

signals:


public slots:

};

#endif // BCIHOST_H

#ifndef EYEXHOST_H
#define EYEXHOST_H

#define WIN32_LEAN_AND_MEAN
#include <QDebug>
#include <QObject>
#include <QVector>
#include <QCursor>
#include <assert.h>
#include <windows.h>
#include "eyex/EyeX.h"


struct ActivatableRegion {

    int     id;
    RECT    bounds;
    ActivatableRegion(){}
    ActivatableRegion(int paramId, RECT paramBounds) : id(paramId), bounds(paramBounds){}
};


class EyeXHost : public QObject {

    Q_OBJECT


public:

    explicit EyeXHost(QObject *parent = nullptr);
    ~EyeXHost();

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

    Mouse   mouse;
    int     gazeX;
    int     gazeY;
    int     emaX;
    int     emaY;
    int     ema_size;
    float   ema_multiplier;

    void Init(HWND hWnd);

    bool IsFunctional();

    void SetActivatableRegions(const QVector<ActivatableRegion> &regions);

    void TriggerActivation();


private:

    HWND                _hWnd;
    TX_CONTEXTHANDLE    _context;
    TX_TICKET           _connectionStateChangedTicket;
    TX_TICKET           _queryHandlerTicket;
    TX_TICKET           _eventHandlerTicket;
    TX_STRING           _gazeInteractorId;
    TX_HANDLE           _gazeInteractorSnapshot;
    State               _state;
    QVector<ActivatableRegion> _regions;

    bool InitializeGlobalInteractorSnapshot();
    bool RegisterConnectionStateChangedHandler();
    bool RegisterQueryHandler();
    bool RegisterEventHandler();

    void OnEngineConnectionStateChanged(TX_CONNECTIONSTATE connectionState);
    void HandleQuery(TX_CONSTHANDLE hAsyncData);
    void HandleEvent(TX_CONSTHANDLE hAsyncData);

    static void TX_CALLCONVENTION OnSnapshotCommitted(TX_CONSTHANDLE hAsyncData, TX_USERPARAM param);

    void SetState(State state);


signals:

    void ConnectionStateChanged();

    void GazeEvent(int X, int Y);


public slots:

    void on_GazeEvent(int X, int Y);

};

#endif // EYEXHOST_H

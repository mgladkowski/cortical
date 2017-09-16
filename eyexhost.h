#ifndef EYEXHOST_H
#define EYEXHOST_H

#define WIN32_LEAN_AND_MEAN
#include <QDebug>
#include <QObject>
#include <QVector>
#include <assert.h>
#include <windows.h>
#include "eyex/EyeX.h"


struct ActivatableRegion {

    int id;
    RECT bounds;
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

    void Init(HWND hWnd);

    bool IsFunctional();

    void SetActivatableRegions(const QVector<ActivatableRegion> &regions);

    void TriggerActivation();


private:

    HWND        _hWnd;
    TX_CONTEXTHANDLE _context;
    TX_TICKET   _connectionStateChangedTicket;
    TX_TICKET   _queryHandlerTicket;
    TX_TICKET   _eventHandlerTicket;
    TX_STRING   _gazeInteractorId;
    TX_HANDLE   _gazeInteractorSnapshot;
    State       _state;
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

};

#endif // EYEXHOST_H

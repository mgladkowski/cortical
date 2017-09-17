#include "eyexhost.h"


EyeXHost::EyeXHost(QObject *parent) : QObject(parent),
    _hWnd(nullptr),
    _context(TX_EMPTY_HANDLE),
    _connectionStateChangedTicket(0),
    _queryHandlerTicket(0),
    _eventHandlerTicket(0),
    _gazeInteractorId((char *)"interactor-gazedata"),
    _gazeInteractorSnapshot(TX_EMPTY_HANDLE),
    _state(Initializing) {


    mouse           = Mouse::Off;
    ema_size        = 20;
    ema_multiplier  = 2.0 / (ema_size + 1);
    gazeX           = 0;
    gazeY           = 0;
    emaX            = 0;
    emaY            = 0;

    bool success = true;

    success &= txInitializeEyeX(TX_EYEXCOMPONENTOVERRIDEFLAG_NONE, nullptr, nullptr, nullptr, nullptr) == TX_RESULT_OK;
    success &= txCreateContext(&_context, TX_FALSE) == TX_RESULT_OK;
    success &= InitializeGlobalInteractorSnapshot();
    success &= RegisterConnectionStateChangedHandler();
    success &= RegisterQueryHandler();
    success &= RegisterEventHandler();

    if (!success) {
        qDebug() << "EyeX did not initialize properly";
        return;
    }

    QObject::connect(
                this, SIGNAL(GazeEvent(int, int)),
                this, SLOT(on_GazeEvent(int, int))
    );
}


EyeXHost::~EyeXHost() {

    if (_context != TX_EMPTY_HANDLE) {

        bool success = true;

        txDisableConnection(_context);
        txReleaseObject(&_gazeInteractorSnapshot);
        success &= txShutdownContext(_context, TX_CLEANUPTIMEOUT_DEFAULT, TX_FALSE) == TX_RESULT_OK;
        success &= txReleaseContext(&_context) == TX_RESULT_OK;
        success &= txUninitializeEyeX() == TX_RESULT_OK;

        if (!success) qDebug() << "EyeX did not shut down properly";
    }
}


void EyeXHost::Init(HWND hWnd) {

    _hWnd = hWnd;
    if (txEnableConnection(_context) != TX_RESULT_OK) {
        SetState(Failed);
    }
}


bool EyeXHost::IsFunctional() {

    return _state == Initializing || _state == Connected;
}


bool EyeXHost::InitializeGlobalInteractorSnapshot() {

    TX_HANDLE hInteractor = TX_EMPTY_HANDLE;
    TX_GAZEPOINTDATAPARAMS params = { TX_GAZEPOINTDATAMODE_LIGHTLYFILTERED };

    bool success;

    success = txCreateGlobalInteractorSnapshot(_context, _gazeInteractorId, &_gazeInteractorSnapshot, &hInteractor) == TX_RESULT_OK;
    success &= txCreateGazePointDataBehavior(hInteractor, &params) == TX_RESULT_OK;
    txReleaseObject(&hInteractor);

    return success;
}


bool EyeXHost::RegisterConnectionStateChangedHandler() {

    auto connectionStateChangedTrampoline = [](TX_CONNECTIONSTATE connectionState, TX_USERPARAM userParam) {
        static_cast<EyeXHost*>(userParam)->OnEngineConnectionStateChanged(connectionState);
    };
    bool success = txRegisterConnectionStateChangedHandler(_context, &_connectionStateChangedTicket, connectionStateChangedTrampoline, this) == TX_RESULT_OK;
    return success;
}


bool EyeXHost::RegisterQueryHandler() {

    auto queryHandlerTrampoline = [](TX_CONSTHANDLE hObject, TX_USERPARAM userParam) {
        static_cast<EyeXHost*>(userParam)->HandleQuery(hObject);
    };
    bool success = txRegisterQueryHandler(_context, &_queryHandlerTicket, queryHandlerTrampoline, this) == TX_RESULT_OK;
    return success;
}


bool EyeXHost::RegisterEventHandler() {

    auto eventHandlerTrampoline = [](TX_CONSTHANDLE hObject, TX_USERPARAM userParam) {
        static_cast<EyeXHost*>(userParam)->HandleEvent(hObject);
    };
    bool success = txRegisterEventHandler(_context, &_eventHandlerTicket, eventHandlerTrampoline, this) == TX_RESULT_OK;
    return success;
}


void EyeXHost::OnEngineConnectionStateChanged(TX_CONNECTIONSTATE connectionState) {

    switch (connectionState) {
    case TX_CONNECTIONSTATE_CONNECTED: {

            qDebug() << "EyeX CONNECTED";
            SetState(Connected);

            bool success = txCommitSnapshotAsync(_gazeInteractorSnapshot, OnSnapshotCommitted, this) == TX_RESULT_OK;
            if (!success) {
                qDebug() << "Gaze stream failed";
            } else {
                qDebug() << "Gaze stream started";
            }
        }
        break;

    case TX_CONNECTIONSTATE_DISCONNECTED:
        qDebug() << "EyeX DISCONNECTED";
        SetState(Disconnected);
        break;

    case TX_CONNECTIONSTATE_TRYINGTOCONNECT:
        qDebug() << "EyeX TRYINGTOCONNECT";
        SetState(Initializing);
        break;

    case TX_CONNECTIONSTATE_SERVERVERSIONTOOLOW:
        qDebug() << "EyeX SERVER_VERSION_TOO_LOW";
        SetState(Failed);
        break;

    case TX_CONNECTIONSTATE_SERVERVERSIONTOOHIGH:
        qDebug() << "EyeX SERVER_VERSION_TOO_HIGH";
        SetState(Failed);
        break;
    }
}


void EyeXHost::HandleQuery(TX_CONSTHANDLE hAsyncData) {

    TX_HANDLE hQuery(TX_EMPTY_HANDLE);
    txGetAsyncDataContent(hAsyncData, &hQuery);
    txReleaseObject(&hQuery);
}


void EyeXHost::HandleEvent(TX_CONSTHANDLE hAsyncData) {

    TX_HANDLE hEvent = TX_EMPTY_HANDLE;
    TX_HANDLE hBehavior = TX_EMPTY_HANDLE;

    txGetAsyncDataContent(hAsyncData, &hEvent);

    if (txGetEventBehavior(hEvent, &hBehavior, TX_BEHAVIORTYPE_GAZEPOINTDATA) == TX_RESULT_OK) {

        TX_GAZEPOINTDATAEVENTPARAMS e;
        if (txGetGazePointDataEventParams(hBehavior, &e) == TX_RESULT_OK) {
            int x = static_cast<int>(e.X);
            int y = static_cast<int>(e.Y);
            emit GazeEvent(x, y);
        }
        txReleaseObject(&hBehavior);
    }
    txReleaseObject(&hEvent);
}


void TX_CALLCONVENTION EyeXHost::OnSnapshotCommitted(TX_CONSTHANDLE hAsyncData, TX_USERPARAM param) {

    TX_RESULT result = TX_RESULT_UNKNOWN;
    txGetAsyncDataResultCode(hAsyncData, &result);
    assert(result == TX_RESULT_OK || result == TX_RESULT_CANCELLED);
}


void EyeXHost::SetState(State state) {

    if (_state != state) {
        _state = state;
        emit ConnectionStateChanged();
    }
}


void EyeXHost::SetActivatableRegions(const QVector<ActivatableRegion> &regions) {

    qCopy(regions.begin(), regions.end(), _regions.begin());
}


void EyeXHost::TriggerActivation() {

    TX_HANDLE command(TX_EMPTY_HANDLE);
    txCreateActionCommand(_context, &command, TX_ACTIONTYPE_ACTIVATE);
    txExecuteCommandAsync(command, NULL, NULL);
    txReleaseObject(&command);
}


void EyeXHost::on_GazeEvent(int X, int Y) {

    if (mouse == Mouse::Off || X < 1 || Y < 1) return;

    if ((GetKeyState(VK_LBUTTON) & 0x80) != 0) return;
    if ((GetKeyState(VK_RBUTTON) & 0x80) != 0) return;

    gazeX = X;
    gazeY = Y;

    emaX = ((gazeX - emaX) * ema_multiplier) + emaX;
    emaY = ((gazeY - emaY) * ema_multiplier) + emaY;

    if (mouse == Mouse::Gaze) {

        HWND desktop = GetDesktopWindow();
        HDC dc = GetDC(desktop);
        RECT rect = { emaX, emaY, emaX+1, emaY+1  };
        HBRUSH brush = CreateSolidBrush( RGB(128, 255, 255) );
        FillRect(dc, &rect, brush);

    } else if (mouse == Mouse::Control) {

        QCursor::setPos(emaX, emaY);
    }
}

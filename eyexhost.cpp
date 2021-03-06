#include "eyexhost.h"


EyeXHost::EyeXHost(QObject *parent) : QObject(parent),
    _hWnd( nullptr ),
    _context( TX_EMPTY_HANDLE ),
    _connectionStateChangedTicket( 0 ),
    _stateHandlerTicket( 0 ),
    _queryHandlerTicket( 0 ),
    _eventHandlerTicket( 0 ),
    _gazeInteractorId( (char *)"100" ),
    _gazeInteractorSnapshot( TX_EMPTY_HANDLE ),
    _state(Initializing) {


    mouse           = Mouse::Off;
    ema_size        = 22;
    ema_multiplier  = 2.0 / (ema_size + 1);
    gazeX           = 0;
    gazeY           = 0;
    emaX            = 0;
    emaY            = 0;

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

    bool success = true;

    success &= txInitializeEyeX(TX_EYEXCOMPONENTOVERRIDEFLAG_NONE, nullptr, nullptr, nullptr, nullptr) == TX_RESULT_OK;
    success &= txCreateContext(&_context, TX_FALSE) == TX_RESULT_OK;
    success &= InitializeGlobalInteractorSnapshot();
    success &= RegisterConnectionStateChangedHandler();
    success &= RegisterStateHandler();
    success &= RegisterQueryHandler();
    success &= RegisterEventHandler();

    // given a string processId representing the process id of another application:
    // _context.RegisterQueryHandler(processId, HandleQuery);

    if (!success) {

        SetState(Failed);
        qDebug() << "EyeX did not initialize properly";
        return;
    }

    if (txEnableConnection(_context) != TX_RESULT_OK) {

        SetState(Failed);
        qDebug() << "EyeX did not connect properly";
        return;
    }

    QObject::connect(
                this, SIGNAL(GazeEvent(int, int)),
                this, SLOT(OnGazeEvent(int, int))
    );
}


bool EyeXHost::IsFunctional() {

    return _state == Initializing || _state == Connected;
}


bool EyeXHost::InitializeGlobalInteractorSnapshot() {

    TX_HANDLE hInteractor = TX_EMPTY_HANDLE;
    TX_GAZEPOINTDATAPARAMS paramsGaze = { TX_GAZEPOINTDATAMODE_LIGHTLYFILTERED };
    //TX_FIXATIONDATAPARAMS paramsFixation = { TX_FIXATIONDATAMODE_SENSITIVE };

    bool success;

    success = txCreateGlobalInteractorSnapshot(_context, _gazeInteractorId, &_gazeInteractorSnapshot, &hInteractor) == TX_RESULT_OK;

    success &= txCreateGazePointDataBehavior(hInteractor, &paramsGaze) == TX_RESULT_OK;
    //success &= txCreateFixationDataBehavior(hInteractor, &paramsFixation) == TX_RESULT_OK;

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


bool EyeXHost::RegisterStateHandler() {

    auto stateChangedTrampoline = [](TX_CONSTHANDLE hAsyncData, TX_USERPARAM userParam) {
        static_cast<EyeXHost*>(userParam)->OnEngineStateChanged(hAsyncData);
    };
    bool success = txRegisterStateChangedHandler(_context, &_stateHandlerTicket, TX_STATEPATH_USERPRESENCE, stateChangedTrampoline, this) == TX_RESULT_OK;
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
                qDebug() << "EyeX Gaze stream failed";
            } else {
                qDebug() << "EyeX Gaze stream started";
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


void EyeXHost::OnEngineStateChanged(TX_CONSTHANDLE hAsyncData) {

    TX_RESULT result = TX_RESULT_UNKNOWN;
    TX_HANDLE hStateBag = TX_EMPTY_HANDLE;

    if (txGetAsyncDataResultCode(hAsyncData, &result) == TX_RESULT_OK &&
        txGetAsyncDataContent(hAsyncData, &hStateBag) == TX_RESULT_OK) {

        bool success = (txGetStateValueAsInteger(hStateBag, TX_STATEPATH_USERPRESENCE, &_presenceData) == TX_RESULT_OK);
        if (success) {
            emit UserPresenceChanged( (_presenceData == TX_USERPRESENCE_PRESENT) );
        }
        txReleaseObject(&hStateBag);
    }
}


void EyeXHost::HandleQuery(TX_CONSTHANDLE hAsyncData) {

    TX_HANDLE hQuery(TX_EMPTY_HANDLE);
    txGetAsyncDataContent(hAsyncData, &hQuery);

    const int bufferSize = 20;
    TX_CHAR windowId[bufferSize];
    TX_CHAR regionId[bufferSize];

    sprintf(windowId, "%d", reinterpret_cast<std::uintptr_t>(_hWnd));

    // read the query bounds from the query, the area on the screen that the query concerns

    TX_HANDLE hBounds(TX_EMPTY_HANDLE);
    txGetQueryBounds(hQuery, &hBounds);
    TX_REAL pX, pY, pWidth, pHeight;
    txGetRectangularBoundsData(hBounds, &pX, &pY, &pWidth, &pHeight);
    txReleaseObject(&hBounds);
    Gdiplus::Rect queryBounds(static_cast<int>(pX),
                              static_cast<int>(pY),
                              static_cast<int>(pWidth),
                              static_cast<int>(pHeight));

    // create a new snapshot with the same window id and bounds as the query

    TX_HANDLE hSnapshot(TX_EMPTY_HANDLE);
    txCreateSnapshotForQuery(hQuery, &hSnapshot);

    if ( QueryIsForWindowId(hQuery, windowId) ) {

        // define options for our activatable regions: yes, we want tentative focus events

        TX_ACTIVATABLEPARAMS params = { TX_TRUE };

        // iterate through all regions and create interactors for those that overlap with the query bounds

        for (auto region : _regions) {

            Gdiplus::Rect regionBounds(
                        static_cast<int>(region.bounds.left),
                        static_cast<int>(region.bounds.top),
                        static_cast<int>((region.bounds.right - region.bounds.left)),
                        static_cast<int>((region.bounds.bottom - region.bounds.top)));

            if (queryBounds.IntersectsWith(regionBounds)) {

                TX_HANDLE hInteractor(TX_EMPTY_HANDLE);

                sprintf(regionId, "%d", region.id);

                TX_RECT bounds;
                bounds.X = region.bounds.left;
                bounds.Y = region.bounds.top;
                bounds.Width = region.bounds.right - region.bounds.left;
                bounds.Height = region.bounds.bottom - region.bounds.top;

                txCreateRectangularInteractor(hSnapshot, &hInteractor, regionId, &bounds, TX_LITERAL_ROOTID, windowId);
                txCreateActivatableBehavior(hInteractor, &params);
                txReleaseObject(&hInteractor);
            }
        }
    }

    txCommitSnapshotAsync(hSnapshot, OnSnapshotCommitted, nullptr);
    txReleaseObject(&hSnapshot);
    txReleaseObject(&hQuery);
}


void EyeXHost::HandleEvent(TX_CONSTHANDLE hAsyncData) {

    TX_HANDLE hEvent        = TX_EMPTY_HANDLE;
    TX_HANDLE hActivatable  = TX_EMPTY_HANDLE;
    TX_HANDLE hBehavior     = TX_EMPTY_HANDLE;

    txGetAsyncDataContent(hAsyncData, &hEvent);

    if (txGetEventBehavior(hEvent, &hActivatable, TX_BEHAVIORTYPE_ACTIVATABLE) == TX_RESULT_OK) {

        int         interactorId = 0;
        const int   bufferSize = 20;
        TX_CHAR     stringBuffer[bufferSize];
        TX_SIZE     idLength(bufferSize);

        if (txGetEventInteractorId(hEvent, stringBuffer, &idLength) == TX_RESULT_OK) {

            interactorId = atoi(stringBuffer);
        }

        TX_ACTIVATABLEEVENTTYPE eventType;

        if (txGetActivatableEventType(hActivatable, &eventType) == TX_RESULT_OK) {

            if (eventType == TX_ACTIVATABLEEVENTTYPE_ACTIVATED) {

                OnActivated(hActivatable, interactorId);

            } else if (eventType == TX_ACTIVATABLEEVENTTYPE_ACTIVATIONFOCUSCHANGED) {

                OnActivationFocusChanged(hActivatable, interactorId);
            }
        }

        txReleaseObject(&hActivatable);

    }
    if (txGetEventBehavior(hEvent, &hBehavior, TX_BEHAVIORTYPE_GAZEPOINTDATA) == TX_RESULT_OK) {

        TX_GAZEPOINTDATAEVENTPARAMS e;
        if (txGetGazePointDataEventParams(hBehavior, &e) == TX_RESULT_OK) {

            int x = static_cast<int>(e.X);
            int y = static_cast<int>(e.Y);
            emit GazeEvent(x, y);
        }

        txReleaseObject(&hBehavior);

    }
    if (txGetEventBehavior(hEvent, &hBehavior, TX_BEHAVIORTYPE_FIXATIONDATA) == TX_RESULT_OK) {

        TX_FIXATIONDATAEVENTPARAMS eventParams;
        TX_FIXATIONDATAEVENTTYPE eventType;

        if (txGetFixationDataEventParams(hBehavior, &eventParams) == TX_RESULT_OK) {

            eventType = eventParams.EventType;

            if (eventType == TX_FIXATIONDATAEVENTTYPE_DATA) {
            } else if (eventType == TX_FIXATIONDATAEVENTTYPE_END) {
                //qDebug() << qPrintable("Fixation end ") << eventParams.X << eventParams.Y;
            } else if (eventType == TX_FIXATIONDATAEVENTTYPE_BEGIN) {
                //qDebug() << qPrintable("Fixation begin ") << eventParams.X << eventParams.Y;
            }

        } else {

            qDebug() << qPrintable("Failed to interpret fixation data event packet");
        }

        txReleaseObject(&hBehavior);
    }

    txReleaseObject(&hEvent);

}


void TX_CALLCONVENTION EyeXHost::OnSnapshotCommitted(TX_CONSTHANDLE hAsyncData, TX_USERPARAM param) {

    Q_UNUSED(param);
    TX_RESULT result = TX_RESULT_UNKNOWN;
    txGetAsyncDataResultCode(hAsyncData, &result);
    assert(result == TX_RESULT_OK || result == TX_RESULT_CANCELLED);
}


bool EyeXHost::QueryIsForWindowId(TX_HANDLE hQuery, const TX_CHAR* windowId) {

    const int bufferSize = 20;
    TX_CHAR buffer[bufferSize];
    TX_SIZE count;

    if (TX_RESULT_OK == txGetQueryWindowIdCount(hQuery, &count)) {

        for (int i = 0; i < count; i++) {

            TX_SIZE size = bufferSize;
            if (TX_RESULT_OK == txGetQueryWindowId(hQuery, i, buffer, &size)) {

                if (0 == strcmp(windowId, buffer)) return true;
            }
        }
    }
    return false;
}


void EyeXHost::SetState(State state) {

    if (_state != state) {
        _state = state;
        emit ConnectionStateChanged();
    }
}


void EyeXHost::SetActivatableRegions(const std::vector<ActivatableRegion> & regions) {

    _regions.assign(regions.begin(), regions.end());
}


void EyeXHost::TriggerActivation() {

    TX_HANDLE command(TX_EMPTY_HANDLE);
    txCreateActionCommand(_context, &command, TX_ACTIONTYPE_ACTIVATE);
    txExecuteCommandAsync(command, nullptr, nullptr);
    txReleaseObject(&command);
}


void EyeXHost::TriggerActivationModeOn()
{
    TX_HANDLE command(TX_EMPTY_HANDLE);
    txCreateActionCommand(_context, &command, TX_ACTIONTYPE_ACTIVATIONMODEON);
    txExecuteCommandAsync(command, nullptr, nullptr);
    txReleaseObject(&command);
}


void EyeXHost::OnGazeEvent(int X, int Y) {

    if (mouse == Mouse::Off || X < 1 || Y < 1) return;

    if ((GetKeyState(VK_LBUTTON) & 0x80) != 0) return;
    if ((GetKeyState(VK_RBUTTON) & 0x80) != 0) return;

    gazeX = X;
    gazeY = Y;

    emaX = static_cast<int>( ((gazeX - emaX) * ema_multiplier) + emaX );
    emaY = static_cast<int>( ((gazeY - emaY) * ema_multiplier) + emaY );

    if (mouse == Mouse::Gaze) {

//        HWND desktop = GetDesktopWindow();
//        HDC dc = GetDC(desktop);
//        RECT rect = { emaX, emaY, emaX+1, emaY+1  };
//        HBRUSH brush = CreateSolidBrush( RGB(128, 255, 255) );
//        FillRect(dc, &rect, brush);

    } else if (mouse == Mouse::Control) {

        INPUT im;
        im.type = INPUT_MOUSE;
        im.mi.time = 0;
        im.mi.mouseData = 0;
        im.mi.dx = emaX * (65536 / GetSystemMetrics(SM_CXSCREEN));
        im.mi.dy = emaY * (65536 / GetSystemMetrics(SM_CYSCREEN));
        im.mi.dwFlags = MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE;
        SendInput(1, &im, sizeof(INPUT));
    }
}


void EyeXHost::OnFixationDataEvent() {

}


void EyeXHost::OnActivationFocusChanged(TX_HANDLE hBehavior, int interactorId) {

    TX_ACTIVATIONFOCUSCHANGEDEVENTPARAMS eventData;

    if (txGetActivationFocusChangedEventParams(hBehavior, &eventData) == TX_RESULT_OK) {

        if (eventData.HasTentativeActivationFocus) {

            emit ActivationFocusEvent( interactorId );

        } else {

            emit ActivationFocusEvent( -1 );
        }
    }
}


void EyeXHost::OnActivated(TX_HANDLE hBehavior, int interactorId) {

    Q_UNUSED(hBehavior);
    emit ActivationEvent( interactorId );
}

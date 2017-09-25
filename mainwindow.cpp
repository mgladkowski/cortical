#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {


    // frameless always on top

    Qt::WindowFlags flags = this->windowFlags();
    setWindowFlags(flags | Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint);
    setParent(0);
    setAttribute(Qt::WA_NoSystemBackground, true);
    setAttribute(Qt::WA_TranslucentBackground, true);

    ui->setupUi(this);

    InitializeUi();

    eyes.Init( (HWND)this->winId() );

    QObject::connect(
                &eyes, SIGNAL(UserPresenceChanged(bool)),
                this, SLOT(on_UserPresenceChanged(bool))
    );

    QObject::connect(
                &systemTimer, SIGNAL(timeout()),
                this, SLOT(on_SystemTimer())
    );
    systemTimer.start( 1000 );
}


MainWindow::~MainWindow() {

    delete ui;
}


void MainWindow::InitializeUi() {

    // icons

    QFontDatabase::addApplicationFont(":/Glyphicons-Regular.otf");
    iconFont.setFamily("GLYPHICONS");
    iconFont.setPixelSize(24);

    // sets position of main window objects

    this->showFullScreen();

    QRect   desktopRect = QApplication::desktop()->availableGeometry(this);
    QPoint  center      = desktopRect.center();

    ui->frameScreen->move( 0,0 );
    ui->frameScreen->resize( desktopRect.width(), desktopRect.height() );
    int main_X = static_cast<int>( desktopRect.left() + (desktopRect.width() * 0.65) - 400 );
    int main_Y = static_cast<int>( desktopRect.bottom() - ui->widgetMain->height() );
    ui->widgetMain->move( main_X, main_Y) ;

    // connect events for each designer control

    Interactor::Params mainStyle    = GetPresetInteractor(STYLE_MAIN);
    Interactor::Params menuStyle    = GetPresetInteractor(STYLE_MENU);
    Interactor::Params buttonStyle  = GetPresetInteractor(STYLE_BUTTON);

    QList<EyeButton*> list = this->findChildren<EyeButton *>();
    foreach(EyeButton *b, list) {

        if (b->isInteractor == false) {

            QObject::connect(
                        &eyes, SIGNAL(ActivationEvent(int)),
                        b, SLOT(on_ActivationEvent(int))
            );
            QObject::connect(
                        &eyes, SIGNAL(ActivationFocusEvent(int)),
                        b, SLOT(on_ActivationFocusEvent(int))
            );
            QObject::connect(
                        b, SIGNAL(ActivationEvent(int)),
                        this, SLOT(on_ActivationEvent(int))
            );
            b->setFont(iconFont);
            b->setStyle( buttonStyle );
        }
    }

    ui->buttonMouse->setText( glyphicons_mouse );
    ui->buttonMain->setText( glyphicons_chevron_up );
    ui->buttonOp->setText( glyphicons_wrench );
    ui->buttonEye->setText( glyphicons_eye_open );
    ui->buttonBci->setText( glyphicons_cardio );

    ui->buttonMouse->setStyle( mainStyle );
    ui->buttonMain->setStyle( mainStyle );
    ui->buttonOp->setStyle( menuStyle );
    ui->buttonEye->setStyle( menuStyle );
    ui->buttonBci->setStyle( menuStyle );

    // initial state

    ui->frameMenu->hide();
    ui->frameOp->hide();
    ui->frameEye->hide();
    ui->frameBci->hide();
    ui->frameSlider->move(MENU_START, ui->frameSlider->y());

    UpdateActivatableRegions();
}


/* Gets the bounds of a button in screen coordinates
 */
RECT MainWindow::GetScreenBounds( EyeButton * button ) {

    HWND hButton = (HWND)button->winId();
    QSize size = button->size();
    POINT point = { 0, 0 };
    ClientToScreen(hButton, &point);

    RECT bounds;
    bounds.left = point.x;
    bounds.top = point.y;
    bounds.right = bounds.left + size.width();
    bounds.bottom = bounds.top + size.height();

    return bounds;
}


/* Reports the buttons as activatable regions to the EyeX host
 */
void MainWindow::UpdateActivatableRegions() {

    std::vector<ActivatableRegion> regions;

    QList<EyeButton*> list = this->findChildren<EyeButton *>();

    foreach(EyeButton *b, list) {

        if (b->isVisible()) {
            regions.push_back(ActivatableRegion(b->winId(), GetScreenBounds(b)));
        }
    }

    eyes.SetActivatableRegions(regions);
}


void MainWindow::ClearInteractorProfile() {

    QList<EyeButton*> list = this->findChildren<EyeButton *>();
    foreach(EyeButton *b, list) {

        if (b->isInteractor == true) {
            delete b;
        }
    }

    currentProfile = 0;

    UpdateActivatableRegions();
}


void MainWindow::AddInteractor( Interactor data ) {

    EyeButton *button = new EyeButton(ui->frameScreen);

    button->setProperties( data );

    QObject::connect(
                button, SIGNAL(ActivationEvent(int)),
                this, SLOT(on_InteractorActivated())
    );
    QObject::connect(
                &eyes, SIGNAL(ActivationEvent(int)),
                button, SLOT(on_ActivationEvent(int))
    );
    QObject::connect(
                &eyes, SIGNAL(ActivationFocusEvent(int)),
                button, SLOT(on_ActivationFocusEvent(int))
    );

    button->show();
}


void MainWindow::SetInteractorProfile( int profileId = ITP_NONE ) {

    if (currentProfile == profileId) return;

    ClearInteractorProfile();
    currentProfile = profileId;

    switch (profileId) {
    case ITP_BROWSER:
        AddInteractor(Interactor(  860,   50,  200,  50, ITK_PAGEUP,
                                   glyphicons_chevron_up,
                                   GetPresetInteractor(STYLE_INTERACTOR) ));

        AddInteractor(Interactor(  860, 1000,  200,  70, ITK_PAGEDN,
                                   glyphicons_chevron_down,
                                   GetPresetInteractor(STYLE_INTERACTOR) ));

        AddInteractor(Interactor( 1600,  500,   70,  70, ITK_ALT_LEFT,
                                  glyphicons_arrow_left,
                                  GetPresetInteractor(STYLE_INTERACTOR) ));

        AddInteractor(Interactor( 1600,  700,   70,  70, ITK_CTRL_T,
                                  glyphicons_more_windows,
                                  GetPresetInteractor(STYLE_INTERACTOR) ));

        AddInteractor(Interactor( 1600,  900,   70,  70, ITK_CTRL_T_TAB_F4,
                                  glyphicons_remove,
                                  GetPresetInteractor(STYLE_DANGER) ));
        break;

    case ITP_BROWSER_FS:
        AddInteractor(Interactor(  860,  700,  70,  70, ITK_SPACE,
                                   glyphicons_pause,
                                   GetPresetInteractor(STYLE_INTERACTOR) ));

        AddInteractor(Interactor(  1060,  700,  70,  70, ITK_ESC,
                                   glyphicons_fit_frame_to_image,
                                   GetPresetInteractor(STYLE_INTERACTOR) ));
        break;

    case ITP_VLC:
        AddInteractor(Interactor(  660,  700,  70,  70, ITK_P,
                                   glyphicons_step_backward,
                                   GetPresetInteractor(STYLE_INTERACTOR) ));

        AddInteractor(Interactor(  860,  700,  70,  70, ITK_SPACE,
                                   glyphicons_pause,
                                   GetPresetInteractor(STYLE_INTERACTOR) ));

        AddInteractor(Interactor(  1060,  700,  70,  70, ITK_N,
                                   glyphicons_step_forward,
                                   GetPresetInteractor(STYLE_INTERACTOR) ));

        AddInteractor(Interactor(  1260,  700,  70,  70, ITK_F,
                                   glyphicons_fit_image_to_frame,
                                   GetPresetInteractor(STYLE_INTERACTOR) ));
        break;

    case ITP_VLC_FS:
        AddInteractor(Interactor(  660,  700,  70,  70, ITK_P,
                                   glyphicons_step_backward,
                                   GetPresetInteractor(STYLE_INTERACTOR) ));

        AddInteractor(Interactor(  860,  700,  70,  70, ITK_SPACE,
                                   glyphicons_pause,
                                   GetPresetInteractor(STYLE_INTERACTOR) ));

        AddInteractor(Interactor(  1060,  700,  70,  70, ITK_N,
                                   glyphicons_step_forward,
                                   GetPresetInteractor(STYLE_INTERACTOR) ));

        AddInteractor(Interactor(  1260,  700,  70,  70, ITK_ESC,
                                   glyphicons_fit_frame_to_image,
                                   GetPresetInteractor(STYLE_INTERACTOR) ));
        break;

    case ITP_DEV:
        AddInteractor(Interactor(  860,   60,  200,  50, ITK_PAGEUP,
                                   glyphicons_chevron_up,
                                   GetPresetInteractor(STYLE_INTERACTOR) ));

        AddInteractor(Interactor(  860, 1000,  200,  50, ITK_PAGEDN,
                                   glyphicons_chevron_down,
                                   GetPresetInteractor(STYLE_INTERACTOR) ));
        break;

    case ITP_NONE:
    default:
        break;
    }

    UpdateActivatableRegions();
}


Interactor::Params MainWindow::GetPresetInteractor( int styleId = 0 ) {

    Interactor::Params i;

    i.isInteractor          = false;
    i.showHover             = true;
    i.showEyeHover          = true;
    i.showProgress          = true;
    i.suppressEyes          = false;
    i.clickOnActivation     = true;
    i.suspendOnActivation   = true;
    i.toggleActivation      = false;
    i.msecActivate          = 1200;
    i.msecRecovery          = 500;
    i.styleDefault          = "background:rgba(0,0,0,0.6); border:1px solid rgba(0,0,0,0.8); color:rgba(30,136,229,1);";
    i.styleFixate           = "background:qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop: 0 rgba(41,182,246,0.2), stop: %1 rgba(0,0,0,0.6) );border:3px solid rgba(41,182,246,0.8); color:rgba(41,182,246,1);";
    i.styleHover            = "background:rgba(41,182,246,0.2); border:3px solid rgba(41,182,246,0.6); color:rgba(41,182,246,1);";
    i.styleActive           = "background:rgba(41,182,246,0.2); border:3px solid rgba(41,182,246,0.6); color:rgba(41,182,246,1);";

    switch (styleId) {
    case STYLE_MAIN:
        i.styleDefault          = "background:rgba(0,0,0,0.01); border:0px solid rgba(0,0,0,0); color:rgba(30,136,229,0.5);";
        i.styleFixate           = "background:qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop: 0 rgba(41,182,246,0.2), stop: %1 rgba(0,0,0,0.01) );border:0px solid rgba(41,182,246,0.6); color:rgba(41,182,246,1);";
        break;

    case STYLE_MENU:

        i.suspendOnActivation   = true;
        i.toggleActivation      = false;
        i.showProgress          = true;
        i.msecActivate          = 400;
        i.msecRecovery          = 400;
        break;

    case STYLE_INTERACTOR:

        i.isInteractor          = true;
        i.showHover             = false;
        i.styleDefault          = "background:rgba(0,0,0,0.01); border:0px solid rgba(0,0,0,0); color:rgba(30,136,229,0.5);";
        i.styleFixate           = "background:qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop: 0 rgba(41,182,246,0.2), stop: %1 rgba(0,0,0,0.01) );border:0px solid rgba(41,182,246,0.6); color:rgba(41,182,246,1);";
        break;

    case STYLE_INVISIBLE:

        i.isInteractor          = true;
        i.showHover             = false;
        i.showEyeHover          = false;
        i.showProgress          = false;
        i.toggleActivation      = false;
        i.styleDefault          = "background:rgba(0,0,0,0.01); border:0px solid rgba(0,0,0,0); color:rgba(30,136,229,0.5);";
        i.styleFixate           = "background:qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop: 0 rgba(41,182,246,0.2), stop: %1 rgba(0,0,0,0.01) );border:0px solid rgba(41,182,246,0.6); color:rgba(41,182,246,1);";
        break;

    case STYLE_SUCCESS:

        i.isInteractor          = true;
        i.showHover             = false;
        i.styleDefault          = "background:rgba(0,0,0,0.01); border:0px solid rgba(0,0,0,0); color:rgba(30,136,229,0.5);";
        i.styleFixate           = "background:qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop: 0 rgba(41,182,246,0.2), stop: %1 rgba(0,0,0,0.01) );border:0px solid rgba(41,182,246,0.6); color:rgba(41,182,246,1);";
        break;

    case STYLE_WARNING:

        i.isInteractor          = true;
        i.showHover             = false;
        i.msecActivate          = 2500;
        i.msecRecovery          = 500;
        i.styleDefault          = "background:rgba(0,0,0,0.01); border:0px solid rgba(0,0,0,0); color:rgba(30,136,229,0.5);";
        i.styleFixate           = "background:qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop: 0 rgba(41,182,246,0.2), stop: %1 rgba(0,0,0,0.01) );border:0px solid rgba(41,182,246,0.6); color:rgba(41,182,246,1);";
        break;

    case STYLE_DANGER:

        i.isInteractor          = true;
        i.showHover             = false;
        i.msecActivate          = 2500;
        i.msecRecovery          = 500;
        i.styleDefault          = "background:rgba(0,0,0,0.01); border:1px solid rgba(0,0,0,0.01); color:rgba(255,0,0,0.5);";
        i.styleFixate           = "background:qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop: 0 rgba(255,0,0,0.2), stop: %1 rgba(0,0,0,0.2) );border:3px solid rgba(255,0,0,0.6); color:rgba(255,0,0,1);";
        i.styleHover            = "background:rgba(255,0,0,0.2); border:3px solid rgba(255,0,0,0.6); color:rgba(255,0,0,1);";
        i.styleActive           = "background:rgba(255,0,0,0.2); border:3px solid rgba(255,0,0,0.6); color:rgba(255,0,0,1);";
        break;

    case STYLE_BUTTON:
    default:
        break;
    }
    return i;
}


QString MainWindow::GetProcessName( HWND window ) {

    DWORD   processId;

    GetWindowThreadProcessId(window , &processId);

    TCHAR buffer[MAX_PATH] = TEXT("unknown");

    HANDLE hProcess = OpenProcess( PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processId );

    if (NULL != hProcess ) {

        GetModuleFileNameEx( hProcess, NULL, buffer, MAX_PATH );
    }

    QString result = QString::fromStdWString( buffer );

    CloseHandle( hProcess );

    return result;

    // this is how you would invoke an external DLL function, in this case lock the workstation
    //QProcess::execute("rundll32", QStringList("USER32.DLL,LockWorkStation"));
}


bool MainWindow::isProcessFullscreen( HWND window ) {

    RECT a, b;
    GetWindowRect(window, &a);
    GetWindowRect(GetDesktopWindow(), &b);
    return (a.left   == b.left  &&
            a.top    == b.top   &&
            a.right  == b.right &&
            a.bottom == b.bottom);
}


void MainWindow::UpdateFocusedProcess() {

    if (focusedExecutable == "C:\\Windows\\explorer.exe") {

        SetInteractorProfile( ITP_EXPLORER );

    } else if (focusedExecutable == "C:\\Program Files (x86)\\Mozilla Firefox\\firefox.exe") {

        SetInteractorProfile( focusedIsFullscreen
                              ? ITP_BROWSER_FS
                              : ITP_BROWSER );

    } else if (focusedExecutable == "C:\\Program Files (x86)\\Google\\Chrome\\Application\\chrome.exe") {

        SetInteractorProfile( focusedIsFullscreen
                              ? ITP_BROWSER_FS
                              : ITP_BROWSER );

    } else if (focusedExecutable == "C:\\Qt\\Tools\\QtCreator\\bin\\qtcreator.exe") {

        SetInteractorProfile( ITP_DEV );

    } else if (focusedExecutable == "C:\\Program Files (x86)\\VideoLAN\\VLC\\vlc.exe") {

        SetInteractorProfile( focusedIsFullscreen
                              ? ITP_VLC_FS
                              : ITP_VLC );
    } else {

        SetInteractorProfile( ITP_NONE );
    }
}


void MainWindow::UpdateStartMenu() {

    QString strClass = "Shell_TrayWnd";
    HWND hShell = FindWindow( (LPCWSTR)(const wchar_t*)strClass.utf16(), NULL );

    strClass = "Start";
    HWND hStart = FindWindowEx(hShell, NULL, (LPCWSTR)(const wchar_t*)strClass.utf16(), NULL );

    strClass = "ReBarWindow32";
    HWND h1 = FindWindowEx(hShell, NULL, (LPCWSTR)(const wchar_t*)strClass.utf16(), NULL );

    strClass = "MSTaskSwWClass";
    HWND h2 = FindWindowEx(h1, NULL, (LPCWSTR)(const wchar_t*)strClass.utf16(), NULL );

    strClass = "MSTaskListWClass";
    HWND hTask = FindWindowEx(h2, NULL, (LPCWSTR)(const wchar_t*)strClass.utf16(), NULL );
    qDebug() << hTask;

//    int count = SendMessage( hStart, TB_BUTTONCOUNT, 0, 0 );
//    qDebug() << count;
    //BOOL foundChild = EnumChildWindows( hTask, (WNDENUMPROC)EnumChildProc, 0);
}


BOOL CALLBACK EnumChildProc(HWND hwnd, LPARAM lParam) {

     qDebug() << qPrintable("hwnd_Child = ") << hwnd;
     return true;
}


void TaskBarTest() {

    APPBARDATA abd = { sizeof(abd) };
    UINT uState = (UINT)SHAppBarMessage(ABM_GETSTATE, &abd);
    qDebug() << QString((uState & ABS_ALWAYSONTOP) ? "always on top" : "not always on top");
    qDebug() << QString((uState & ABS_AUTOHIDE) ? "auto hide" : "not auto hide");
}


void MainWindow::on_AnimationFinished() {

    UpdateActivatableRegions();
}


void MainWindow::on_ActivationEvent( int interactorId ) {

}


void MainWindow::on_FadeAllFinished() {

    ui->frameScreen->setGraphicsEffect(NULL);
    UpdateActivatableRegions();
}


void MainWindow::on_FadeMenuFinished() {

    ui->frameMenu->hide();
    ui->frameOp->hide();
    ui->frameEye->hide();
    ui->frameBci->hide();

    ui->frameMenu->setGraphicsEffect(NULL);

    UpdateActivatableRegions();
}


void MainWindow::on_FadeOpsFinished() {

    ui->frameOp->hide();
    ui->frameOp->setGraphicsEffect(NULL);
    UpdateActivatableRegions();
}


void MainWindow::on_FadeEyeFinished() {

    ui->frameEye->hide();
    ui->frameEye->setGraphicsEffect(NULL);
    UpdateActivatableRegions();
}


void MainWindow::on_FadeBciFinished() {

    ui->frameBci->hide();
    ui->frameBci->setGraphicsEffect(NULL);
    UpdateActivatableRegions();
}


void MainWindow::on_SystemTimer() {

    HWND phWnd = GetForegroundWindow();
    bool pFs = isProcessFullscreen( phWnd );

    if ((focusedWindow == phWnd) && (focusedIsFullscreen == pFs)) return;

    focusedWindow = phWnd;
    focusedIsFullscreen = pFs;
    focusedExecutable = GetProcessName( phWnd );

    UpdateFocusedProcess();
}


void MainWindow::on_InteractorActivated() {

    EyeButton * sender = qobject_cast<EyeButton*>( QObject::sender() );
    if (!sender) return;
    QString senderName = sender->objectName();

    INPUT ip;
    ip.type = INPUT_KEYBOARD;
    ip.ki.wScan = 0;
    ip.ki.time = 0;
    ip.ki.dwExtraInfo = 0;

    if (senderName == ITK_ALT_LEFT) {

        // send ALT + LEFT

        int key_count = 4;
        INPUT * input = new INPUT[ key_count ];
        for (int i = 0; i < key_count; i++) {
            input[i].ki.dwFlags = 0;
            input[i].type = INPUT_KEYBOARD;
            input[i].ki.time = 0;
        }
        input[0].ki.wVk = VK_MENU;
        input[0].ki.wScan = MapVirtualKey(VK_MENU, MAPVK_VK_TO_VSC);
        input[1].ki.wVk = VK_LEFT;
        input[1].ki.wScan = MapVirtualKey(VK_LEFT, MAPVK_VK_TO_VSC);
        input[2].ki.dwFlags = KEYEVENTF_KEYUP;
        input[2].ki.wVk = input[0].ki.wVk;
        input[2].ki.wScan = input[0].ki.wScan;
        input[3].ki.dwFlags = KEYEVENTF_KEYUP;
        input[3].ki.wVk = input[1].ki.wVk;
        input[3].ki.wScan = input[1].ki.wScan;
        SendInput(key_count, (LPINPUT)input, sizeof(INPUT));

    }
    if (senderName == ITK_PAGEUP) {

        // send PAGEUP

        ip.ki.wVk = VK_PRIOR;
        ip.ki.dwFlags = 0;
        SendInput(1, &ip, sizeof(INPUT));
        ip.ki.dwFlags = KEYEVENTF_KEYUP;
        SendInput(1, &ip, sizeof(INPUT));

    }
    if (senderName == ITK_PAGEDN) {

        // send PAGEDOWN

        ip.ki.wVk = VK_NEXT;
        ip.ki.dwFlags = 0;
        SendInput(1, &ip, sizeof(INPUT));
        ip.ki.dwFlags = KEYEVENTF_KEYUP;
        SendInput(1, &ip, sizeof(INPUT));

    }
    if (senderName == ITK_CTRL_T) {

        // send CTRL + (T, TAB, F4)

        int key_count = 8;
        INPUT * input = new INPUT[ key_count ];
        for (int i = 0; i < key_count; i++) {
            input[i].ki.dwFlags = 0;
            input[i].type = INPUT_KEYBOARD;
            input[i].ki.time = 0;
        }
        input[0].ki.wVk = VK_CONTROL;
        input[0].ki.wScan = MapVirtualKey(VK_CONTROL, MAPVK_VK_TO_VSC);

        input[1].ki.wVk = 0x54;
        input[1].ki.wScan = MapVirtualKey(0x54, MAPVK_VK_TO_VSC);
        input[2].ki.dwFlags = KEYEVENTF_KEYUP;
        input[2].ki.wVk = input[1].ki.wVk;
        input[2].ki.wScan = input[1].ki.wScan;

        input[3].ki.wVk = VK_TAB;
        input[3].ki.wScan = MapVirtualKey(VK_TAB, MAPVK_VK_TO_VSC);
        input[4].ki.dwFlags = KEYEVENTF_KEYUP;
        input[4].ki.wVk = input[3].ki.wVk;
        input[4].ki.wScan = input[3].ki.wScan;

        input[5].ki.wVk = VK_F4;
        input[5].ki.wScan = MapVirtualKey(VK_F4, MAPVK_VK_TO_VSC);
        input[6].ki.dwFlags = KEYEVENTF_KEYUP;
        input[6].ki.wVk = input[5].ki.wVk;
        input[6].ki.wScan = input[5].ki.wScan;

        input[7].ki.dwFlags = KEYEVENTF_KEYUP;
        input[7].ki.wVk = input[0].ki.wVk;
        input[7].ki.wScan = input[0].ki.wScan;
        SendInput(key_count, (LPINPUT)input, sizeof(INPUT));

    }
    if (senderName == ITK_CTRL_T_TAB_F4) {

        // send CTRL + F4

        int key_count = 4;
        INPUT * input = new INPUT[ key_count ];
        for (int i = 0; i < key_count; i++) {
            input[i].ki.dwFlags = 0;
            input[i].type = INPUT_KEYBOARD;
            input[i].ki.time = 0;
        }
        input[0].ki.wVk = VK_CONTROL;
        input[0].ki.wScan = MapVirtualKey(VK_CONTROL, MAPVK_VK_TO_VSC);
        input[1].ki.wVk = VK_F4;
        input[1].ki.wScan = MapVirtualKey(VK_F4, MAPVK_VK_TO_VSC);
        input[2].ki.dwFlags = KEYEVENTF_KEYUP;
        input[2].ki.wVk = input[0].ki.wVk;
        input[2].ki.wScan = input[0].ki.wScan;
        input[3].ki.dwFlags = KEYEVENTF_KEYUP;
        input[3].ki.wVk = input[1].ki.wVk;
        input[3].ki.wScan = input[1].ki.wScan;
        SendInput(key_count, (LPINPUT)input, sizeof(INPUT));
    }
    if (senderName == ITK_ESC) {

        // send ESC

        ip.ki.wVk = VK_ESCAPE;
        ip.ki.dwFlags = 0;
        SendInput(1, &ip, sizeof(INPUT));
        ip.ki.dwFlags = KEYEVENTF_KEYUP;
        SendInput(1, &ip, sizeof(INPUT));

    }
    if (senderName == ITK_SPACE) {

        // send SPACE

        ip.ki.wVk = VK_SPACE;
        ip.ki.dwFlags = 0;
        SendInput(1, &ip, sizeof(INPUT));
        ip.ki.dwFlags = KEYEVENTF_KEYUP;
        SendInput(1, &ip, sizeof(INPUT));

    }
    if (senderName == ITK_F) {

        // send f

        ip.ki.wVk = 0x46;
        ip.ki.dwFlags = 0;
        SendInput(1, &ip, sizeof(INPUT));
        ip.ki.dwFlags = KEYEVENTF_KEYUP;
        SendInput(1, &ip, sizeof(INPUT));

    }
    if (senderName == ITK_N) {

        // send n

        ip.ki.wVk = 0x4E;
        ip.ki.dwFlags = 0;
        SendInput(1, &ip, sizeof(INPUT));
        ip.ki.dwFlags = KEYEVENTF_KEYUP;
        SendInput(1, &ip, sizeof(INPUT));

    }
    if (senderName == ITK_P) {

        // send p

        ip.ki.wVk = 0x50;
        ip.ki.dwFlags = 0;
        SendInput(1, &ip, sizeof(INPUT));
        ip.ki.dwFlags = KEYEVENTF_KEYUP;
        SendInput(1, &ip, sizeof(INPUT));

    }
}


void MainWindow::on_ThoughtActivated() {

    if (brain.mouse == BciHost::Mouse::Control) {

        INPUT im;
        im.type = INPUT_MOUSE;
        im.mi.time = 0;
        im.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
        SendInput(1, &im, sizeof(INPUT));
        im.mi.dwFlags = MOUSEEVENTF_LEFTUP;
        SendInput(1, &im, sizeof(INPUT));
    }
}


void MainWindow::on_SuppressFinished() {

    suppressEyes = false;
    suppressTimer.stop();
}


void MainWindow::on_UserPresenceChanged( bool present ) {

    if (present != userPresent) {
        userPresent = present;
        ShowAll( userPresent );
    }
}


void MainWindow::ToggleMouse() {

    eyes.mouse = (eyes.mouse == EyeXHost::Mouse::Off)
            ? EyeXHost::Mouse::Control
            : EyeXHost::Mouse::Off;

    if (eyes.mouse == EyeXHost::Mouse::Off) {

    } else {

    }
    UpdateActivatableRegions();
}


void MainWindow::ToggleBrain() {

    brain.mouse = (brain.mouse == BciHost::Mouse::Off)
            ? BciHost::Mouse::Control
            : BciHost::Mouse::Off;

    if (brain.mouse == BciHost::Mouse::Off) {

    } else {

    }
}


void MainWindow::ToggleMenu() {

    ShowMenu( !isVisibleMenu() );

    UpdateActivatableRegions();
}


void MainWindow::ShowAll( bool visible ) {

    QGraphicsOpacityEffect * effect = new QGraphicsOpacityEffect(this);

    if (visible == true) {

        ui->frameScreen->setGraphicsEffect(effect);

        QPropertyAnimation *a = new QPropertyAnimation(effect,"opacity");
        a->setDuration(700);
        a->setStartValue(0);
        a->setEndValue(1);
        a->setEasingCurve(QEasingCurve::InBack);
        a->start(QPropertyAnimation::DeleteWhenStopped);

    } else {

        ui->frameScreen->setGraphicsEffect(effect);

        QPropertyAnimation *a = new QPropertyAnimation(effect,"opacity");
        a->setDuration(700);
        a->setStartValue(1);
        a->setEndValue(0);
        a->setEasingCurve(QEasingCurve::OutBack);
        a->start(QPropertyAnimation::DeleteWhenStopped);
    }
}


void MainWindow::SlideMenu( int position ) {

    int x = ui->frameSlider->x();
    int newX = MENU_START;

    switch (position) {
    case 2:
        newX = MENU_START + MENU_SPACE;
        break;

    case 3:
        newX = MENU_START + (MENU_SPACE * 2);
        break;
    }

    int y = ui->frameSlider->y();
    int w = ui->frameSlider->width();
    int h = ui->frameSlider->height();

    QPropertyAnimation *animation = new QPropertyAnimation(ui->frameSlider, "geometry");

    animation->setDuration(400);
    animation->setStartValue( QRect(x, y, w, h) );
    animation->setEndValue( QRect(newX, y, w, h) );

    connect(animation, SIGNAL(finished()), this, SLOT( on_AnimationFinished()) );

    animation->start(QAbstractAnimation::DeleteWhenStopped);
}


void MainWindow::ShowMenu( bool visible ) {

    ui->frameScreen->setGraphicsEffect(NULL);

    QGraphicsOpacityEffect * effect = new QGraphicsOpacityEffect(this);

    if (visible == true) {

        if (! isVisibleMenu()) {

            ui->frameMenu->setGraphicsEffect(effect);

            QPropertyAnimation *a = new QPropertyAnimation(effect,"opacity");
            a->setDuration(400);
            a->setStartValue(0);
            a->setEndValue(1);
            a->setEasingCurve(QEasingCurve::InBack);
            a->start(QPropertyAnimation::DeleteWhenStopped);

            ui->frameMenu->show();
        }

    } else {

        if (isVisibleMenu()) ui->frameMenu->setGraphicsEffect(effect);
        if (isVisibleOp())   ShowOp( false );
        if (isVisibleEye())  ShowEye( false );
        if (isVisibleBci())  ShowBci( false );

        QPropertyAnimation *a = new QPropertyAnimation(effect,"opacity");
        a->setDuration(700);
        a->setStartValue(1);
        a->setEndValue(0);
        a->setEasingCurve(QEasingCurve::OutBack);
        a->start(QPropertyAnimation::DeleteWhenStopped);

        connect(a,SIGNAL(finished()),this,SLOT(on_FadeMenuFinished()));
    }

    UpdateActivatableRegions();
}


void MainWindow::ShowOp( bool visible ) {

    ui->frameScreen->setGraphicsEffect(NULL);

    QGraphicsOpacityEffect * effect = new QGraphicsOpacityEffect(this);

    if (visible == true) {

        ShowEye(false);
        ShowBci(false);

        if (! isVisibleOp()) {

            ui->frameOp->setGraphicsEffect(effect);

            QPropertyAnimation *a = new QPropertyAnimation(effect,"opacity");
            a->setDuration(700);
            a->setStartValue(0);
            a->setEndValue(1);
            a->setEasingCurve(QEasingCurve::InBack);
            a->start(QPropertyAnimation::DeleteWhenStopped);

            ui->frameOp->show();
        }

    } else {

        ui->frameOp->setGraphicsEffect(effect);

        QPropertyAnimation *a = new QPropertyAnimation(effect,"opacity");
        a->setDuration(700);
        a->setStartValue(1);
        a->setEndValue(0);
        a->setEasingCurve(QEasingCurve::OutBack);
        a->start(QPropertyAnimation::DeleteWhenStopped);

        connect(a,SIGNAL(finished()),this,SLOT(on_FadeOpsFinished()));
    }

    UpdateActivatableRegions();
}


void MainWindow::ShowEye( bool visible ) {

    ui->frameScreen->setGraphicsEffect(NULL);

    QGraphicsOpacityEffect * effect = new QGraphicsOpacityEffect(this);

    if (visible == true) {

        ShowOp(false);
        ShowBci(false);

        if (! isVisibleEye()) {

            ui->frameEye->setGraphicsEffect(effect);

            QPropertyAnimation *a = new QPropertyAnimation(effect,"opacity");
            a->setDuration(700);
            a->setStartValue(0);
            a->setEndValue(1);
            a->setEasingCurve(QEasingCurve::InBack);
            a->start(QPropertyAnimation::DeleteWhenStopped);

            ui->frameEye->show();
        }

    } else {

        ui->frameEye->setGraphicsEffect(effect);

        QPropertyAnimation *a = new QPropertyAnimation(effect,"opacity");
        a->setDuration(700);
        a->setStartValue(1);
        a->setEndValue(0);
        a->setEasingCurve(QEasingCurve::OutBack);
        a->start(QPropertyAnimation::DeleteWhenStopped);

        connect(a,SIGNAL(finished()),this,SLOT(on_FadeEyeFinished()));
    }

    UpdateActivatableRegions();
}


void MainWindow::ShowBci( bool visible ) {

    ui->frameScreen->setGraphicsEffect(NULL);

    QGraphicsOpacityEffect * effect = new QGraphicsOpacityEffect(this);

    if (visible == true) {

        ShowOp(false);
        ShowEye(false);

        if (! isVisibleBci()) {

            ui->frameBci->setGraphicsEffect(effect);

            QPropertyAnimation *a = new QPropertyAnimation(effect,"opacity");
            a->setDuration(700);
            a->setStartValue(0);
            a->setEndValue(1);
            a->setEasingCurve(QEasingCurve::InBack);
            a->start(QPropertyAnimation::DeleteWhenStopped);

            ui->frameBci->show();
            ui->frameBci->setGraphicsEffect(NULL);
        }

    } else {

        ui->frameBci->setGraphicsEffect(effect);

        QPropertyAnimation *a = new QPropertyAnimation(effect,"opacity");
        a->setDuration(700);
        a->setStartValue(1);
        a->setEndValue(0);
        a->setEasingCurve(QEasingCurve::OutBack);
        a->start(QPropertyAnimation::DeleteWhenStopped);

        connect(a,SIGNAL(finished()),this,SLOT(on_FadeBciFinished()));
    }

    UpdateActivatableRegions();
}


void MainWindow::SuppressEyes( int msec ) {

    suppressEyes = true;
    suppressTimer.start( msec );
}


bool MainWindow::isVisibleApp() {

    return ui->frameScreen->isVisible();
}


bool MainWindow::isVisibleMenu() {

    return ui->frameMenu->isVisible();
}


bool MainWindow::isVisibleOp() {

    return ui->frameOp->isVisible();
}


bool MainWindow::isVisibleEye() {

    return ui->frameEye->isVisible();
}


bool MainWindow::isVisibleBci() {

    return ui->frameBci->isVisible();
}


void MainWindow::on_hotkey_pressed() {

    ToggleMouse();
}


void MainWindow::on_buttonMouse_clicked(){

    ToggleMouse();
}


void MainWindow::on_buttonMain_clicked() {

    ToggleMenu();
}


void MainWindow::on_buttonOp_clicked() {

    if (isVisibleOp()) return;

    SlideMenu(1);

    ShowOp( true );
}


void MainWindow::on_buttonOp1_clicked() {

    SetInteractorProfile(0);
}


void MainWindow::on_buttonOp2_clicked() {

}


void MainWindow::on_buttonOp3_clicked() {

    SetInteractorProfile(1);
}


void MainWindow::on_buttonOp4_clicked() {

}


void MainWindow::on_buttonEye_clicked() {

    if (isVisibleEye()) return;

    ShowEye( true );

    SlideMenu(2);
}


void MainWindow::on_buttonBci_clicked() {

    if (isVisibleBci()) return;

    ShowBci( true );

    SlideMenu(3);
}

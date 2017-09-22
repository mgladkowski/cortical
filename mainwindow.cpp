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

    eyes.Init( (HWND)this->winId() );

    InitializeUi();

    QObject::connect(
                &systemTimer, SIGNAL(timeout()),
                this, SLOT(on_SystemTimer())
    );
    systemTimer.start( 2000 );

}


MainWindow::~MainWindow() {

    delete ui;
}


void MainWindow::InitializeUi() {


    // sets position of main window objects

    QRect   desktopRect = QApplication::desktop()->availableGeometry(this);
    QPoint  center      = desktopRect.center();

    this->showFullScreen();

    ui->frameScreen->move( 0,0 );
    ui->frameScreen->resize( desktopRect.width(), desktopRect.height() );

    int main_X = static_cast<int>( desktopRect.left() + (desktopRect.width() * 0.65) - 400 );
    int main_Y = static_cast<int>( desktopRect.bottom() - ui->widgetMain->height() );

    ui->widgetMain->move( main_X, main_Y) ;


    // connect events for each designer control

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
            b->setActivationType( ActivatorFlags::ACTIVATE_NORMAL );
            b->setFocusPolicy(Qt::NoFocus);
        }
    }

    ActivatorFlags flags = ActivatorFlags::ACTIVATE_QUICK | ActivatorFlags::HIDE_PROGRESS_BAR;
    ui->buttonOp->setActivationType( flags );
    ui->buttonEye->setActivationType( flags );
    ui->buttonBci->setActivationType( flags );

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


void MainWindow::AddInteractor( InteractorParam data ) {

    EyeButton *button = new EyeButton(ui->frameScreen);

    button->setObjectName(data.name);
    button->setText(tr(""));
    button->setGeometry( data.x, data.y, data.width, data.height );

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

    button->setActivationType( data.flags );
    button->setFocusPolicy(Qt::NoFocus);

    this->layout()->addWidget( button );
}


void MainWindow::SetInteractorProfile( int profileId ) {

    ClearInteractorProfile();

    switch (profileId) {
    case 1:
        AddInteractor( InteractorParam(  900,   50,   200,  50, INTERACTOR_PGUP, ActivatorFlags::ACTIVATE_NORMAL | ActivatorFlags::INTERACTOR_INVISIBLE ) );
        AddInteractor( InteractorParam(  900, 1000,   200,  50, INTERACTOR_PGDN, ActivatorFlags::ACTIVATE_NORMAL | ActivatorFlags::INTERACTOR_INVISIBLE ) );
        AddInteractor( InteractorParam( 1700,  700,    50,  50, INTERACTOR_BACK, ActivatorFlags::ACTIVATE_NORMAL | ActivatorFlags::INTERACTOR_DEFAULT ) );
        AddInteractor( InteractorParam( 1700,  800,    50,  50, INTERACTOR_NEWTAB, ActivatorFlags::ACTIVATE_NORMAL | ActivatorFlags::INTERACTOR_DEFAULT ) );
        AddInteractor( InteractorParam( 1700,  900,    50,  50, INTERACTOR_CLSTAB, ActivatorFlags::ACTIVATE_SLOW | ActivatorFlags::INTERACTOR_DANGER ) );
        break;

    case 2:
        AddInteractor( InteractorParam(  900,   50,   200,  50, INTERACTOR_PGUP, ActivatorFlags::ACTIVATE_NORMAL | ActivatorFlags::INTERACTOR_INVISIBLE) );
        AddInteractor( InteractorParam(  900, 1000,   200,  50, INTERACTOR_PGDN, ActivatorFlags::ACTIVATE_NORMAL | ActivatorFlags::INTERACTOR_INVISIBLE) );
        break;

    case 0:
    default:
        break;
    }

    UpdateActivatableRegions();
}


void MainWindow::ClearInteractorProfile() {

    QList<EyeButton*> list = this->findChildren<EyeButton *>();
    foreach(EyeButton *b, list) {

        if (b->isInteractor == true) {
            delete b;
        }
    }

    UpdateActivatableRegions();
}


void MainWindow::on_AnimationFinished() {

    UpdateActivatableRegions();
}


void MainWindow::on_ActivationEvent( int interactorId ) {

}


void MainWindow::on_FadeMenuFinished() {

    ui->frameMenu->hide();
    ui->frameOp->hide();
    ui->frameEye->hide();
    ui->frameBci->hide();

    UpdateActivatableRegions();
}


void MainWindow::on_FadeOpsFinished() {

    ui->frameOp->hide();
    UpdateActivatableRegions();
}


void MainWindow::on_FadeEyeFinished() {

    ui->frameEye->hide();
    UpdateActivatableRegions();
}


void MainWindow::on_FadeBciFinished() {

    ui->frameBci->hide();
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

    if (senderName == INTERACTOR_BACK) {

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
    if (senderName == INTERACTOR_PGUP) {

        // send PAGEUP

        ip.ki.wVk = VK_PRIOR;
        ip.ki.dwFlags = 0;
        SendInput(1, &ip, sizeof(INPUT));
        ip.ki.dwFlags = KEYEVENTF_KEYUP;
        SendInput(1, &ip, sizeof(INPUT));

    }
    if (senderName == INTERACTOR_PGDN) {

        // send PAGEDOWN

        ip.ki.wVk = VK_NEXT;
        ip.ki.dwFlags = 0;
        SendInput(1, &ip, sizeof(INPUT));
        ip.ki.dwFlags = KEYEVENTF_KEYUP;
        SendInput(1, &ip, sizeof(INPUT));

    }
    if (senderName == INTERACTOR_NEWTAB) {

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
    if (senderName == INTERACTOR_CLSTAB) {

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



void MainWindow::ToggleMouse() {

    eyes.mouse = (eyes.mouse == EyeXHost::Mouse::Off)
            ? EyeXHost::Mouse::Control
            : EyeXHost::Mouse::Off;

    if (eyes.mouse == EyeXHost::Mouse::Off) {

    } else {

    }
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

    if (visible == true) {

        if (! isVisibleMenu()) {

            QGraphicsOpacityEffect *effect = new QGraphicsOpacityEffect(this);

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

        QGraphicsOpacityEffect *effect = new QGraphicsOpacityEffect(this);

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

    if (visible == true) {

        ShowEye(false);
        ShowBci(false);

        if (! isVisibleOp()) {

            QGraphicsOpacityEffect *effect = new QGraphicsOpacityEffect(this);

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

        QGraphicsOpacityEffect *effect = new QGraphicsOpacityEffect(this);

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

    if (visible == true) {

        ShowOp(false);
        ShowBci(false);

        if (! isVisibleEye()) {

            QGraphicsOpacityEffect *effect = new QGraphicsOpacityEffect(this);

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

        QGraphicsOpacityEffect *effect = new QGraphicsOpacityEffect(this);

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

    if (visible == true) {

        ShowOp(false);
        ShowEye(false);

        if (! isVisibleBci()) {

            QGraphicsOpacityEffect *effect = new QGraphicsOpacityEffect(this);

            ui->frameBci->setGraphicsEffect(effect);

            QPropertyAnimation *a = new QPropertyAnimation(effect,"opacity");
            a->setDuration(700);
            a->setStartValue(0);
            a->setEndValue(1);
            a->setEasingCurve(QEasingCurve::InBack);
            a->start(QPropertyAnimation::DeleteWhenStopped);

            ui->frameBci->show();
        }

    } else {

        QGraphicsOpacityEffect *effect = new QGraphicsOpacityEffect(this);

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

    qDebug() << qPrintable(focusedExecutable) << focusedIsFullscreen;

    if (focusedExecutable == "C:\\Windows\\explorer.exe") {

        SetInteractorProfile(0);

    } else if (focusedExecutable == "C:\\Program Files (x86)\\Mozilla Firefox\\firefox.exe") {

        SetInteractorProfile(1);

    } else if (focusedExecutable == "C:\\Program Files (x86)\\Google\\Chrome\\Application\\chrome.exe") {

        SetInteractorProfile(1);

    } else if (focusedExecutable == "C:\\Qt\\Tools\\QtCreator\\bin\\qtcreator.exe") {

        SetInteractorProfile(2);

    } else if (focusedExecutable == "C:\\Program Files (x86)\\VideoLAN\\VLC\\vlc.exe") {

        SetInteractorProfile(0);
    }
}


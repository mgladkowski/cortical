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

    // initialize HID

    eyes.Init( (HWND)this->winId() );

    SetInteractorProfile();

    InitializeUi();

    ShowMenu( false );
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
    int main_Y = static_cast<int>( desktopRect.bottom()- ui->widgetMain->height() );

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
    ui->buttonOpenBci->setActivationType( flags );
    ui->buttonEyeX->setActivationType( flags );
    ui->buttonMode->setActivationType( flags );

}


/* Gets the bounds of a button in screen coordinates
 *
 */
RECT MainWindow::GetScreenBounds(EyeButton * button) {

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
 *
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


void MainWindow::SetInteractorProfile() {

    AddInteractor( InteractorParam(  900,   50,   200,  50, INTERACTOR_PGUP, ActivatorFlags::ACTIVATE_NORMAL | ActivatorFlags::INTERACTOR_DEFAULT) );
    AddInteractor( InteractorParam(  900, 1000,   200,  50, INTERACTOR_PGDN, ActivatorFlags::ACTIVATE_NORMAL | ActivatorFlags::INTERACTOR_DEFAULT) );
    AddInteractor( InteractorParam( 1750,  700,    50,  50, INTERACTOR_BACK, ActivatorFlags::ACTIVATE_SLOW | ActivatorFlags::INTERACTOR_DEFAULT) );
    AddInteractor( InteractorParam( 1750,  800,    50,  50, INTERACTOR_NEWTAB, ActivatorFlags::ACTIVATE_SLOW | ActivatorFlags::INTERACTOR_DEFAULT) );
    AddInteractor( InteractorParam( 1750,  900,    50,  50, INTERACTOR_CLSTAB, ActivatorFlags::ACTIVATE_SLOW | ActivatorFlags::INTERACTOR_DANGER) );

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


void MainWindow::on_hotkey_pressed() {

    ToggleMouse();
}


void MainWindow::on_buttonMain_clicked() {

    ToggleMenu();
}


void MainWindow::on_buttonMouse_clicked(){

    ToggleMouse();
}


void MainWindow::on_buttonBCI_clicked() {

    ToggleBrain();
}


void MainWindow::on_ActivationEvent(int interactorId) {

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


void MainWindow::on_Suppress() {

    suppressEyeEvents = false;
    suppressTimer.stop();
}


void MainWindow::SuppressEyeEvents( int msec ) {

    suppressEyeEvents = true;
    suppressTimer.start( msec );
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

    ShowMenu( !IsVisibleMenu() );

    UpdateActivatableRegions();
}


void MainWindow::SlideMenu(int position) {


    switch (position) {

    case 2:
        ui->buttonMode->move(MENU_POSITION_4, ui->buttonMode->y());
        ui->line12->move(LINE_POSITION_34, ui->line12->y());
        ui->buttonEyeX->move(MENU_POSITION_3, ui->buttonEyeX->y());
        ui->line23->move(LINE_POSITION_23, ui->line23->y());
        ui->buttonOpenBci->move(MENU_POSITION_2, ui->buttonOpenBci->y());
        break;

    case 3:
        ui->buttonMode->move(MENU_POSITION_5, ui->buttonMode->y());
        ui->line12->move(LINE_POSITION_45, ui->line12->y());
        ui->buttonEyeX->move(MENU_POSITION_4, ui->buttonEyeX->y());
        ui->line23->move(LINE_POSITION_34, ui->line23->y());
        ui->buttonOpenBci->move(MENU_POSITION_3, ui->buttonOpenBci->y());
        break;

    case 1:
    default:
        ui->buttonMode->move(MENU_POSITION_3, ui->buttonMode->y());
        ui->line12->move(LINE_POSITION_23, ui->line12->y());
        ui->buttonEyeX->move(MENU_POSITION_2, ui->buttonEyeX->y());
        ui->line23->move(LINE_POSITION_12, ui->line23->y());
        ui->buttonOpenBci->move(MENU_POSITION_1, ui->buttonOpenBci->y());
    }

    UpdateActivatableRegions();
}


void MainWindow::ShowMenu(bool visible) {

    if (visible == false) {

        ShowDialogMode( visible );
        ShowDialogEye( visible );
        ShowDialogBrain( visible );
    }

    ui->buttonMode->setVisible( visible );
    ui->buttonEyeX->setVisible( visible );
    ui->buttonOpenBci->setVisible( visible );

    ui->lineDialog->setVisible( false );
    ui->lineMenu->setVisible( visible );
    ui->line12->setVisible( visible );
    ui->line23->setVisible( visible );

    if (visible == true) SlideMenu(1);

    UpdateActivatableRegions();
}


void MainWindow::ShowDialogMode(bool visible) {

    if (visible == true) {

        ShowDialogEye(false);
        ShowDialogBrain(false);
    }

    ui->labelMode->setVisible( visible );
    ui->buttonModeRead->setVisible( visible );
    ui->buttonModeQt->setVisible( visible );
    ui->buttonModePhp->setVisible( visible );
    ui->buttonModeOff->setVisible( visible );

    if (visible == true) {
        ui->lineDialog->setVisible( true );
    }

    UpdateActivatableRegions();
}


void MainWindow::ShowDialogEye(bool visible) {

    if (visible == true) {

        ShowDialogMode(false);
        ShowDialogBrain(false);
    }

    ui->labelEye->setVisible( visible );
    ui->buttonEye1->setVisible( visible );
    ui->buttonEye2->setVisible( visible );
    ui->buttonEye3->setVisible( visible );
    ui->buttonEye4->setVisible( visible );

    if (visible == true) {
        ui->lineDialog->setVisible( true );
    }

    UpdateActivatableRegions();
}


void MainWindow::ShowDialogBrain(bool visible) {

    if (visible == true) {

        ShowDialogMode(false);
        ShowDialogEye(false);
    }

    ui->labelBci->setVisible( visible );
    ui->buttonBci1->setVisible( visible );
    ui->buttonBci2->setVisible( visible );
    ui->buttonBci3->setVisible( visible );
    ui->buttonBci4->setVisible( visible );

    if (visible == true) {
        ui->lineDialog->setVisible( true );
    }

    UpdateActivatableRegions();
}



bool MainWindow::IsVisibleMenu() {

    return ui->buttonMode->isVisible();
}


bool MainWindow::IsVisibleDialogMode() {

    return ui->labelMode->isVisible();
}


bool MainWindow::IsVisibleDialogEye() {

    return ui->labelEye->isVisible();
}


bool MainWindow::IsVisibleDialogBrain() {

    return ui->labelBci->isVisible();
}



void MainWindow::on_buttonMode_clicked() {

    if (IsVisibleDialogMode()) return;

    ShowDialogMode( true );

    SlideMenu(1);
}


void MainWindow::on_buttonModeOff_clicked() {

    ClearInteractorProfile();
    ShowMenu(false);
}


void MainWindow::on_buttonModeRead_clicked() {


    ClearInteractorProfile();
    SetInteractorProfile();
    ShowMenu(false);
}


void MainWindow::on_buttonEyeX_clicked() {

    if (IsVisibleDialogEye()) return;

    ShowDialogEye( true );

    SlideMenu(2);
}


void MainWindow::on_buttonOpenBci_clicked() {

    if (IsVisibleDialogBrain()) return;

    ShowDialogBrain( true );

    SlideMenu(3);
}



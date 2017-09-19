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


    // dialogs

    ShowMenu(false);


    // button custom events

    ui->buttonMode->installEventFilter(this);
    ui->buttonEyeX->installEventFilter(this);
    ui->buttonOpenBci->installEventFilter(this);

    QObject::connect(
                this, SIGNAL(ButtonEnterEvent(QPushButton*)),
                this, SLOT(on_ButtonEnterEvent(QPushButton*))
    );
    QObject::connect(
                this, SIGNAL(ButtonLeaveEvent(QPushButton*)),
                this, SLOT(on_ButtonLeaveEvent(QPushButton*))
    );


    // initialize HID

    UpdateActivatableRegions();

    eyes.Init( (HWND)this->winId() );

    QObject::connect(
                &currentTimer, SIGNAL(timeout()),
                this, SLOT(on_Timer())
    );
    QObject::connect(
                &progressTimer, SIGNAL(timeout()),
                this, SLOT(on_Progress())
    );
    QObject::connect(
                &suppressTimer, SIGNAL(timeout()),
                this, SLOT(on_Suppress())
    );
    QObject::connect(
                &eyes, SIGNAL(ActivationEvent(int)),
                this, SLOT(on_ActivationEvent(int))
    );
    QObject::connect(
                &eyes, SIGNAL(ActivationFocusEvent(int)),
                this, SLOT(on_ActivationFocusEvent(int))
    );

}


MainWindow::~MainWindow() {

    delete ui;
}


/* Override hover event
 *
 */
bool MainWindow::eventFilter(QObject *obj, QEvent *event) {

    // called for QObjects that installed eventFilter so we filter for desired buttons

    if (   obj == (QObject*)ui->buttonMouse
        || obj == (QObject*)ui->buttonBCI
        || obj == (QObject*)ui->buttonMain ) {

        if (event->type() == QEvent::Enter) {

            emit ButtonEnterEvent( (QPushButton*)obj );

        } else if (event->type() == QEvent::Leave) {

            emit ButtonLeaveEvent( (QPushButton*)obj );
        }
    }

    // pass the event on to the parent class

    return QWidget::eventFilter(obj, event);
}


void MainWindow::InitializeUi() {


    // sets position of main window (activator)

    QRect   desktopRect = QApplication::desktop()->availableGeometry(this);
    QPoint  center      = desktopRect.center();

    this->showFullScreen();

    ui->frameScreen->move( 0,0 );
    ui->frameScreen->resize( this->width(), this->height() );

    int main_X = static_cast<int>( desktopRect.left() + (desktopRect.width() * 0.65) - 350 );
    int main_Y = static_cast<int>( desktopRect.bottom()- ui->widgetMain->height() );
    ui->widgetMain->move( main_X, main_Y) ;

    SetInteractorProfile();
}


/* Gets the bounds of a button in screen coordinates
 *
 */
RECT MainWindow::GetScreenBounds(QPushButton * button) {

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

    QList<QPushButton*> list = this->findChildren<QPushButton *>();

    foreach(QPushButton *b, list) {

        if (b->isVisible()) {
            regions.push_back(ActivatableRegion(b->winId(), GetScreenBounds(b)));
        }
    }

    eyes.SetActivatableRegions(regions);
}


void MainWindow::SetInteractorProfile() {

    AddInteractor( InteractorParam( 900,   50,   200,  50, "interactor_PGUP", "601") );
    AddInteractor( InteractorParam( 900, 1000,   200,  50, "interactor_PGDN", "602") );
    AddInteractor( InteractorParam(  20,   50,    50,  50, "interactor_BACK", "603") );

    UpdateActivatableRegions();
}


void MainWindow::AddInteractor( InteractorParam data ) {

    QPushButton *button = new QPushButton(ui->frameScreen);

    button->setObjectName(data.name);
    button->setText(tr(""));
    button->setGeometry( data.x, data.y, data.width, data.height );
    button->setStyleSheet("background:rgba(0,0,0,0.4);");

    QObject::connect(button, SIGNAL(clicked(bool)), this, SLOT(on_InteractorActivated()) );

    layout()->addWidget(button);
}


void MainWindow::ClearInteractorProfile() {

    qDeleteAll(ui->frameScreen->findChildren<QWidget*>("", Qt::FindDirectChildrenOnly));

    UpdateActivatableRegions();
}


void MainWindow::ClearDelays() {

    currentTimer.stop();
    progressTimer.stop();
    progressCounter = 0;

    QPushButton *sender = qobject_cast<QPushButton*>( QWidget::find( (WId)currentInteractor ) );
    if (!sender) return;
    QString senderName = sender->objectName();
    QString css = "";

    if ((senderName == IDC_MOUSE_BUTTON)        ||
        (senderName == IDC_ACTIVATOR_BUTTON)    ||
        (senderName == IDC_BCI_BUTTON)          ||
        (senderName == IDC_BUTTON_MODE_OFF)     ||
        (senderName == IDC_BUTTON_MODE_READ)    ){

        sender->setStyleSheet( css );
    }
}


void MainWindow::on_hotkey_pressed() {

    ClearDelays();
    ToggleMouse();
}


void MainWindow::on_buttonMain_clicked() {

    ClearDelays();
    ToggleMenu();
}


void MainWindow::on_buttonMouse_clicked(){

    ClearDelays();
    ToggleMouse();
}


void MainWindow::on_buttonBCI_clicked() {

    ClearDelays();
    ToggleBrain();
    ClearInteractorProfile();   // TEST
}


void MainWindow::on_ButtonEnterEvent(QPushButton * button) {

    if (button == ui->buttonMouse) {
    }
    if (button == ui->buttonBCI) {
    }
    if (button == ui->buttonMain) {
    }
    if (button == ui->buttonMode) {
        button->click();
    }
    if (button == ui->buttonEyeX) {
        button->click();
    }
    if (button == ui->buttonOpenBci) {
        button->click();
    }
}


void MainWindow::on_ButtonLeaveEvent(QPushButton * button) {

    if (button == ui->buttonMouse) {
    }
    if (button == ui->buttonBCI) {
    }
    if (button == ui->buttonMain) {
    }
    if (button == ui->buttonMode) {
    }
    if (button == ui->buttonEyeX) {
    }
    if (button == ui->buttonOpenBci) {
    }
}


void MainWindow::on_ActivationEvent(int interactorId) {

    if (suppressEyeEvents) return;

    ClearDelays();
    if (interactorId == -1) return;
}


void MainWindow::on_ActivationFocusEvent(int interactorId) {

    if (suppressEyeEvents == true) return;

    currentInteractor = interactorId;
    ClearDelays();

    if (interactorId == -1) {
        ClearHover();
    }

    QPushButton *sender = qobject_cast<QPushButton*>( QWidget::find( (WId)interactorId ) );
    if (!sender) return;
    QString senderName = sender->objectName();

    if ((senderName == IDC_BUTTON_MENU_MODE) ||
        (senderName == IDC_BUTTON_MENU_EYEX) ||
        (senderName == IDC_BUTTON_MENU_BCI)  ){

        GazeHover( sender );
        sender->click();

    } else
    if ((senderName == IDC_ACTIVATOR_BUTTON) ||
        (senderName == IDC_MOUSE_BUTTON)     ||
        (senderName == IDC_BCI_BUTTON)       ||
        (senderName == IDC_BUTTON_MODE_READ) ||
        (senderName == IDC_BUTTON_MODE_QT)   ||
        (senderName == IDC_BUTTON_MODE_PHP)  ||
        (senderName == IDC_BUTTON_MODE_OFF)  ){

        currentTimer.start( intervalActivate );
        progressTimer.start( intervalProgress );

    } else
    if ((senderName == INTERACTOR_BACK)      ||
        (senderName == INTERACTOR_PGUP)      ||
        (senderName == INTERACTOR_PGDN)      ){

        currentTimer.start( intervalInteractor );
        progressTimer.start( intervalProgress );

    } else if (interactorId > 0) {

        GazeHover( sender );
    }
}


void MainWindow::on_InteractorActivated() {

    QPushButton *sender = qobject_cast<QPushButton*>( QWidget::find( (WId)currentInteractor ) );
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
        input[0].ki.wScan = MapVirtualKey(VK_CONTROL, MAPVK_VK_TO_VSC);
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


void MainWindow::on_Timer() {

    ClearDelays();

    QPushButton *sender = qobject_cast<QPushButton*>( QWidget::find( (WId)currentInteractor ) );
    if (!sender) return;
    QString senderName = sender->objectName();

    if (senderName == IDC_MOUSE_BUTTON) {

        ToggleMouse();
    }
    if (senderName == IDC_ACTIVATOR_BUTTON) {

        ToggleMenu();
    }
    if (senderName == IDC_BCI_BUTTON) {

        ToggleBrain();
    }
    if (senderName == IDC_BUTTON_MODE_OFF) {

        ui->buttonModeOff->click();
    }
    if (senderName == IDC_BUTTON_MODE_READ) {

        ui->buttonModeRead->click();
    }
    if ((senderName == INTERACTOR_BACK) ||
        (senderName == INTERACTOR_PGUP) ||
        (senderName == INTERACTOR_PGDN) ){

        on_InteractorActivated();
    }
}


void MainWindow::on_Progress() {

    QPushButton *sender = qobject_cast<QPushButton*>( QWidget::find( (WId)currentInteractor ) );
    if (!sender) return;
    QString senderName = sender->objectName();

    progressCounter = progressCounter + 0.05;
    if (progressCounter > 1) progressCounter = 1;
    QString css = "background: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop: 0 #00fc00, stop: " + QString::number(progressCounter) + " #fcfcfc);";

    if ((senderName == IDC_MOUSE_BUTTON)        ||
        (senderName == IDC_ACTIVATOR_BUTTON)    ||
        (senderName == IDC_BCI_BUTTON)          ||
        (senderName == IDC_BUTTON_MODE_OFF)     ||
        (senderName == IDC_BUTTON_MODE_READ)    ){

        sender->setStyleSheet( css );
    }
    if ((senderName == INTERACTOR_BACK)         ||
        (senderName == INTERACTOR_PGUP)         ||
        (senderName == INTERACTOR_PGDN)         ){

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


void MainWindow::GazeHover(QPushButton * button) {

    ClearHover();
    if (button) button->setStyleSheet("border: 3px solid #29B6F6; background: #D6EAF8;");
}


void MainWindow::ClearHover() {

    ui->buttonMouse->setStyleSheet("");
    ui->buttonMain->setStyleSheet("");
    ui->buttonBCI->setStyleSheet("");
    ui->buttonOpenBci->setStyleSheet("");
    ui->buttonEyeX->setStyleSheet("");
    ui->buttonMode->setStyleSheet("");
    ui->buttonModeRead->setStyleSheet("");
    ui->buttonModeQt->setStyleSheet("");
    ui->buttonModePhp->setStyleSheet("");
    ui->buttonModeOff->setStyleSheet("");
}



void MainWindow::ShowMenu(bool visible) {

    if (visible == false) {

        ShowDialogMode( visible );
        ShowDialogEye( visible );
        ShowDialogBrain( visible );
    }

    ui->buttonMode->setStyleSheet("");
    ui->buttonEyeX->setStyleSheet("");
    ui->buttonOpenBci->setStyleSheet("");

    ui->buttonMode->setVisible( visible );
    ui->buttonEyeX->setVisible( visible );
    ui->buttonOpenBci->setVisible( visible );

    ui->lineDialog->setVisible( false );
    ui->lineMenu->setVisible( visible );
    ui->line12->setVisible( visible );
    ui->line23->setVisible( visible );

    if (visible == true) SlideMenu(1);
}


void MainWindow::ShowDialogMode(bool visible) {

    if (visible == true) {

        ShowDialogEye(false);
        ShowDialogBrain(false);
    }

    ui->buttonModeRead->setStyleSheet("");
    ui->buttonModeQt->setStyleSheet("");
    ui->buttonModePhp->setStyleSheet("");
    ui->buttonModeOff->setStyleSheet("");

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

    ui->buttonEye1->setStyleSheet("");
    ui->buttonEye2->setStyleSheet("");
    ui->buttonEye3->setStyleSheet("");
    ui->buttonEye4->setStyleSheet("");

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

    ui->buttonBci1->setStyleSheet("");
    ui->buttonBci2->setStyleSheet("");
    ui->buttonBci3->setStyleSheet("");
    ui->buttonBci4->setStyleSheet("");

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

    SuppressEyeEvents(intervalDebounce);
}


void MainWindow::on_buttonModeOff_clicked() {

    ClearInteractorProfile();
    ToggleMenu();
}


void MainWindow::on_buttonModeRead_clicked() {

    SetInteractorProfile();
    ToggleMenu();
}


void MainWindow::on_buttonEyeX_clicked() {

    if (IsVisibleDialogEye()) return;

    ShowDialogEye( true );

    SlideMenu(2);

    SuppressEyeEvents(intervalDebounce);
}


void MainWindow::on_buttonOpenBci_clicked() {

    if (IsVisibleDialogBrain()) return;

    ShowDialogBrain( true );

    SlideMenu(3);

    SuppressEyeEvents(intervalDebounce);
}



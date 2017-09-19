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


    // sets position of main window (activator)

    QRect desktopRect = QApplication::desktop()->availableGeometry(this);
    QPoint center = desktopRect.center();
    int desired_X = static_cast<int>( desktopRect.left() + (desktopRect.width() * 0.65) - 350 );
    int desired_Y = static_cast<int>( desktopRect.bottom()-height()+20 );
    this->move(desired_X, desired_Y);


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

    regions.push_back(ActivatableRegion(IDC_ACTIVATOR_BUTTON, GetScreenBounds(ui->buttonMain)));
    regions.push_back(ActivatableRegion(IDC_MOUSE_BUTTON, GetScreenBounds(ui->buttonMouse)));
    regions.push_back(ActivatableRegion(IDC_BCI_BUTTON, GetScreenBounds(ui->buttonBCI)));

    if (IsVisibleMenu()) {

        regions.push_back(ActivatableRegion(IDC_BUTTON_MENU_MODE, GetScreenBounds(ui->buttonMode)));
        regions.push_back(ActivatableRegion(IDC_BUTTON_MENU_EYEX, GetScreenBounds(ui->buttonEyeX)));
        regions.push_back(ActivatableRegion(IDC_BUTTON_MENU_BCI, GetScreenBounds(ui->buttonOpenBci)));
    }

    if (IsVisibleDialogMode()) {

        regions.push_back(ActivatableRegion(IDC_BUTTON_MODE_READ, GetScreenBounds(ui->buttonModeRead)));
        regions.push_back(ActivatableRegion(IDC_BUTTON_MODE_QT, GetScreenBounds(ui->buttonModeQt)));
        regions.push_back(ActivatableRegion(IDC_BUTTON_MODE_PHP, GetScreenBounds(ui->buttonModePhp)));
        regions.push_back(ActivatableRegion(IDC_BUTTON_MODE_OFF, GetScreenBounds(ui->buttonModeOff)));
    }
    if (IsVisibleDialogEye()) {

        regions.push_back(ActivatableRegion(IDC_BUTTON_EYEX_1, GetScreenBounds(ui->buttonEye1)));
        regions.push_back(ActivatableRegion(IDC_BUTTON_EYEX_2, GetScreenBounds(ui->buttonEye2)));
        regions.push_back(ActivatableRegion(IDC_BUTTON_EYEX_3, GetScreenBounds(ui->buttonEye3)));
        regions.push_back(ActivatableRegion(IDC_BUTTON_EYEX_4, GetScreenBounds(ui->buttonEye4)));
    }
    if (IsVisibleDialogBrain()) {

        regions.push_back(ActivatableRegion(IDC_BUTTON_BCI_1, GetScreenBounds(ui->buttonBci1)));
        regions.push_back(ActivatableRegion(IDC_BUTTON_BCI_2, GetScreenBounds(ui->buttonBci2)));
        regions.push_back(ActivatableRegion(IDC_BUTTON_BCI_3, GetScreenBounds(ui->buttonBci3)));
        regions.push_back(ActivatableRegion(IDC_BUTTON_BCI_4, GetScreenBounds(ui->buttonBci4)));
    }

    eyes.SetActivatableRegions(regions);
}


void MainWindow::ClearDelays() {

    currentTimer.stop();
    progressTimer.stop();
    progressCounter = 0;

    ui->buttonMain->setStyleSheet("");
    ui->buttonMouse->setStyleSheet("");
    ui->buttonBCI->setStyleSheet("");
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
}


void MainWindow::on_ButtonEnterEvent(QPushButton * button) {

    if (button == ui->buttonMouse) {
    }
    if (button == ui->buttonBCI) {
    }
    if (button == ui->buttonMain) {
    }
    if (button == ui->buttonMode) {
        on_buttonMode_clicked();
    }
    if (button == ui->buttonEyeX) {
        on_buttonEyeX_clicked();
    }
    if (button == ui->buttonOpenBci) {
        on_buttonOpenBci_clicked();
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

    if (currentInteractor == IDC_BUTTON_MENU_MODE) {

        GazeHover( ui->buttonMode );
        on_buttonMode_clicked();
    }
    else if (currentInteractor == IDC_BUTTON_MENU_EYEX) {

        GazeHover( ui->buttonEyeX );
        on_buttonEyeX_clicked();
    }
    else if (currentInteractor == IDC_BUTTON_MENU_BCI) {

        GazeHover( ui->buttonOpenBci );
        on_buttonOpenBci_clicked();
    }

    else if (currentInteractor == IDC_BUTTON_MODE_READ) {
        GazeHover(ui->buttonModeRead);
    }
    else if (currentInteractor == IDC_BUTTON_MODE_QT) {
        GazeHover(ui->buttonModeQt);
    }
    else if (currentInteractor == IDC_BUTTON_MODE_PHP) {
        GazeHover(ui->buttonModePhp);
    }
    else if (currentInteractor == IDC_BUTTON_MODE_OFF) {
        GazeHover(ui->buttonModeOff);
    }

    else if (interactorId > -1) {

        currentTimer.start( intervalActivate );
        progressTimer.start( intervalProgress );
    }
}


void MainWindow::on_Timer() {

    ClearDelays();

    if (currentInteractor == IDC_MOUSE_BUTTON) {

        ToggleMouse();
    }
    if (currentInteractor == IDC_ACTIVATOR_BUTTON) {

        ToggleMenu();
    }
    if (currentInteractor == IDC_BCI_BUTTON) {

        ToggleBrain();
    }
}


void MainWindow::on_Progress() {

    progressCounter = progressCounter + 0.05;
    if (progressCounter > 1) progressCounter = 1;
    QString css = "background: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop: 0 #00fc00, stop: " + QString::number(progressCounter) + " #fcfcfc);";

    if (currentInteractor == IDC_MOUSE_BUTTON) {

        ui->buttonMouse->setStyleSheet( css );
    }
    if (currentInteractor == IDC_ACTIVATOR_BUTTON) {

        ui->buttonMain->setStyleSheet( css );
    }
    if (currentInteractor == IDC_BCI_BUTTON) {

        ui->buttonBCI->setStyleSheet( css );
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

    ui->buttonOpenBci->setStyleSheet("");
    ui->buttonEyeX->setStyleSheet("");
    ui->buttonMode->setStyleSheet("");

    ui->buttonModeRead->setStyleSheet("");
    ui->buttonModeQt->setStyleSheet("");
    ui->buttonModePhp->setStyleSheet("");
    ui->buttonModeOff->setStyleSheet("");

    button->setStyleSheet("border: 3px solid #29B6F6; background: #D6EAF8;");
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


void MainWindow::on_buttonOpenBci_clicked() {

    if (IsVisibleDialogBrain()) return;

    ShowDialogBrain( true );

    SlideMenu(3);

    SuppressEyeEvents(intervalDebounce);
}


void MainWindow::on_buttonEyeX_clicked() {

    if (IsVisibleDialogEye()) return;

    ShowDialogEye( true );

    SlideMenu(2);

    SuppressEyeEvents(intervalDebounce);
}


void MainWindow::on_buttonMode_clicked() {

    if (IsVisibleDialogMode()) return;

    ShowDialogMode( true );

    SlideMenu(1);

    SuppressEyeEvents(intervalDebounce);
}

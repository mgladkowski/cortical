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
    int desired_Y = static_cast<int>( desktopRect.bottom()-height() );
    this->move(desired_X, desired_Y);


    // dialogs

    dialogMode = new DialogMode;
    dialogEyeX = new DialogEyeX;
    dialogBci = new DialogBci;

    ui->buttonMode->setVisible( false );
    ui->buttonEyeX->setVisible( false );
    ui->buttonOpenBci->setVisible( false );


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
                &eyes, SIGNAL(ActivationEvent(int)),
                this, SLOT(on_ActivationEvent(int))
    );
    QObject::connect(
                &eyes, SIGNAL(ActivationFocusEvent(int)),
                this, SLOT(on_ActivationFocusEvent(int))
    );

}


MainWindow::~MainWindow() {

    delete dialogMode;
    delete dialogEyeX;
    delete dialogBci;
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

    if (ui->buttonMode->isVisible()) {

        regions.push_back(ActivatableRegion(IDC_BUTTON_MENU_MODE, GetScreenBounds(ui->buttonMode)));
        regions.push_back(ActivatableRegion(IDC_BUTTON_MENU_EYEX, GetScreenBounds(ui->buttonEyeX)));
        regions.push_back(ActivatableRegion(IDC_BUTTON_MENU_BCI, GetScreenBounds(ui->buttonOpenBci)));
    }

    eyes.SetActivatableRegions(regions);
}


void MainWindow::ClearDelays() {

    currentTimer.stop();
    progressTimer.stop();
    progressCounter = 0;

    ui->buttonMain->setStyleSheet("* { background: #fcfcfc; }");
    ui->buttonMouse->setStyleSheet("* { background: #fcfcfc; }");
    ui->buttonBCI->setStyleSheet("* { background: #fcfcfc; }");
}


void MainWindow::ToggleMouse() {

    eyes.mouse = (eyes.mouse == EyeXHost::Mouse::Off)
            ? EyeXHost::Mouse::Control
            : EyeXHost::Mouse::Off;

    if (eyes.mouse == EyeXHost::Mouse::Off) {

    } else {

    }
}


void MainWindow::ToggleMenu() {

    bool visible = !ui->buttonMode->isVisible();

    ui->buttonOpenBci->setStyleSheet("");
    ui->buttonEyeX->setStyleSheet("");
    ui->buttonMode->setStyleSheet("");

    ui->buttonMode->setVisible( visible );
    ui->buttonEyeX->setVisible( visible );
    ui->buttonOpenBci->setVisible( visible );

    if (visible == false) {

        dialogBci->hide();
        dialogEyeX->hide();
        dialogMode->hide();
    }

    UpdateActivatableRegions();
}


void MainWindow::ToggleBrain() {

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

    ClearDelays();
    if (interactorId == -1) return;
}


void MainWindow::on_ActivationFocusEvent(int interactorId) {

    currentInteractor = interactorId;
    ClearDelays();

    if (currentInteractor == IDC_BUTTON_MENU_MODE) {
        on_buttonMode_clicked();
    }
    else if (currentInteractor == IDC_BUTTON_MENU_EYEX) {
        on_buttonEyeX_clicked();
    }
    else if (currentInteractor == IDC_BUTTON_MENU_BCI) {
        on_buttonOpenBci_clicked();
    }
    else if (interactorId > -1) {

        currentTimer.start( intervalActivate );
        progressTimer.start( intervalProgress );
    }
}


void MainWindow::on_Progress() {

    progressCounter = progressCounter + 0.05;
    if (progressCounter > 1) progressCounter = 1;
    QString background = "* { background: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop: 0 #00fc00, stop: " + QString::number(progressCounter) + "  #fcfcfc); }";

    if (currentInteractor == IDC_MOUSE_BUTTON) {

        ui->buttonMouse->setStyleSheet( background );
    }
    if (currentInteractor == IDC_ACTIVATOR_BUTTON) {

        ui->buttonMain->setStyleSheet( background );
    }
    if (currentInteractor == IDC_BCI_BUTTON) {

        ui->buttonBCI->setStyleSheet( background );
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


void MainWindow::on_buttonOpenBci_clicked() {

    ui->buttonOpenBci->setStyleSheet("border: 3px solid #29B6F6; background: #D6EAF8;");
    ui->buttonEyeX->setStyleSheet("");
    ui->buttonMode->setStyleSheet("");

    if (dialogMode->isVisible()) dialogMode->close();
    if (dialogEyeX->isVisible()) dialogEyeX->close();
    if (!dialogBci->isVisible()) dialogBci->show();
}


void MainWindow::on_buttonEyeX_clicked() {

    ui->buttonOpenBci->setStyleSheet("");
    ui->buttonEyeX->setStyleSheet("border: 3px solid #29B6F6; background: #D6EAF8;");
    ui->buttonMode->setStyleSheet("");

    if (dialogMode->isVisible()) dialogMode->close();
    if (dialogBci->isVisible()) dialogBci->close();
    if (!dialogEyeX->isVisible()) dialogEyeX->show();
}


void MainWindow::on_buttonMode_clicked() {

    ui->buttonOpenBci->setStyleSheet("");
    ui->buttonEyeX->setStyleSheet("");
    ui->buttonMode->setStyleSheet("border: 3px solid #29B6F6; background: #D6EAF8;");

    if (dialogEyeX->isVisible()) dialogEyeX->close();
    if (dialogBci->isVisible()) dialogBci->close();
    if (!dialogMode->isVisible()) dialogMode->show();
}

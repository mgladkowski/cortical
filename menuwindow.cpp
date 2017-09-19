#include "menuwindow.h"
#include "ui_menuwindow.h"


MenuWindow::MenuWindow(QWidget *parent) : QDialog(parent), ui(new Ui::MenuWindow) {


    mainWindow = (MainWindow*)parent;


    // frameless always on top

    Qt::WindowFlags flags = this->windowFlags();
    setWindowFlags(flags | Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint | Qt::SubWindow);
    setParent(0);
    setAttribute(Qt::WA_TranslucentBackground, true);
    setAttribute(Qt::WA_NoSystemBackground, true);

    ui->setupUi(this);


    // center-bottom on screen

    QRect desktopRect = QApplication::desktop()->availableGeometry(this);
    QPoint center = desktopRect.center();
    this->move(center.x()-width()*0.5, desktopRect.bottom()-height());


    // setup dialogs

    dialogMode = new DialogMode;
    dialogEyeX = new DialogEyeX;
    dialogBci = new DialogBci;


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

    QObject::connect(
                &mainWindow->eyes, SIGNAL(ActivationEvent(int)),
                this, SLOT(on_ActivationEvent(int))
    );
    QObject::connect(
                &mainWindow->eyes, SIGNAL(ActivationFocusEvent(int)),
                this, SLOT(on_ActivationFocusEvent(int))
    );

}


MenuWindow::~MenuWindow() {

    delete dialogBci;
    delete dialogEyeX;
    delete dialogMode;
    delete ui;
}


/* Override hover event
 *
 */
bool MenuWindow::eventFilter(QObject *obj, QEvent *event) {

    // called for QObjects that installed eventFilter so we filter for desired buttons

    if (   obj == (QObject*)ui->buttonMode
        || obj == (QObject*)ui->buttonEyeX
        || obj == (QObject*)ui->buttonOpenBci ) {

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
RECT MenuWindow::GetScreenBounds(QPushButton * button) {

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
void MenuWindow::UpdateActivatableRegions(std::vector<ActivatableRegion> & regions) {

    regions.push_back(ActivatableRegion(IDC_BUTTON_MENU_MODE, GetScreenBounds(ui->buttonMode)));
    regions.push_back(ActivatableRegion(IDC_BUTTON_MENU_EYEX, GetScreenBounds(ui->buttonEyeX)));
    regions.push_back(ActivatableRegion(IDC_BUTTON_MENU_BCI, GetScreenBounds(ui->buttonOpenBci)));
}


void MenuWindow::ClearDelays() {

    currentTimer.stop();
    progressTimer.stop();
    progressCounter = 0;

    ui->buttonMode->setStyleSheet("* { background: #fcfcfc; }");
    ui->buttonEyeX->setStyleSheet("* { background: #fcfcfc; }");
    ui->buttonOpenBci->setStyleSheet("* { background: #fcfcfc; }");
}


void MenuWindow::on_buttonMode_clicked() {

    if (dialogEyeX->isVisible()) dialogEyeX->close();
    if (dialogBci->isVisible()) dialogBci->close();
    if (!dialogMode->isVisible()) dialogMode->show();
}


void MenuWindow::on_buttonEyeX_clicked() {

    if (dialogMode->isVisible()) dialogMode->close();
    if (dialogBci->isVisible()) dialogBci->close();
    if (!dialogEyeX->isVisible()) dialogEyeX->show();
}


void MenuWindow::on_buttonOpenBci_clicked() {

    if (dialogMode->isVisible()) dialogMode->close();
    if (dialogEyeX->isVisible()) dialogEyeX->close();
    if (!dialogBci->isVisible()) dialogBci->show();
}


void MenuWindow::on_ButtonEnterEvent(QPushButton * button) {

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


void MenuWindow::on_ButtonLeaveEvent(QPushButton * button) {

    if (button == ui->buttonMode) {
    }
    if (button == ui->buttonEyeX) {
    }
    if (button == ui->buttonOpenBci) {
    }
}


void MenuWindow::on_MenuWindow_finished(int result) {

    if (dialogMode->isVisible()) dialogMode->close();
    if (dialogEyeX->isVisible()) dialogEyeX->close();
    if (dialogBci->isVisible()) dialogBci->close();
}


void MenuWindow::on_ActivationEvent(int interactorId) {

    if (interactorId == -1) return;
}


void MenuWindow::on_ActivationFocusEvent(int interactorId) {

    currentInteractor = interactorId;

    if (currentInteractor == IDC_BUTTON_MENU_MODE) {
        on_buttonMode_clicked();
    }
    else if (currentInteractor == IDC_BUTTON_MENU_EYEX) {
        on_buttonEyeX_clicked();
    }
    else if (currentInteractor == IDC_BUTTON_MENU_BCI) {
        on_buttonOpenBci_clicked();
    }
}


void MenuWindow::on_Progress() {

    progressCounter = progressCounter + 0.05;
    if (progressCounter > 1) progressCounter = 1;
    QString background = "* { background: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop: 0 #00fc00, stop: " + QString::number(progressCounter) + "  #fcfcfc); }";

    if (currentInteractor == IDC_BUTTON_MENU_MODE) {

        ui->buttonMode->setStyleSheet( background );
    }
    if (currentInteractor == IDC_BUTTON_MENU_EYEX) {

        ui->buttonEyeX->setStyleSheet( background );
    }
    if (currentInteractor == IDC_BUTTON_MENU_BCI) {

        ui->buttonOpenBci->setStyleSheet( background );
    }
}


void MenuWindow::on_Timer() {

    ClearDelays();

    if (currentInteractor == IDC_BUTTON_MENU_MODE) {
        on_buttonMode_clicked();
    }
    if (currentInteractor == IDC_BUTTON_MENU_EYEX) {
        on_buttonEyeX_clicked();
    }
    if (currentInteractor == IDC_BUTTON_MENU_BCI) {
        on_buttonOpenBci_clicked();
    }
}



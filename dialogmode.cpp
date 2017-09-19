#include "dialogmode.h"
#include "ui_dialogmode.h"


DialogMode::DialogMode(QWidget *parent) : QDialog(parent), ui(new Ui::DialogMode) {


    // frameless always on top

    Qt::WindowFlags flags = this->windowFlags();
    setWindowFlags(flags | Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint | Qt::SubWindow);
    setParent(0);
    setAttribute(Qt::WA_TranslucentBackground, true);
    setAttribute(Qt::WA_NoSystemBackground, true);

    ui->setupUi(this);


    // position

    QRect screenSize = QApplication::desktop()->availableGeometry(this);
    QPoint center = screenSize.center();
    int desired_X = static_cast<int>( center.x()-width()*0.5 );
    int desired_Y = static_cast<int>( screenSize.bottom()-height()-200 );
    this->move(desired_X, desired_Y);


    // button custom events

    ui->buttonModeRead->installEventFilter(this);
    ui->buttonModeQt->installEventFilter(this);
    ui->buttonModePhp->installEventFilter(this);
    ui->buttonModeOff->installEventFilter(this);

    QObject::connect(
                this, SIGNAL(ButtonEnterEvent(QPushButton*)),
                this, SLOT(on_ButtonEnterEvent(QPushButton*))
    );
    QObject::connect(
                this, SIGNAL(ButtonLeaveEvent(QPushButton*)),
                this, SLOT(on_ButtonLeaveEvent(QPushButton*))
    );


    // hook into eye events

//    QObject::connect(
//                &currentTimer, SIGNAL(timeout()),
//                this, SLOT(on_Timer())
//    );
//    QObject::connect(
//                &progressTimer, SIGNAL(timeout()),
//                this, SLOT(on_Progress())
//    );
//    QObject::connect(
//                eyes, SIGNAL(ActivationEvent(int)),
//                this, SLOT(on_ActivationEvent(int))
//    );
//    QObject::connect(
//                eyes, SIGNAL(ActivationFocusEvent(int)),
//                this, SLOT(on_ActivationFocusEvent(int))
//    );

}


DialogMode::~DialogMode() {

    delete ui;
}


/* Override hover event
 *
 */
bool DialogMode::eventFilter(QObject *obj, QEvent *event) {

    // called for QObjects that installed eventFilter so we filter for desired buttons

    if (   obj == (QObject*)ui->buttonModeRead
        || obj == (QObject*)ui->buttonModeQt
        || obj == (QObject*)ui->buttonModePhp
        || obj == (QObject*)ui->buttonModeOff) {

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
RECT DialogMode::GetScreenBounds(QPushButton * button) {

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
void DialogMode::UpdateActivatableRegions(std::vector<ActivatableRegion> & regions) {

    regions.push_back(ActivatableRegion(IDC_BUTTON_MODE_READ, GetScreenBounds(ui->buttonModeRead)));
    regions.push_back(ActivatableRegion(IDC_BUTTON_MODE_QT, GetScreenBounds(ui->buttonModeQt)));
    regions.push_back(ActivatableRegion(IDC_BUTTON_MODE_PHP, GetScreenBounds(ui->buttonModePhp)));
    regions.push_back(ActivatableRegion(IDC_BUTTON_MODE_OFF, GetScreenBounds(ui->buttonModeOff)));
}


void DialogMode::ClearDelays() {

    currentTimer.stop();
    progressTimer.stop();
    progressCounter = 0;

    ui->buttonModeRead->setStyleSheet("* { background: #fcfcfc; }");
    ui->buttonModeQt->setStyleSheet("* { background: #fcfcfc; }");
    ui->buttonModePhp->setStyleSheet("* { background: #fcfcfc; }");
    ui->buttonModeOff->setStyleSheet("* { background: #fcfcfc; }");
}


void DialogMode::on_ButtonEnterEvent(QPushButton * button) {
}


void DialogMode::on_ButtonLeaveEvent(QPushButton * button) {
}


void DialogMode::on_ActivationEvent(int interactorId) {

    ClearDelays();
    if (interactorId == -1) return;
}


void DialogMode::on_ActivationFocusEvent(int interactorId) {

    currentInteractor = interactorId;
    ClearDelays();

    if (interactorId > -1) {

        currentTimer.start( intervalActivate );
        progressTimer.start( intervalProgress );
    }
}


void DialogMode::on_Progress() {

    progressCounter = progressCounter + 0.05;
    if (progressCounter > 1) progressCounter = 1;
    QString background = "* { background: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop: 0 #81D4FA, stop: " + QString::number(progressCounter) + "  #fcfcfc); }";

    if (currentInteractor == IDC_BUTTON_MODE_READ) {

        ui->buttonModeRead->setStyleSheet( background );
    }
    if (currentInteractor == IDC_BUTTON_MODE_QT) {

        ui->buttonModeQt->setStyleSheet( background );
    }
    if (currentInteractor == IDC_BUTTON_MODE_PHP) {

        ui->buttonModePhp->setStyleSheet( background );
    }
    if (currentInteractor == IDC_BUTTON_MODE_OFF) {

        ui->buttonModeOff->setStyleSheet( background );
    }
}


void DialogMode::on_Timer() {

    ClearDelays();

    if (currentInteractor == IDC_BUTTON_MODE_READ) {

    }
    if (currentInteractor == IDC_BUTTON_MODE_QT) {

    }
    if (currentInteractor == IDC_BUTTON_MODE_PHP) {

    }
    if (currentInteractor == IDC_BUTTON_MODE_OFF) {

    }
}



#include "dialogeyex.h"
#include "ui_dialogeyex.h"


DialogEyeX::DialogEyeX(QWidget *parent) : QDialog(parent), ui(new Ui::DialogEyeX) {


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

    ui->buttonEye1->installEventFilter(this);
    ui->buttonEye2->installEventFilter(this);
    ui->buttonEye3->installEventFilter(this);
    ui->buttonEye4->installEventFilter(this);

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


DialogEyeX::~DialogEyeX() {

    delete ui;
}


/* Override hover event
 *
 */
bool DialogEyeX::eventFilter(QObject *obj, QEvent *event) {

    // called for QObjects that installed eventFilter so we filter for desired buttons

    if (   obj == (QObject*)ui->buttonEye1
        || obj == (QObject*)ui->buttonEye2
        || obj == (QObject*)ui->buttonEye3
        || obj == (QObject*)ui->buttonEye4 ) {

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
RECT DialogEyeX::GetScreenBounds(QPushButton * button) {

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
void DialogEyeX::UpdateActivatableRegions(std::vector<ActivatableRegion> & regions) {

    regions.push_back(ActivatableRegion(IDC_BUTTON_EYEX_1, GetScreenBounds(ui->buttonEye1)));
    regions.push_back(ActivatableRegion(IDC_BUTTON_EYEX_2, GetScreenBounds(ui->buttonEye2)));
    regions.push_back(ActivatableRegion(IDC_BUTTON_EYEX_3, GetScreenBounds(ui->buttonEye3)));
    regions.push_back(ActivatableRegion(IDC_BUTTON_EYEX_4, GetScreenBounds(ui->buttonEye4)));
}


void DialogEyeX::ClearDelays() {

    currentTimer.stop();
    progressTimer.stop();
    progressCounter = 0;

    ui->buttonEye1->setStyleSheet("* { background: #fcfcfc; }");
    ui->buttonEye2->setStyleSheet("* { background: #fcfcfc; }");
    ui->buttonEye3->setStyleSheet("* { background: #fcfcfc; }");
    ui->buttonEye4->setStyleSheet("* { background: #fcfcfc; }");
}


void DialogEyeX::on_ButtonEnterEvent(QPushButton * button) {
}


void DialogEyeX::on_ButtonLeaveEvent(QPushButton * button) {
}


void DialogEyeX::on_ActivationEvent(int interactorId) {

    ClearDelays();
    if (interactorId == -1) return;
}


void DialogEyeX::on_ActivationFocusEvent(int interactorId) {

    currentInteractor = interactorId;
    ClearDelays();

    if (interactorId > -1) {

        currentTimer.start( intervalActivate );
        progressTimer.start( intervalProgress );
    }
}


void DialogEyeX::on_Progress() {

    progressCounter = progressCounter + 0.05;
    if (progressCounter > 1) progressCounter = 1;
    QString background = "* { background: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop: 0 #81D4FA, stop: " + QString::number(progressCounter) + "  #fcfcfc); }";

    if (currentInteractor == IDC_BUTTON_EYEX_1) {

        ui->buttonEye1->setStyleSheet( background );
    }
    if (currentInteractor == IDC_BUTTON_EYEX_2) {

        ui->buttonEye2->setStyleSheet( background );
    }
    if (currentInteractor == IDC_BUTTON_EYEX_3) {

        ui->buttonEye3->setStyleSheet( background );
    }
    if (currentInteractor == IDC_BUTTON_EYEX_4) {

        ui->buttonEye4->setStyleSheet( background );
    }
}


void DialogEyeX::on_Timer() {

    ClearDelays();

    if (currentInteractor == IDC_BUTTON_EYEX_1) {

    }
    if (currentInteractor == IDC_BUTTON_EYEX_2) {

    }
    if (currentInteractor == IDC_BUTTON_EYEX_3) {

    }
    if (currentInteractor == IDC_BUTTON_EYEX_4) {

    }
}



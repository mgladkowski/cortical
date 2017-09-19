#include "dialogbci.h"
#include "ui_dialogbci.h"


DialogBci::DialogBci(QWidget *parent) : QDialog(parent), ui(new Ui::DialogBci) {


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

    ui->buttonBci1->installEventFilter(this);
    ui->buttonBci2->installEventFilter(this);
    ui->buttonBci3->installEventFilter(this);
    ui->buttonBci4->installEventFilter(this);

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


DialogBci::~DialogBci() {

    delete ui;
}


/* Override hover event
 *
 */
bool DialogBci::eventFilter(QObject *obj, QEvent *event) {

    // called for QObjects that installed eventFilter so we filter for desired buttons

    if (   obj == (QObject*)ui->buttonBci1
        || obj == (QObject*)ui->buttonBci2
        || obj == (QObject*)ui->buttonBci3
        || obj == (QObject*)ui->buttonBci4 ) {

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
RECT DialogBci::GetScreenBounds(QPushButton * button) {

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
void DialogBci::UpdateActivatableRegions(std::vector<ActivatableRegion> & regions) {

    regions.push_back(ActivatableRegion(IDC_BUTTON_BCI_1, GetScreenBounds(ui->buttonBci1)));
    regions.push_back(ActivatableRegion(IDC_BUTTON_BCI_2, GetScreenBounds(ui->buttonBci2)));
    regions.push_back(ActivatableRegion(IDC_BUTTON_BCI_3, GetScreenBounds(ui->buttonBci3)));
    regions.push_back(ActivatableRegion(IDC_BUTTON_BCI_4, GetScreenBounds(ui->buttonBci4)));
}


void DialogBci::ClearDelays() {

    currentTimer.stop();
    progressTimer.stop();
    progressCounter = 0;

    ui->buttonBci1->setStyleSheet("* { background: #fcfcfc; }");
    ui->buttonBci2->setStyleSheet("* { background: #fcfcfc; }");
    ui->buttonBci3->setStyleSheet("* { background: #fcfcfc; }");
    ui->buttonBci4->setStyleSheet("* { background: #fcfcfc; }");
}


void DialogBci::on_ButtonEnterEvent(QPushButton * button) {
}


void DialogBci::on_ButtonLeaveEvent(QPushButton * button) {
}


void DialogBci::on_ActivationEvent(int interactorId) {

    ClearDelays();
    if (interactorId == -1) return;
}


void DialogBci::on_ActivationFocusEvent(int interactorId) {

    currentInteractor = interactorId;
    ClearDelays();

    if (interactorId > -1) {

        currentTimer.start( intervalActivate );
        progressTimer.start( intervalProgress );
    }
}


void DialogBci::on_Progress() {

    progressCounter = progressCounter + 0.05;
    if (progressCounter > 1) progressCounter = 1;
    QString background = "* { background: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop: 0 #81D4FA, stop: " + QString::number(progressCounter) + "  #fcfcfc); }";

    if (currentInteractor == IDC_BUTTON_BCI_1) {

        ui->buttonBci1->setStyleSheet( background );
    }
    if (currentInteractor == IDC_BUTTON_BCI_2) {

        ui->buttonBci2->setStyleSheet( background );
    }
    if (currentInteractor == IDC_BUTTON_BCI_3) {

        ui->buttonBci3->setStyleSheet( background );
    }
    if (currentInteractor == IDC_BUTTON_BCI_4) {

        ui->buttonBci4->setStyleSheet( background );
    }
}


void DialogBci::on_Timer() {

    ClearDelays();

    if (currentInteractor == IDC_BUTTON_BCI_1) {

    }
    if (currentInteractor == IDC_BUTTON_BCI_2) {

    }
    if (currentInteractor == IDC_BUTTON_BCI_3) {

    }
    if (currentInteractor == IDC_BUTTON_BCI_4) {

    }
}



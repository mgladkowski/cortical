#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {


    // setup frameless always on top main window

    Qt::WindowFlags flags = this->windowFlags();
    setWindowFlags(flags | Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint);
    setParent(0);
    setAttribute(Qt::WA_NoSystemBackground, true);
    setAttribute(Qt::WA_TranslucentBackground, true);

    ui->setupUi(this);


    // sets position of main window (activator)

    QDesktopWidget widget;
    QRect screenSize = widget.availableGeometry(widget.primaryScreen());
    int desired_X = static_cast<int>( screenSize.left() + (screenSize.width() * 0.65) );
    int desired_Y = static_cast<int>( screenSize.bottom() );
    this->move(desired_X, desired_Y);


    // initialize HID state

    ema_size        = 20;
    ema_multiplier  = 2.0 / (ema_size + 1);
    eyeMouse        = true;
    gazeX           = 0;
    gazeY           = 0;
    emaX            = 0;
    emaY            = 0;

    QObject::connect(
                &eyes, SIGNAL(GazeEvent(int, int)),
                this, SLOT(on_gaze_event(int,int))
    );

    eyes.Init( (HWND)this->winId() );
}


MainWindow::~MainWindow() {

    delete ui;
}


void MainWindow::on_gaze_event(int X, int Y) {

    if (!eyeMouse) return;

    if (X < 1 || Y < 1) return;

    if ((GetKeyState(VK_LBUTTON) & 0x80) != 0) return;
    if ((GetKeyState(VK_RBUTTON) & 0x80) != 0) return;

    gazeX = X;
    gazeY = Y;

    emaX = ((gazeX - emaX) * ema_multiplier) + emaX;
    emaY = ((gazeY - emaY) * ema_multiplier) + emaY;

    QCursor::setPos(emaX, emaY);
}


void MainWindow::on_buttonMain_clicked() {

    this->close();
}

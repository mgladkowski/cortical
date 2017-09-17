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


    // initialize HID

    eyes.Init( (HWND)this->winId() );
}


MainWindow::~MainWindow() {

    delete ui;
}


void MainWindow::on_buttonMain_clicked() {

    eyes.mouse = (eyes.mouse == EyeXHost::Mouse::Off)
            ? EyeXHost::Mouse::Control
            : EyeXHost::Mouse::Off;
}


void MainWindow::on_hotkey_pressed() {

    eyes.mouse = (eyes.mouse == EyeXHost::Mouse::Off)
            ? EyeXHost::Mouse::Control
            : EyeXHost::Mouse::Off;
}

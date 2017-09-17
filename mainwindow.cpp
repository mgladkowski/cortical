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

    UpdateActivatableRegions();

}


MainWindow::~MainWindow() {

    delete ui;
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

    //qDebug() << bounds.left << " " << bounds.top << " " << bounds.right << " " << bounds.bottom;

    return bounds;
}


/* Reports the buttons as activatable regions to the EyeX host
 *
 */
void MainWindow::UpdateActivatableRegions() {

    std::vector<ActivatableRegion> regions;

    regions.push_back(ActivatableRegion(IDC_ACTIVATOR_BUTTON, GetScreenBounds(ui->buttonMain)));

    eyes.SetActivatableRegions(regions);
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

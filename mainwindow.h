#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <QDesktopWidget>
#include <QHotkey>
#include <QPushButton>
#include <QSize>
#include <QTimer>
#include <QLabel>
#include <windows.h>
#include "eyexhost.h"
#include "dialogmode.h"
#include "dialogeyex.h"
#include "dialogbci.h"


#define IDC_ACTIVATOR_BUTTON    101
#define IDC_MOUSE_BUTTON        102
#define IDC_BCI_BUTTON          103

#define IDC_BUTTON_MENU_MODE    201
#define IDC_BUTTON_MENU_EYEX    202
#define IDC_BUTTON_MENU_BCI     203


namespace Ui {
class MainWindow;
}


class MainWindow : public QMainWindow {

    Q_OBJECT


public:

    explicit    MainWindow(QWidget *parent = 0);
                ~MainWindow();
    bool        eventFilter(QObject *obj, QEvent *event);

    EyeXHost    eyes;

    void        on_hotkey_pressed();


private:

    Ui::MainWindow  *ui;

    DialogMode  *dialogMode;
    DialogEyeX  *dialogEyeX;
    DialogBci   *dialogBci;

    QTimer      progressTimer;
    QTimer      currentTimer;
    int         currentInteractor   = -1;
    int         intervalActivate    = 1000;
    int         intervalProgress    = 50;
    float       progressCounter     = 0.0;

    RECT        GetScreenBounds(QPushButton * button);
    void        UpdateActivatableRegions();
    void        ClearDelays();
    void        ToggleMouse();
    void        ToggleBrain();
    void        ToggleMenu();


signals:

    void        ButtonEnterEvent(QPushButton * button);
    void        ButtonLeaveEvent(QPushButton * button);


public slots:

    void        on_ActivationEvent(int interactorId);
    void        on_ActivationFocusEvent(int interactorId);


private slots:

    void        on_Progress();
    void        on_Timer();
    void        on_ButtonEnterEvent(QPushButton * button);
    void        on_ButtonLeaveEvent(QPushButton * button);
    void        on_buttonMain_clicked();
    void        on_buttonMouse_clicked();
    void        on_buttonBCI_clicked();
    void        on_buttonOpenBci_clicked();
    void        on_buttonEyeX_clicked();
    void        on_buttonMode_clicked();
};

#endif // MAINWINDOW_H

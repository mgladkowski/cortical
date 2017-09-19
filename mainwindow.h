#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <QDesktopWidget>
#include <QHotkey>
#include <QPushButton>
#include <QSize>
#include <QTimer>
#include <QThread>
#include <QLabel>
#include <windows.h>
#include "eyexhost.h"
#include "bcihost.h"



#define IDC_ACTIVATOR_BUTTON    101
#define IDC_MOUSE_BUTTON        102
#define IDC_BCI_BUTTON          103
#define IDC_BUTTON_MENU_MODE    201
#define IDC_BUTTON_MENU_EYEX    202
#define IDC_BUTTON_MENU_BCI     203
#define IDC_BUTTON_BCI_1        301
#define IDC_BUTTON_BCI_2        302
#define IDC_BUTTON_BCI_3        303
#define IDC_BUTTON_BCI_4        304
#define IDC_BUTTON_EYEX_1       401
#define IDC_BUTTON_EYEX_2       402
#define IDC_BUTTON_EYEX_3       403
#define IDC_BUTTON_EYEX_4       404
#define IDC_BUTTON_MODE_READ    501
#define IDC_BUTTON_MODE_QT      502
#define IDC_BUTTON_MODE_PHP     503
#define IDC_BUTTON_MODE_OFF     504


#define MENU_POSITION_1         20
#define LINE_POSITION_12        130
#define MENU_POSITION_2         240
#define LINE_POSITION_23        350
#define MENU_POSITION_3         460
#define LINE_POSITION_34        570
#define MENU_POSITION_4         680
#define LINE_POSITION_45        790
#define MENU_POSITION_5         900


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
    BciHost     brain;

    void        on_hotkey_pressed();


private:

    Ui::MainWindow  *ui;

    QTimer      progressTimer;
    QTimer      currentTimer;
    QTimer      suppressTimer;
    int         currentInteractor   = -1;
    int         intervalActivate    = 800;
    int         intervalProgress    = 40;
    int         intervalDebounce    = 600;
    float       progressCounter     = 0.0;

    bool        suppressEyeEvents   = false;

    RECT        GetScreenBounds(QPushButton * button);
    void        UpdateActivatableRegions();
    void        ClearDelays();
    void        ToggleMouse();
    void        ToggleBrain();
    void        ToggleMenu();
    void        SlideMenu( int position );
    void        GazeHover(QPushButton * button);
    void        ShowMenu(bool visible);
    void        ShowDialogMode(bool visible);
    void        ShowDialogEye(bool visible);
    void        ShowDialogBrain(bool visible);

    bool        IsVisibleMenu();
    bool        IsVisibleDialogMode();
    bool        IsVisibleDialogEye();
    bool        IsVisibleDialogBrain();

    void        SuppressEyeEvents( int msec );

signals:

    void        ButtonEnterEvent(QPushButton * button);
    void        ButtonLeaveEvent(QPushButton * button);


public slots:

    void        on_ActivationEvent(int interactorId);
    void        on_ActivationFocusEvent(int interactorId);


private slots:

    void        on_Progress();
    void        on_Timer();
    void        on_Suppress();
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

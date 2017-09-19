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
#include <QMessageBox>
#include <windows.h>
#include "eyexhost.h"
#include "bcihost.h"



#define IDC_ACTIVATOR_BUTTON    "buttonMain"
#define IDC_MOUSE_BUTTON        "buttonMouse"
#define IDC_BCI_BUTTON          "buttonBCI"
#define IDC_BUTTON_MENU_MODE    "buttonMode"
#define IDC_BUTTON_MENU_EYEX    "buttonEyeX"
#define IDC_BUTTON_MENU_BCI     "buttonOpenBci"
#define IDC_BUTTON_MODE_READ    "buttonModeRead"
#define IDC_BUTTON_MODE_QT      "buttonModeQt"
#define IDC_BUTTON_MODE_PHP     "buttonModePhp"
#define IDC_BUTTON_MODE_OFF     "buttonModeOff"
#define IDC_BUTTON_BCI_1        301
#define IDC_BUTTON_BCI_2        302
#define IDC_BUTTON_BCI_3        303
#define IDC_BUTTON_BCI_4        304
#define IDC_BUTTON_EYEX_1       401
#define IDC_BUTTON_EYEX_2       402
#define IDC_BUTTON_EYEX_3       403
#define IDC_BUTTON_EYEX_4       404


#define MENU_POSITION_1         90
#define LINE_POSITION_12        200
#define MENU_POSITION_2         310
#define LINE_POSITION_23        420
#define MENU_POSITION_3         530
#define LINE_POSITION_34        640
#define MENU_POSITION_4         750
#define LINE_POSITION_45        860
#define MENU_POSITION_5         970

#define INTERACTOR_BACK         "interactor_BACK"
#define INTERACTOR_PGUP         "interactor_PGUP"
#define INTERACTOR_PGDN         "interactor_PGDN"


namespace Ui {
class MainWindow;
}


class InteractorParam {
public:
    int         x;
    int         y;
    int         width;
    int         height;
    QString     name;
    QString     keypress;

    InteractorParam(int px, int py, int pwidth, int pheight, QString pname, QString pkeypress) {

        x       = px;
        y       = py;
        width   = pwidth;
        height  = pheight;
        name    = pname;
        keypress= pkeypress;
    }
};


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
    int         intervalInteractor  = 1200;
    int         intervalProgress    = 40;
    int         intervalDebounce    = 600;
    float       progressCounter     = 0.0;

    bool        suppressEyeEvents   = false;

    void        InitializeUi();

    void        SetInteractorProfile();
    void        ClearInteractorProfile();
    void        AddInteractor( InteractorParam data );
    RECT        GetScreenBounds(QPushButton * button);
    void        UpdateActivatableRegions();

    void        ClearDelays();
    void        SuppressEyeEvents( int msec );

    void        ToggleMouse();
    void        ToggleBrain();
    void        ToggleMenu();
    void        SlideMenu( int position );

    void        GazeHover(QPushButton * button);
    void        ClearHover();

    void        ShowMenu(bool visible);
    void        ShowDialogMode(bool visible);
    void        ShowDialogEye(bool visible);
    void        ShowDialogBrain(bool visible);

    bool        IsVisibleMenu();
    bool        IsVisibleDialogMode();
    bool        IsVisibleDialogEye();
    bool        IsVisibleDialogBrain();


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

    void        on_InteractorActivated();
    void        on_ThoughtActivated();

    void        on_buttonMain_clicked();
    void        on_buttonMouse_clicked();
    void        on_buttonBCI_clicked();
    void        on_buttonOpenBci_clicked();
    void        on_buttonEyeX_clicked();
    void        on_buttonMode_clicked();
    void on_buttonModeOff_clicked();
    void on_buttonModeRead_clicked();
};

#endif // MAINWINDOW_H

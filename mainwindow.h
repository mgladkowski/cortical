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
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>
#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <iostream>
#include <psapi.h>
#include "eyexhost.h"
#include "eyebutton.h"
#include "bcihost.h"


#define MENU_START              0
#define MENU_SPACE              260

#define INTERACTOR_BACK         "interactor_BACK"
#define INTERACTOR_PGUP         "interactor_PGUP"
#define INTERACTOR_PGDN         "interactor_PGDN"
#define INTERACTOR_NEWTAB       "interactor_NEWTAB"
#define INTERACTOR_CLSTAB       "interactor_CLSTAB"


namespace Ui {
class MainWindow;
}


class InteractorParam {
public:
    int             x;
    int             y;
    int             width;
    int             height;
    QString         name;
    ActivatorFlags  flags;

    InteractorParam(int px, int py, int pwidth, int pheight, QString pname, ActivatorFlags pflags) {

        x           = px;
        y           = py;
        width       = pwidth;
        height      = pheight;
        name        = pname;
        flags       = pflags;
    }
};


class MainWindow : public QMainWindow {

    Q_OBJECT


public:

    explicit    MainWindow(QWidget *parent = 0);
                ~MainWindow();

    EyeXHost    eyes;

    BciHost     brain;

    void        on_hotkey_pressed();


private:

    Ui::MainWindow  *ui;

    QTimer      systemTimer;
    QTimer      suppressTimer;
    bool        suppressEyes = false;

    HWND        focusedWindow;
    QString     focusedExecutable;
    bool        focusedIsFullscreen;

    bool        isProcessFullscreen( HWND window );

    QString     GetProcessName( HWND handle );
    void        UpdateFocusedProcess();

    void        InitializeUi();

    void        AddInteractor( InteractorParam data );
    void        SetInteractorProfile( int profileId );
    void        ClearInteractorProfile();

    RECT        GetScreenBounds( EyeButton * button );
    void        UpdateActivatableRegions();
    void        SuppressEyes( int msec );

    void        ToggleMouse();
    void        ToggleBrain();
    void        ToggleMenu();

    void        SlideMenu( int position );
    void        ShowMenu( bool visible );
    void        ShowOp( bool visible );
    void        ShowEye( bool visible );
    void        ShowBci( bool visible );


    bool        isVisibleMenu();
    bool        isVisibleOp();
    bool        isVisibleEye();
    bool        isVisibleBci();


signals:

public slots:

    void        on_ActivationEvent( int interactorId );
    void        on_AnimationFinished();
    void        on_FadeMenuFinished();
    void        on_FadeOpsFinished();
    void        on_FadeEyeFinished();
    void        on_FadeBciFinished();
    void        on_SystemTimer();

private slots:

    void        on_InteractorActivated();
    void        on_ThoughtActivated();
    void        on_SuppressFinished();

    void        on_buttonMouse_clicked();
    void        on_buttonMain_clicked();

    void        on_buttonOp_clicked();
    void        on_buttonOp1_clicked();
    void        on_buttonOp2_clicked();
    void        on_buttonOp3_clicked();
    void        on_buttonOp4_clicked();
    void        on_buttonEye_clicked();
    void        on_buttonBci_clicked();
};

#endif // MAINWINDOW_H

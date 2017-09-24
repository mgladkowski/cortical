#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThread>
#include <QHotkey>
#include <QDebug>
#include <QDesktopWidget>
#include <QPushButton>
#include <QSize>
#include <QTimer>
#include <QLabel>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>
#include <QFontDatabase>
#include <QFont>
#include <windows.h>
#include <psapi.h>
#include "glyphicons.h"
#include "eyexhost.h"
#include "eyebutton.h"
#include "bcihost.h"
#include "interactor.h"


#define MENU_START              0
#define MENU_SPACE              260

#define ITK_ALT_LEFT            "ITK_ALT_LEFT"
#define ITK_PAGEUP              "ITK_PAGEUP"
#define ITK_PAGEDN              "ITK_PAGEDN"
#define ITK_CTRL_T              "ITK_CTRL_T"
#define ITK_CTRL_T_TAB_F4       "ITK_CTRL_T_TAB_F4"
#define ITK_ESC                 "ITK_ESC"
#define ITK_SPACE               "ITK_SPACE"
#define ITK_N                   "ITK_N"
#define ITK_P                   "ITK_P"
#define ITK_F                   "ITK_F"

#define ITR_PROFILE_NONE        0
#define ITR_PROFILE_EXPLORER    1
#define ITR_PROFILE_BROWSER     2
#define ITR_PROFILE_BROWSER_FS  3
#define ITR_PROFILE_VLC         4
#define ITR_PROFILE_VLC_FS      5
#define ITR_PROFILE_DEV         6


namespace Ui {
class MainWindow;
}


class MainWindow : public QMainWindow {

    Q_OBJECT


public:

    explicit    MainWindow(QWidget *parent = 0);
                ~MainWindow();

    EyeXHost    eyes;
    BciHost     brain;

    enum InteractorPreset {
        STYLE_MAIN = 1,
        STYLE_MENU,
        STYLE_BUTTON,
        STYLE_INTERACTOR,
        STYLE_INVISIBLE,
        STYLE_SUCCESS,
        STYLE_WARNING,
        STYLE_DANGER,
    };

    void        on_hotkey_pressed();


private:

    Ui::MainWindow  *ui;

    QFont       iconFont;
    QTimer      systemTimer;
    QTimer      suppressTimer;
    bool        suppressEyes = false;

    int         currentProfile;
    HWND        focusedWindow;
    QString     focusedExecutable;
    bool        focusedIsFullscreen;

    void        InitializeUi();

    bool        isProcessFullscreen( HWND window );
    QString     GetProcessName( HWND handle );
    void        UpdateFocusedProcess();

    void        AddInteractor( Interactor data );
    void        SetInteractorProfile( int profileId );
    void        ClearInteractorProfile();
    Interactor::Params GetPresetInteractor( int styleId );

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

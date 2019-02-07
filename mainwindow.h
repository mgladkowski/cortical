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
#include "enums.h"
#include "bcihost.h"
#include "eyexhost.h"
#include "eyebutton.h"
#include "interactor.h"
#include "glyphicons.h"
#include "gazepoint.h"
#include "heatmap.h"

#define MENU_START              0
#define MENU_SPACE              260
#define HEATMAP_INITIAL_X       1700
#define HEATMAP_INITIAL_Y       900

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {

    Q_OBJECT


public:

    explicit    MainWindow(QWidget *parent = nullptr);
                ~MainWindow();

    EyeXHost    eyes;
    BciHost     brain;

    void        on_hotkey_pressed( const char * key );


private:

    Ui::MainWindow  *ui;

    HeatMap     *heatmap;
    GazePoint   *gazepoint;

    QFont       iconFont;
    QTimer      systemTimer;
    QTimer      suppressTimer;
    bool        suppressEyes = false;

    int         currentProfile;
    HWND        focusedWindow;
    QString     focusedExecutable;
    bool        focusedIsFullscreen;
    bool        userPresent;

    void        InitializeUi();
    QString     GetProcessName( HWND handle );
    bool        isProcessFullscreen( HWND window );
    void        UpdateFocusedProcess();
    void        ClearInteractorProfile();
    void        ShowInteractorProfile( bool visible );
    void        AddInteractor( Interactor data );
    void        SetInteractorProfile( int profileId );
    Interactor::Params GetPresetInteractor( int styleId );
    RECT        GetScreenBounds( EyeButton * button );
    void        UpdateActivatableRegions();
    void        SuppressEyes( int msec );

    void        ToggleMouse();
    void        ToggleGaze();
    void        ToggleBrain();
    void        ToggleMenu();

    void        ShowAll( bool visible );
    void        SlideMenu( int position );
    void        ShowMenu( bool visible );
    void        ShowOp( bool visible );
    void        ShowEye( bool visible );
    void        ShowBci( bool visible );

    bool        isVisibleApp();
    bool        isVisibleMenu();
    bool        isVisibleOp();
    bool        isVisibleEye();
    bool        isVisibleBci();


signals:

public slots:

    void        on_ActivationEvent( int interactorId );
    void        on_AnimationFinished();
    void        on_FadeAllFinished();
    void        on_FadeMenuFinished();
    void        on_FadeOpsFinished();
    void        on_FadeEyeFinished();
    void        on_FadeBciFinished();
    void        on_SystemTimer();
    void        on_UserPresenceChanged( bool present );
    void        on_gazeEvent( int X, int Y );
    void        on_eegEvent( double packet[4] );
    void        on_fftEvent( double packet[125] );


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

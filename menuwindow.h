#ifndef MENUWINDOW_H
#define MENUWINDOW_H

#include <QDialog>
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
#include "mainwindow.h"


#define IDC_BUTTON_MENU_MODE    201
#define IDC_BUTTON_MENU_EYEX    202
#define IDC_BUTTON_MENU_BCI     203


namespace Ui {
class MenuWindow;
}

class MainWindow;

class MenuWindow : public QDialog {

    Q_OBJECT


public:

    explicit    MenuWindow(QWidget *parent = 0);
                ~MenuWindow();
    bool        eventFilter(QObject *obj, QEvent *event);

    MainWindow  *mainWindow;

    void        UpdateActivatableRegions(std::vector<ActivatableRegion> & regions);


private:

    Ui::MenuWindow *ui;

    DialogMode  *dialogMode;
    DialogEyeX  *dialogEyeX;
    DialogBci   *dialogBci;

    QTimer      progressTimer;
    QTimer      currentTimer;
    int         currentInteractor   = -1;
    int         intervalActivate    = 1200;
    int         intervalProgress    = 60;
    float       progressCounter     = 0.0;

    RECT        GetScreenBounds(QPushButton * button);
    void        ClearDelays();


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
    void        on_buttonMode_clicked();
    void        on_buttonEyeX_clicked();
    void        on_buttonOpenBci_clicked();
    void        on_MenuWindow_finished(int result);

};

#endif // MENUWINDOW_H

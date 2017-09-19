#ifndef DIALOGMODE_H
#define DIALOGMODE_H

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


#define IDC_BUTTON_MODE_READ    501
#define IDC_BUTTON_MODE_QT      502
#define IDC_BUTTON_MODE_PHP     503
#define IDC_BUTTON_MODE_OFF     504


namespace Ui {
class DialogMode;
}


class DialogMode : public QDialog {

    Q_OBJECT


public:

    explicit    DialogMode(QWidget *parent = 0);
                ~DialogMode();
    bool        eventFilter(QObject *obj, QEvent *event);

    EyeXHost    *eyes = 0;
    void        UpdateActivatableRegions(std::vector<ActivatableRegion> & regions);

private:

    Ui::DialogMode *ui;

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

};

#endif // DIALOGMODE_H

#ifndef DIALOGEYEX_H
#define DIALOGEYEX_H

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


#define IDC_BUTTON_EYEX_1        401
#define IDC_BUTTON_EYEX_2        402
#define IDC_BUTTON_EYEX_3        403
#define IDC_BUTTON_EYEX_4        404


namespace Ui {
class DialogEyeX;
}


class DialogEyeX : public QDialog {

    Q_OBJECT


public:

    explicit    DialogEyeX(QWidget *parent = 0);
                ~DialogEyeX();
    bool        eventFilter(QObject *obj, QEvent *event);

    EyeXHost    *eyes = 0;
    void        UpdateActivatableRegions(std::vector<ActivatableRegion> & regions);


private:

    Ui::DialogEyeX *ui;

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

#endif // DIALOGEYEX_H

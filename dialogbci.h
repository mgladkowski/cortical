#ifndef DIALOGBCI_H
#define DIALOGBCI_H

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


#define IDC_BUTTON_BCI_1        301
#define IDC_BUTTON_BCI_2        302
#define IDC_BUTTON_BCI_3        303
#define IDC_BUTTON_BCI_4        304


namespace Ui {
class DialogBci;
}


class DialogBci : public QDialog {

    Q_OBJECT


public:

    explicit    DialogBci(QWidget *parent = 0);
                ~DialogBci();
    bool        eventFilter(QObject *obj, QEvent *event);

    EyeXHost    *eyes = 0;
    void        UpdateActivatableRegions(std::vector<ActivatableRegion> & regions);


private:

    Ui::DialogBci *ui;

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

#endif // DIALOGBCI_H

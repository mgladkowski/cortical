#ifndef EYEBUTTON_H
#define EYEBUTTON_H

#include <QObject>
#include <QWidget>
#include <QDebug>
#include <QPushButton>
#include <QTimer>
#include "interactor.h"

class EyeButton : public QPushButton {

    Q_OBJECT


public:

    explicit    EyeButton(QWidget *parent = 0);

    bool        isHovered;
    bool        isEyeHovered;
    bool        isActivated;
    bool        isInteractor;
    bool        showHover;
    bool        showEyeHover;
    bool        showProgress;
    bool        suppressEyes;
    bool        clickOnActivation;
    bool        suspendOnActivation;
    bool        toggleActivation;
    QString     styleDefault;
    QString     styleHover;
    QString     styleFixate;
    QString     styleActive;
    QString     styleCurrent;
    int         msecActivate;
    int         msecRecovery;

    void        setProperties( Interactor data );
    void        setStyle( Interactor::Params data );


protected:

    virtual void enterEvent(QEvent *) {

        emit MouseEnterEvent();
        emit EnterEvent();
    }

    virtual void leaveEvent(QEvent *) {

        emit MouseLeaveEvent();
        emit LeaveEvent();
    }


private:

    QTimer      timerActivate;
    QTimer      timerRecovery;
    QTimer      timerProgress;
    float       progressCounter;


    void        UpdateStyleSheet();
    void        UpdateStyleState();

    void        stopTimers();

signals:

    void        EnterEvent();
    void        LeaveEvent();
    void        MouseEnterEvent();
    void        MouseLeaveEvent();
    void        EyeEnterEvent();
    void        EyeLeaveEvent();

    void        FixationActivated( int interactorId );
    void        ActivationEvent( int interactorId );
    void        SuspendEvent( int interactorId );

public slots:

    void        on_ActivationEvent( int interactorId );
    void        on_ActivationFocusEvent( int interactorId );

private slots:

    void        on_Timer_Progress();
    void        on_Timer_Activate();
    void        on_Timer_Recovery();

    void        on_EnterEvent();
    void        on_LeaveEvent();
    void        on_MouseEnterEvent();
    void        on_MouseLeaveEvent();
    void        on_EyeEnterEvent();
    void        on_EyeLeaveEvent();

};

#endif // EYEBUTTON_H

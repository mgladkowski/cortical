#include "eyebutton.h"


EyeButton::EyeButton(QWidget *parent) : QPushButton(parent) {


    isHovered           = false;
    isEyeHovered        = false;
    isActivated         = false;

    isInteractor        = false;
    showHover           = true;
    showEyeHover        = true;
    showProgress        = true;
    suppressEyes        = false;

    progressCounter     = 0.0;
    msecActivate        = 0;
    msecRecovery        = 0;
    clickOnActivation   = true;
    suspendOnActivation = true;
    toggleActivation    = false;

    styleDefault   = "background:rgba(0,0,0,0.4);border:1px solid rgba(0,0,0,0.3);";
    styleHover     = "background:rgba(41,182,246,0.2);border:3px solid rgba(41,182,246,0.6);";
    styleFixate    = "background: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop: 0 rgba(41,182,246,0.2), stop: %1 rgba(0,0,0,0.2) );border:3px solid rgba(41,182,246,0.6);";
    styleActive    = "background:rgba(129,212,250,0.2);border:3px solid rgba(41,182,246,0.6);";

    UpdateStyleState();

    QObject::connect(
                this, SIGNAL(EnterEvent()),
                this, SLOT(on_EnterEvent())
    );
    QObject::connect(
                this, SIGNAL(LeaveEvent()),
                this, SLOT(on_LeaveEvent())
    );
    QObject::connect(
                this, SIGNAL(MouseEnterEvent()),
                this, SLOT(on_MouseEnterEvent())
    );
    QObject::connect(
                this, SIGNAL(MouseLeaveEvent()),
                this, SLOT(on_MouseLeaveEvent())
    );
    QObject::connect(
                this, SIGNAL(EyeEnterEvent()),
                this, SLOT(on_EyeEnterEvent())
    );
    QObject::connect(
                this, SIGNAL(EyeLeaveEvent()),
                this, SLOT(on_EyeLeaveEvent())
    );

    QObject::connect(
                &timerProgress, SIGNAL(timeout()),
                this, SLOT(on_Timer_Progress())
    );
    QObject::connect(
                &timerActivate, SIGNAL(timeout()),
                this, SLOT(on_Timer_Activate())
    );
    QObject::connect(
                &timerRecovery, SIGNAL(timeout()),
                this, SLOT(on_Timer_Recovery())
    );
    setParent(parent);
}


void EyeButton::UpdateStyleSheet() {

    setStyleSheet( styleCurrent );
    update();
}


void EyeButton::UpdateStyleState() {

    styleCurrent = styleDefault;

    styleCurrent = (isActivated)
            ? styleActive
            : styleCurrent;

    styleCurrent = ((showHover && isHovered))
            ? styleHover
            : styleCurrent;

    UpdateStyleSheet();
}


void EyeButton::on_ActivationEvent( int interactorId ) {

    if (suppressEyes) return;

    if ( (WId)interactorId == this->winId() ) {

        emit ActivationEvent( interactorId );

        if (clickOnActivation) this->click();

        if (toggleActivation) isActivated = !isActivated;

        stopTimers();

        if (msecRecovery > 0) {

            suppressEyes = true;

            timerRecovery.start( msecRecovery );

            if (suspendOnActivation) emit SuspendEvent( interactorId );
        }
    }
}


void EyeButton::on_ActivationFocusEvent( int interactorId ) {

    if (isEyeHovered && interactorId == -1) {

        emit EyeLeaveEvent();

    } else if ( (WId)interactorId == this->winId() ) {

        if (suppressEyes) return;

        emit EyeEnterEvent();

        if (msecActivate > 0) {

            int slice = msecActivate / 20;
            if (slice < 1) slice = 1;

            timerActivate.start( msecActivate );

            if (showProgress)
            timerProgress.start( slice );

        } else if (msecActivate == 0) {

            on_ActivationEvent( interactorId );
        }

        UpdateStyleState();
    }
}


void EyeButton::on_Timer_Progress() {

    progressCounter = progressCounter + 0.05;
    if (progressCounter > 1) progressCounter = 1;

    styleCurrent = styleFixate.arg(QString::number( progressCounter ));
    UpdateStyleSheet();
}


void EyeButton::on_Timer_Activate() {

    on_ActivationEvent( (int)this->winId() );
}


void EyeButton::on_Timer_Recovery() {

    timerRecovery.stop();
    suppressEyes = false;
}


void EyeButton::stopTimers() {

    timerProgress.stop();
    timerActivate.stop();
    progressCounter = 0;

    UpdateStyleState();
}


void EyeButton::setProperties( Interactor data ) {

    setObjectName( data.name );
    setText( data.icon );
    setGeometry( data.x, data.y, data.width, data.height );
    setStyle( data.params );
}


void EyeButton::setStyle( Interactor::Params params ) {

    setFocusPolicy(Qt::NoFocus);

    isInteractor        = params.isInteractor;
    showHover           = params.showHover;
    showEyeHover        = params.showEyeHover;
    showProgress        = params.showProgress;
    suppressEyes        = params.suppressEyes;
    clickOnActivation   = params.clickOnActivation;
    suspendOnActivation = params.suspendOnActivation;
    toggleActivation    = params.toggleActivation;
    msecActivate        = params.msecActivate;
    msecRecovery        = params.msecRecovery;
    styleDefault        = params.styleDefault;
    styleFixate         = params.styleFixate;
    styleHover          = params.styleHover;
    styleActive         = params.styleActive;

    UpdateStyleState();
}


void EyeButton::on_EnterEvent() {
}


void EyeButton::on_LeaveEvent() {
}


void EyeButton::on_MouseEnterEvent() {

    isHovered = true;

    UpdateStyleState();

    if (!isEyeHovered) emit EnterEvent();
}


void EyeButton::on_MouseLeaveEvent() {

    isHovered = false;

    UpdateStyleState();

    if (!isEyeHovered) emit LeaveEvent();
}


void EyeButton::on_EyeEnterEvent() {

    isEyeHovered = true;

    UpdateStyleState();

    if (!isHovered) emit EnterEvent();
}


void EyeButton::on_EyeLeaveEvent() {

    isEyeHovered = false;

    stopTimers();

    if (!isHovered) emit LeaveEvent();
}

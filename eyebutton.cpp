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

    defaultBackground   = "background:rgba(0,0,0,0.2);";
    defaultBorder       = "border:1px solid rgba(0,0,0,0.3);";
    hoverBackground     = "background:rgba(41,182,246,0.2);";
    hoverBorder         = "border:3px solid rgba(41,182,246,0.6);";
    fixateBorder        = "border:3px solid rgba(41,182,246,0.6);";
    fixateBackground    = "background: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop: 0 rgba(0,255,0,0.2), stop: %1 rgba(0,0,0,0.2) );";
    activeBackground    = "background:rgba(129,212,250,0.2);";
    activeBorder        = "border:3px solid rgba(41,182,246,0.6);";

    currentBackground   = defaultBackground;
    currentBorder       = defaultBorder;

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

}


void EyeButton::UpdateStyleSheet() {

    setStyleSheet( currentBackground + currentBorder);
    update();
}


void EyeButton::UpdateStyleState() {

    currentBackground = (isActivated) ? activeBackground : defaultBackground;
    currentBorder = (isActivated) ? activeBorder : defaultBorder;
    currentBackground = (isHovered || isEyeHovered) ? hoverBackground : currentBackground;
    currentBorder = (isHovered || isEyeHovered) ? hoverBorder : currentBorder;
    UpdateStyleSheet();
}


void EyeButton::on_ActivationEvent( int interactorId ) {

    if (suppressEyes) return;

    if ( (WId)interactorId == this->winId() ) {

        emit ActivationEvent( interactorId );

        if (clickOnActivation) this->click();

        if (toggleActivation) isActivated = !isActivated;

        stopTimers();
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

            emit ActivationEvent( interactorId );
            qDebug() << "ActivationFocusEvent emitted ActivationEvent";

            if (clickOnActivation == true) this->click();

            if (toggleActivation) isActivated = !isActivated;

        }

        UpdateStyleState();
    }
}


void EyeButton::on_Timer_Progress() {

    progressCounter = progressCounter + 0.05;
    if (progressCounter > 1) progressCounter = 1;

    currentBorder = fixateBorder;
    currentBackground = fixateBackground.arg(QString::number( progressCounter ));

    UpdateStyleSheet();
}


void EyeButton::on_Timer_Activate() {

    on_ActivationEvent( (int)this->winId() );

    if (msecRecovery > 0) {

        suppressEyes = true;
        timerRecovery.start( msecRecovery );

        if (suspendOnActivation)
        emit SuspendEvent( (int)this->winId() );
    }
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


void EyeButton::setActivationType( ActivatorFlags flags ) {

    if (flags & ActivatorFlags::ACTIVATE_QUICK) {
        msecActivate = 300;
        msecRecovery = 300;
    }
    if (flags & ActivatorFlags::ACTIVATE_NORMAL) {
        msecActivate = 1200;
        msecRecovery = 500;
    }
    if (flags & ActivatorFlags::ACTIVATE_SLOW) {
        msecActivate = 3000;
        msecRecovery = 500;
    }
    if (flags & ActivatorFlags::ACTIVATE_INSTANT) {
        msecActivate = 100;
        msecRecovery = 500;
    }
    if (flags & ActivatorFlags::HIDE_PROGRESS_BAR) {
        showProgress = false;
    }
    if (flags & ActivatorFlags::INTERACTOR_DEFAULT) {

        isInteractor        = true;
        showHover           = false;
        showEyeHover        = true;
        defaultBackground   = "background:rgba(0,0,0,0.1);";
        defaultBorder       = "border:1px solid rgba(0,0,0,0);";
        fixateBorder        = "border:0px solid rgba(0,0,0,0);";
        fixateBackground    = "background: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop: 0 rgba(41,182,246,0.1), stop: %1 rgba(0,0,0,0.1) );";
        hoverBorder         = "border:0px solid rgba(0,0,0,0);";
        hoverBackground     = "background:rgba(41,182,246,0.1);";
        activeBackground    = "background:rgba(129,212,250,0.1);";
        activeBorder        = "border:3px solid rgba(41,182,246,0.1);";
    }
    if (flags & ActivatorFlags::INTERACTOR_DANGER) {

        isInteractor        = true;
        showHover           = false;
        showEyeHover        = true;
        defaultBackground   = "background:rgba(0,0,0,0.1);";
        defaultBorder       = "border:1px solid rgba(0,0,0,0);";
        fixateBorder        = "border:1px solid rgba(255,0,0,0.2);";
        fixateBackground    = "background: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop: 0 rgba(255,0,0,0.2), stop: %1 rgba(0,0,0,0.1) );";
        hoverBorder         = "border:3px solid rgba(255,0,0,0.4);";
        hoverBackground     = "background:rgba(41,182,246,0.1);";
        activeBackground    = "background:rgba(129,212,250,0.1);";
        activeBorder        = "border:3px solid rgba(41,182,246,0.1);";
    }
}


void EyeButton::setHoverStyleSheet( QString newBg, QString newBorder ) {

    hoverBackground = newBg;
    hoverBorder     = newBorder;
}


void EyeButton::on_EnterEvent() {
}


void EyeButton::on_LeaveEvent() {

    currentBackground = (isActivated) ? activeBackground : defaultBackground;
    currentBorder = (isActivated) ? activeBorder : defaultBorder;
    UpdateStyleSheet();
}


void EyeButton::on_MouseEnterEvent() {

    isHovered = true;

    if (showHover) {
        currentBackground = hoverBackground;
        currentBorder = hoverBorder;
        UpdateStyleSheet();
    }

    if (!isEyeHovered) emit EnterEvent();
}


void EyeButton::on_MouseLeaveEvent() {

    isHovered = false;

    if (!isEyeHovered) emit LeaveEvent();
}


void EyeButton::on_EyeEnterEvent() {

    isEyeHovered = true;

    if (showEyeHover) {
        currentBackground = hoverBackground;
        currentBorder = hoverBorder;
        UpdateStyleSheet();
    }

    if (!isHovered) emit EnterEvent();
}


void EyeButton::on_EyeLeaveEvent() {

    isEyeHovered = false;

    stopTimers();

    if (!isHovered) emit LeaveEvent();
}

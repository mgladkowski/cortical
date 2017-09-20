#ifndef EYEBUTTON_H
#define EYEBUTTON_H

#include <QObject>
#include <QWidget>
#include <QDebug>
#include <QPushButton>
#include <QTimer>


enum ActivatorFlags {

    ACTIVATE_INSTANT                = (1<<0),
    ACTIVATE_QUICK                  = (1<<1),
    ACTIVATE_NORMAL                 = (1<<2),
    ACTIVATE_SLOW                   = (1<<3),
    HIDE_PROGRESS_BAR               = (1<<4),
    VERTICAL_PROGRESS_BAR           = (1<<5),
    INTERACTOR_DEFAULT              = (1<<6),
    INTERACTOR_DANGER               = (1<<7),
};
inline ActivatorFlags operator|(ActivatorFlags a, ActivatorFlags b) {
    return static_cast<ActivatorFlags>(static_cast<int>(a) | static_cast<int>(b));
}


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

    int         msecActivate;
    int         msecRecovery;

    void        setActivationType( ActivatorFlags flags );
    void        setHoverStyleSheet( QString newBg, QString newBorder );

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

    QString     defaultBackground;
    QString     defaultBorder;
    QString     hoverBackground;
    QString     hoverBorder;
    QString     fixateBackground;
    QString     fixateBorder;
    QString     activeBackground;
    QString     activeBorder;
    QString     currentBackground;
    QString     currentBorder;

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

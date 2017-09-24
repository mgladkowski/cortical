#ifndef INTERACTOR_H
#define INTERACTOR_H

#include <QString>

class Interactor {
public:

    struct Params {
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
        QString     styleDefault;
        QString     styleHover;
        QString     styleFixate;
        QString     styleActive;
    };

    int             x;
    int             y;
    int             width;
    int             height;
    QString         name;
    QString         icon;
    Params          params;

    Interactor(int _x, int _y, int _w, int _h, QString _name, QString _icon, Params _params) :
        x(_x),
        y(_y),
        width(_w),
        height(_h),
        name(_name),
        icon(_icon),
        params(_params){}

};

#endif // INTERACTOR_H

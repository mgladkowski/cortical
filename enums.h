#ifndef ENUMS_H
#define ENUMS_H

#define ITM_APPLICATION         "ITM_APPLICATION"
#define ITK_ALT_LEFT            "ITK_ALT_LEFT"
#define ITK_PAGEUP              "ITK_PAGEUP"
#define ITK_PAGEDN              "ITK_PAGEDN"
#define ITK_CTRL_T              "ITK_CTRL_T"
#define ITK_CTRL_F4             "ITK_CTRL_F4"
#define ITK_CTRL_ALT_BREAK      "ITK_CTRL_ALT_BREAK"
#define ITK_CTRL_HOME_WIN_DOWN  "ITK_CTRL_HOME_WIN_DOWN"
#define ITK_ESC                 "ITK_ESC"
#define ITK_SPACE               "ITK_SPACE"
#define ITK_N                   "ITK_N"
#define ITK_P                   "ITK_P"
#define ITK_F                   "ITK_F"

#define ITP_NONE                0
#define ITP_EXPLORER            1
#define ITP_BROWSER             2
#define ITP_BROWSER_FS          3
#define ITP_VLC                 4
#define ITP_VLC_FS              5
#define ITP_DEV                 6
#define ITP_RDP                 7
#define ITP_RDP_FS              8

enum InteractorPreset {
    STYLE_MAIN = 1,
    STYLE_MENU,
    STYLE_BUTTON,
    STYLE_INTERACTOR,
    STYLE_INVISIBLE,
    STYLE_SUCCESS,
    STYLE_WARNING,
    STYLE_DANGER
};

#endif // ENUMS_H

CONFIG += C++11

HEADERS += $$PWD/qhotkey/qhotkey.h \
        $$PWD/qhotkey/qhotkey_p.h

SOURCES += $$PWD/qhotkey/qhotkey.cpp

mac {
        SOURCES += $$PWD/qhotkey/qhotkey_mac.cpp
	LIBS += -framework Carbon
} else:win32 {
        SOURCES += $$PWD/qhotkey/qhotkey_win.cpp
	LIBS += -lUser32
} else:unix {
        SOURCES += $$PWD/qhotkey/qhotkey_x11.cpp
	QT += x11extras
	LIBS += -lX11
}

INCLUDEPATH += $$PWD/qhotkey

DISTFILES += \
    $$PWD/qhotkey/qhotkey.pri

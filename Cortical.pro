#-------------------------------------------------
#
# Project created by QtCreator 2017-09-13T12:34:49
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

QMAKE_CXXFLAGS_WARN_ON += -Wno-reorder
QMAKE_CXXFLAGS_WARN_ON += -Wno-unused-variable
QMAKE_CXXFLAGS_WARN_ON += -Wno-unused-parameter
QMAKE_CXXFLAGS_WARN_ON += -Wno-unused-but-set-variable
QMAKE_CXXFLAGS_WARN_ON += -Wno-missing-field-initializers

TARGET = Cortical

TEMPLATE = app

LIBS += D:/Lab/Qt/projects/Cortical/lib/x64/Tobii.EyeX.Client.lib
LIBS += D:/Lab/Qt/projects/Cortical/lib/x64/Gdi32.Lib
LIBS += D:/Lab/Qt/projects/Cortical/lib/x64/gdiplus.lib
LIBS += D:/Lab/Qt/projects/Cortical/lib/x64/Psapi.Lib


include(qhotkey.pri)

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        mainwindow.cpp \
    eyexhost.cpp \
    bcihost.cpp \
    eyebutton.cpp

HEADERS += \
        mainwindow.h \
    eyex/EyeX.h \
    eyexhost.h \
    bcihost.h \
    eyebutton.h \
    glyphicons.h \
    interactor.h \
    enums.h

FORMS += \
        mainwindow.ui

RESOURCES += \
    icons.qrc

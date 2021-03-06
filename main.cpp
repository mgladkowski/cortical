#include "mainwindow.h"
#include <QApplication>
#include <QObject>
#include <QHotkey>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

    a.setApplicationName("Telepath");
    a.setOrganizationName("Mike Gladkowski");
    a.setOrganizationDomain("http://northportage.ca");

    QHotkey hk1(QKeySequence("F1"), true);
    QObject::connect(&hk1, &QHotkey::activated, qApp, [&](){ w.on_hotkey_pressed("F1"); });

//    QHotkey hk2(QKeySequence(" "), true);
//    QObject::connect(&hk2, &QHotkey::activated, qApp, [&](){ w.on_hotkey_pressed("SPACE"); });

    w.showFullScreen();

    return a.exec();
}

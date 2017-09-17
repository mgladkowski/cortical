#include "mainwindow.h"
#include <QApplication>
#include <QObject>
#include <QVector>
#include <QHotkey>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

    QHotkey hotkey(QKeySequence("F1"), true);
    QObject::connect(&hotkey, &QHotkey::activated, qApp, [&](){ w.on_hotkey_pressed(); });

    w.show();

    return a.exec();
}

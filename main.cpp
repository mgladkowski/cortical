#include "mainwindow.h"
#include <QApplication>
#include <QObject>
#include <QVector>
#include <QHotkey>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

    w.show();

    return a.exec();
}

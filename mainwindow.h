#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <QDesktopWidget>
#include <QHotkey>
#include <QPainter>
#include <windows.h>
#include <wingdi.h>
#include "eyexhost.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {

    Q_OBJECT


public:

    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    EyeXHost eyes;

    void    on_hotkey_pressed();


private:

    Ui::MainWindow *ui;


public slots:

    void    on_buttonMain_clicked();


};

#endif // MAINWINDOW_H

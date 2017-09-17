#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <QDesktopWidget>
#include <QHotkey>
#include <QPushButton>
#include <QSize>
#include <windows.h>
#include "eyexhost.h"

#define IDC_ACTIVATOR_BUTTON    101

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

    RECT    GetScreenBounds(QPushButton * button);
    void    UpdateActivatableRegions();

public slots:

    void    on_buttonMain_clicked();


};

#endif // MAINWINDOW_H

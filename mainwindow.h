#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <QDesktopWidget>
#include <windows.h>
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

private:

    Ui::MainWindow *ui;

    bool    eyeMouse;
    int     gazeX;
    int     gazeY;
    int     emaX;
    int     emaY;
    int     ema_size;
    float   ema_multiplier;


public slots:

    void    on_gaze_event(int X, int Y);
    void    on_buttonMain_clicked();

};

#endif // MAINWINDOW_H

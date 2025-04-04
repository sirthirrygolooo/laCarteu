// mainwindow.h
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "mapdisplay.h"
#include "eventmanager.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);

private:
    MapDisplay *mapDisplay;
    EventManager *eventManager;
};

#endif // MAINWINDOW_H

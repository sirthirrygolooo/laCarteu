// mainwindow.cpp
#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent) {
    mapDisplay = new MapDisplay(this);
    setCentralWidget(mapDisplay);

    eventManager = new EventManager(mapDisplay, this);

    // Initialiser la carte avec une position et un niveau de zoom
    mapDisplay->setCenter(6.03333, 47.25);
    mapDisplay->setZoom(12);
}

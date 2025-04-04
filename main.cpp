#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    qDebug() << "Application started";
    MainWindow mainWindow;
    mainWindow.show();
    qDebug() << "MainWindow shown";
    return app.exec();
}

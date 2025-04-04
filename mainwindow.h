// mainwindow.h
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QScopedPointer>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QMap>
#include <QListWidgetItem>
#include <QTimer>
#include "mapwidget.h"

class QMenu;
class QGroupBox;
class QPushButton;
class QLineEdit;
class QListWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT

    QMenu *_file_menu;
    QMenu *_help_menu;
    QScopedPointer<QGroupBox> _main_widget;
    QScopedPointer<QPushButton> _button;
    QScopedPointer<QLineEdit> _text_edit;
    QScopedPointer<QListWidget> _list;
    QScopedPointer<QNetworkAccessManager> _network_manager;
    QScopedPointer<QTimer> _search_timer;

    QMap<QString, QPointF> _places_map;
    MapWidget *mapWidget;

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onButtonClicked();
    void onNetworkReply(QNetworkReply *reply);
    void onPlaceSelected(QListWidgetItem *item);
    void onQuitClicked();
    void onAboutClicked();
    void onTextChanged(const QString &text);
    void performSearch();

private:
    void updateStatusBar(double lon, double lat);
};

#endif // MAINWINDOW_H

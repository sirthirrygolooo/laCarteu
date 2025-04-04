// mainwindow.cpp
#include "mainwindow.h"
#include "mapwidget.h"
#include <QMenuBar>
#include <QGroupBox>
#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QListWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPixmap>
#include <QDate>
#include <QMessageBox>
#include <QApplication>
#include <QNetworkRequest>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QStatusBar>

using namespace std;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), _network_manager(new QNetworkAccessManager(this)), _search_timer(new QTimer(this)) {

    setWindowTitle(QString{"La Carteuuuu"});

    _file_menu = menuBar()->addMenu(QString{tr("&Fichier")});
    _help_menu = menuBar()->addMenu(QString{tr("&Aide")});

    _file_menu->addAction(QString{"Preferences"});
    QAction *quitAction = _file_menu->addAction(QString{"Quitter"});
    _help_menu->addAction(QString{"leManueleu"});
    QAction *aboutAction = _help_menu->addAction(QString{"A propos"});

    connect(quitAction, &QAction::triggered, this, &MainWindow::onQuitClicked);
    connect(aboutAction, &QAction::triggered, this, &MainWindow::onAboutClicked);

    QVBoxLayout *leftLayout = new QVBoxLayout{};

    QLabel *searchLabel = new QLabel{"Recherche:", this};
    leftLayout->addWidget(searchLabel);
    _text_edit.reset(new QLineEdit{this});
    leftLayout->addWidget(_text_edit.get());

    connect(_text_edit.get(), &QLineEdit::returnPressed, this, &MainWindow::onButtonClicked);
    connect(_text_edit.get(), &QLineEdit::textChanged, this, &MainWindow::onTextChanged);

    _button.reset(new QPushButton{QString{"Recherche"}, this});
    leftLayout->addWidget(_button.get());
    connect(_button.get(), &QPushButton::clicked, this, &MainWindow::onButtonClicked);

    QLabel *placesLabel = new QLabel{"Lieux:", this};
    leftLayout->addWidget(placesLabel);
    _list.reset(new QListWidget{this});
    leftLayout->addWidget(_list.get());

    connect(_list.get(), &QListWidget::itemDoubleClicked, this, &MainWindow::onPlaceSelected);

    mapWidget = new MapWidget(this);

    QHBoxLayout *mainLayout = new QHBoxLayout{};
    mainLayout->addLayout(leftLayout);
    mainLayout->addWidget(mapWidget);

    QWidget *centralWidget = new QWidget(this);
    centralWidget->setLayout(mainLayout);
    setCentralWidget(centralWidget);

    mapWidget->setCenter(6.03333, 47.25);
    mapWidget->setZoom(12);

    connect(_network_manager.get(), &QNetworkAccessManager::finished, this, &MainWindow::onNetworkReply);

    // Ajouter la barre de statut
    statusBar()->showMessage("RAS");
    updateStatusBar(mapWidget->getLon(), mapWidget->getLat());

    // Connecter le signal de déplacement de la carte à la mise à jour de la barre de statut
    connect(mapWidget, &MapWidget::mapMoved, this, &MainWindow::updateStatusBar);
    connect(mapWidget, &MapWidget::mousePositionChanged, this, [this](double lon, double lat) {
        statusBar()->showMessage(QString("Longitude: %1, Latitude: %2").arg(lon).arg(lat));
    });

    // Configurer le QTimer pour la recherche
    _search_timer->setSingleShot(true);
    connect(_search_timer.get(), &QTimer::timeout, this, &MainWindow::performSearch);
}

MainWindow::~MainWindow() {}

void MainWindow::onButtonClicked() {
    performSearch();
}

void MainWindow::onNetworkReply(QNetworkReply *reply) {
    if (reply->error() != QNetworkReply::NoError) {
        QMessageBox::warning(this, "Network Error", "Failed to retrieve data.");
        reply->deleteLater();
        return;
    }

    QByteArray responseData = reply->readAll();
    reply->deleteLater();

    QJsonDocument doc = QJsonDocument::fromJson(responseData);
    if (!doc.isArray()) {
        QMessageBox::warning(this, "Parsing Error", "Invalid response format.");
        return;
    }

    QJsonArray results = doc.array();
    _list->clear();
    _places_map.clear();

    for (const QJsonValue &value : results) {
        if (value.isObject()) {
            QJsonObject obj = value.toObject();
            QString displayName = obj["display_name"].toString();
            double lat = obj["lat"].toString().toDouble();
            double lon = obj["lon"].toString().toDouble();

            _places_map[displayName] = QPointF(lon, lat);
            _list->addItem(displayName);
        }
    }
}

void MainWindow::onPlaceSelected(QListWidgetItem *item) {
    QString placeName = item->text();
    if (_places_map.contains(placeName)) {
        QPointF coords = _places_map[placeName];
        qDebug() << "carte centrée " << coords;
        mapWidget->setCenter(coords.x(), coords.y());
        updateStatusBar(coords.x(), coords.y());
    }
}

void MainWindow::onQuitClicked() {
    QApplication::quit();
}

void MainWindow::onAboutClicked() {
    QMessageBox::about(this, "A propos de La Carteuuuuu",
                       "<b>La Carteuuuu</b><br>"
                       "Version beaucoup trop je compte plus<br>"
                       "Author: James Genitrini & J-B Froehly<br>"
                       "Description: Globalement on voit une carte et on peut la faire bouger <br> mais prenez un doliprane avant<br>"
                       "Date: " + QDate::currentDate().toString());
}

void MainWindow::updateStatusBar(double lon, double lat) {
    statusBar()->showMessage(QString("Longitude: %1, Latitude: %2").arg(lon).arg(lat));
}

void MainWindow::onTextChanged(const QString &text) {
    if (text.isEmpty()) {
        _list->clear();
        _places_map.clear();
        return;
    }
    _search_timer->start(500); // Délai de 500 ms avant de lancer la recherche
}

void MainWindow::performSearch() {
    QString query = _text_edit->text();
    if (query.isEmpty()) {
        return;
    }

    QString urlString = QString("https://nominatim.openstreetmap.org/search?q=%1&format=json&polygon=0&addressdetails=0")
                            .arg(query);
    QNetworkRequest request((QUrl(urlString)));

    request.setRawHeader("User-Agent", "laCarteu/1.0");

    _network_manager->get(request);
}

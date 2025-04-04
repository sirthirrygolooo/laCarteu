// mapwidget.cpp
#include "mapwidget.h"
#include <QDir>
#include <QFile>
#include <QDebug>
#include <QPaintEvent>
#include <QTimer>
#include <QMouseEvent>
#include <QWheelEvent>

MapWidget::MapWidget(QWidget *parent)
    : QWidget(parent), networkManager(new QNetworkAccessManager(this)), isDragging(false) {
    setMinimumSize(520, 520); // Taille minimale pour la carte
}

void MapWidget::setCenter(double lon, double lat) {
    this->lon = lon;
    this->lat = lat;
    updateVisibleTiles();
    update();
    emit mapMoved(lon, lat); // Émettre le signal lorsque le centre est mis à jour
}

void MapWidget::setZoom(int zoom) {
    this->zoom = zoom;
    updateVisibleTiles();
    update();
}

double MapWidget::getLon() const {
    return lon;
}

double MapWidget::getLat() const {
    return lat;
}

void MapWidget::paintEvent(QPaintEvent *event) {
    qDebug() << "Update carte...";

    QPainter painter(this);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);

    QPointF centerTilePos = latLonToTilePos(lon, lat, zoom);
    int tileX = static_cast<int>(centerTilePos.x());
    int tileY = static_cast<int>(centerTilePos.y());

    bool isLoading = false;

    for (int x = tileX - 1; x <= tileX + 1; ++x) {
        for (int y = tileY - 1; y <= tileY + 1; ++y) {
            QString tileKey = QString("%1-%2-%3").arg(zoom).arg(x).arg(y);
            if (!tileCache.contains(tileKey)) {
                isLoading = true;
                loadTile(x, y, zoom);
            }
            if (tileCache.contains(tileKey)) {
                QPixmap tile = tileCache[tileKey];
                QPointF tilePos = QPointF((x - tileX) * 256, (y - tileY) * 256);
                painter.drawPixmap(tilePos, tile);
                qDebug() << " partie :" << tileKey << " pos :" << tilePos;
            }
        }
    }

    if (isLoading) {
        painter.setPen(Qt::blue);
        painter.setFont(QFont("Arial", 40));
        painter.drawText(event->rect(), Qt::AlignCenter, "⏳");
    }
}

void MapWidget::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        lastMousePos = event->pos();
        isDragging = true;
        setCursor(Qt::ClosedHandCursor);
    }
}

void MapWidget::mouseMoveEvent(QMouseEvent *event) {
    if (isDragging) {
        QPoint delta = event->pos() - lastMousePos;
        double lonDelta = delta.x() / (256.0 * (1 << zoom)) * 360.0;
        double latDelta = -delta.y() / (256.0 * (1 << zoom)) * 360.0;

        lon -= lonDelta;
        lat -= latDelta;

        lastMousePos = event->pos();
        updateVisibleTiles();
        update();
        emit mapMoved(lon, lat);
    }

    // Émettre la position de la souris
    QPointF mouseLatLon = screenPosToLatLon(event->pos());
    emit mousePositionChanged(mouseLatLon.x(), mouseLatLon.y());
}

void MapWidget::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        isDragging = false;
        setCursor(Qt::ArrowCursor);
    }
}

void MapWidget::wheelEvent(QWheelEvent *event) {
    QPointF mousePosBeforeZoom = screenPosToLatLon(event->position().toPoint());

    int numDegrees = event->angleDelta().y() / 8;
    int numSteps = numDegrees / 15;
    zoom += numSteps;

    if (zoom < 0) {
        zoom = 0;
    } else if (zoom > 18) {
        zoom = 18;
    }

    updateVisibleTiles();
    update();

    QPointF mousePosAfterZoom = screenPosToLatLon(event->position().toPoint());
    double lonDelta = mousePosAfterZoom.x() - mousePosBeforeZoom.x();
    double latDelta = mousePosAfterZoom.y() - mousePosBeforeZoom.y();

    lon -= lonDelta;
    lat -= latDelta;

    emit mapMoved(lon, lat);
}

void MapWidget::mouseDoubleClickEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        QPointF mousePosBeforeZoom = screenPosToLatLon(event->pos());
        zoom += 1;
        if (zoom > 18) {
            zoom = 18;
        }
        updateVisibleTiles();
        update();

        QPointF mousePosAfterZoom = screenPosToLatLon(event->pos());
        double lonDelta = mousePosAfterZoom.x() - mousePosBeforeZoom.x();
        double latDelta = mousePosAfterZoom.y() - mousePosBeforeZoom.y();

        lon -= lonDelta;
        lat -= latDelta;

        emit mapMoved(lon, lat);
    }
}

QPointF MapWidget::latLonToTilePos(double lon, double lat, int z) {
    double x = (lon + 180.0) / 360.0 * (1 << z);
    double y = (1.0 - log(tan(lat * M_PI / 180.0) + 1.0 / cos(lat * M_PI / 180.0)) / M_PI) / 2.0 * (1 << z);
    return QPointF(x, y);
}

QPointF MapWidget::screenPosToLatLon(const QPoint &pos) {
    QPointF centerTilePos = latLonToTilePos(lon, lat, zoom);
    int tileX = static_cast<int>(centerTilePos.x());
    int tileY = static_cast<int>(centerTilePos.y());

    double x = (pos.x() / 256.0) + tileX - 1;
    double y = (pos.y() / 256.0) + tileY - 1;

    double lon = x / (1 << zoom) * 360.0 - 180.0;
    double lat = atan(sinh(M_PI * (1 - 2 * y / (1 << zoom)))) * 180.0 / M_PI;

    return QPointF(lon, lat);
}

void MapWidget::loadTile(int x, int y, int z) {
    QString tileKey = QString("%1-%2-%3").arg(z).arg(x).arg(y);
    QString tilePath = QString("tiles/%1/%2/%3.png").arg(z).arg(x).arg(y);

    qDebug() << "Chargement tuile:" << tileKey;

    if (QFile::exists(tilePath)) {
        qDebug() << "tuile en cache:" << tilePath;
        tileCache[tileKey] = QPixmap(tilePath);
    } else {
        qDebug() << "dl de lma tuile :" << tileKey;
        QUrl url(QString("https://tile.openstreetmap.org/%1/%2/%3.png").arg(z).arg(x).arg(y));
        QNetworkRequest request(url);
        request.setRawHeader("User-Agent", "laCarteuuu/1.0");

        QNetworkReply *reply = networkManager->get(request);
        connect(reply, &QNetworkReply::finished, this, [this, reply, tileKey, tilePath, x, y, z]() {
            if (reply->error() == QNetworkReply::NoError) {
                qDebug() << "tuile téléchargée :" << tileKey;
                QByteArray data = reply->readAll();
                QPixmap pixmap;
                pixmap.loadFromData(data);
                if (!pixmap.isNull()) {
                    tileCache[tileKey] = pixmap;
                    QDir().mkpath(QFileInfo(tilePath).absolutePath());
                    pixmap.save(tilePath);
                    update();
                } else {
                    qDebug() << "echec chargement pour tuile :" << tileKey;
                }
            } else {
                qDebug() << "err reseau:" << reply->errorString();
                QTimer::singleShot(1000, this, [this, x, y, z](){
                    loadTile(x, y, z);
                });
            }
            reply->deleteLater();
        });
    }
}

void MapWidget::updateVisibleTiles() {
    tileCache.clear();
    QPointF centerTilePos = latLonToTilePos(lon, lat, zoom);
    int tileX = static_cast<int>(centerTilePos.x());
    int tileY = static_cast<int>(centerTilePos.y());

    for (int x = tileX - 1; x <= tileX + 1; ++x) {
        for (int y = tileY - 1; y <= tileY + 1; ++y) {
            loadTile(x, y, zoom);
        }
    }
}

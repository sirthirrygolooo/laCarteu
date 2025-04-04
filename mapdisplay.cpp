// mapdisplay.cpp
#include "mapdisplay.h"
#include <QDebug>

MapDisplay::MapDisplay(QWidget *parent)
    : QWidget(parent), isDragging(false), tileManager(new TileManager(this)) {
    setMinimumSize(520, 520);
}

void MapDisplay::setCenter(double lon, double lat) {
    this->lon = lon;
    this->lat = lat;
    updateVisibleTiles();
    update();
    emit mapMoved(lon, lat);
}

void MapDisplay::setZoom(int zoom) {
    this->zoom = zoom;
    updateVisibleTiles();
    update();
}

int MapDisplay::getZoom() const {
    return zoom;
}

void MapDisplay::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);

    QPointF centerTilePos = latLonToTilePos(lon, lat, zoom);
    int tileX = static_cast<int>(centerTilePos.x());
    int tileY = static_cast<int>(centerTilePos.y());

    for (int x = tileX - 1; x <= tileX + 1; ++x) {
        for (int y = tileY - 1; y <= tileY + 1; ++y) {
            QPixmap tile = tileManager->getTile(x, y, zoom);
            if (!tile.isNull()) {
                QPointF tilePos = QPointF((x - tileX) * 256, (y - tileY) * 256);
                painter.drawPixmap(tilePos, tile);
            }
        }
    }
}

void MapDisplay::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        lastMousePos = event->pos();
        isDragging = true;
        setCursor(Qt::ClosedHandCursor);
    }
}

void MapDisplay::mouseMoveEvent(QMouseEvent *event) {
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

    QPointF mouseLatLon = screenPosToLatLon(event->pos());
    emit mousePositionChanged(mouseLatLon.x(), mouseLatLon.y());
}

void MapDisplay::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        isDragging = false;
        setCursor(Qt::ArrowCursor);
    }
}

void MapDisplay::wheelEvent(QWheelEvent *event) {
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

void MapDisplay::mouseDoubleClickEvent(QMouseEvent *event) {
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

QPointF MapDisplay::latLonToTilePos(double lon, double lat, int z) {
    double x = (lon + 180.0) / 360.0 * (1 << z);
    double y = (1.0 - log(tan(lat * M_PI / 180.0) + 1.0 / cos(lat * M_PI / 180.0)) / M_PI) / 2.0 * (1 << z);
    return QPointF(x, y);
}

QPointF MapDisplay::screenPosToLatLon(const QPoint &pos) {
    QPointF centerTilePos = latLonToTilePos(lon, lat, zoom);
    int tileX = static_cast<int>(centerTilePos.x());
    int tileY = static_cast<int>(centerTilePos.y());

    double x = (pos.x() / 256.0) + tileX - 1;
    double y = (pos.y() / 256.0) + tileY - 1;

    double lon = x / (1 << zoom) * 360.0 - 180.0;
    double lat = atan(sinh(M_PI * (1 - 2 * y / (1 << zoom)))) * 180.0 / M_PI;

    return QPointF(lon, lat);
}

void MapDisplay::updateVisibleTiles() {
    QPointF centerTilePos = latLonToTilePos(lon, lat, zoom);
    int tileX = static_cast<int>(centerTilePos.x());
    int tileY = static_cast<int>(centerTilePos.y());

    for (int x = tileX - 1; x <= tileX + 1; ++x) {
        for (int y = tileY - 1; y <= tileY + 1; ++y) {
            tileManager->getTile(x, y, zoom);
        }
    }
}

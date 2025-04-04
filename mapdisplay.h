// mapdisplay.h
#ifndef MAPDISPLAY_H
#define MAPDISPLAY_H

#include <QWidget>
#include <QPainter>
#include <QMouseEvent>
#include <QWheelEvent>
#include "tilemanager.h"

class MapDisplay : public QWidget {
    Q_OBJECT

public:
    MapDisplay(QWidget *parent = nullptr);
    void setCenter(double lon, double lat);
    void setZoom(int zoom);
    int getZoom() const;

signals:
    void mapMoved(double lon, double lat);
    void mousePositionChanged(double lon, double lat);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;

private:
    double lon, lat;
    int zoom;
    bool isDragging;
    QPoint lastMousePos;
    TileManager *tileManager;

    QPointF latLonToTilePos(double lon, double lat, int z);
    QPointF screenPosToLatLon(const QPoint &pos);
    void updateVisibleTiles();
};

#endif // MAPDISPLAY_H

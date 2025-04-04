#ifndef MAPWIDGET_H
#define MAPWIDGET_H

#include <QWidget>
#include <QPainter>
#include <QPixmap>
#include <QMap>
#include <QPointF>
#include <QNetworkAccessManager>
#include <QNetworkReply>

class MapWidget : public QWidget {
    Q_OBJECT

public:
    explicit MapWidget(QWidget *parent = nullptr);

    void setCenter(double lon, double lat);
    void setZoom(int zoom);

    double getLon() const;
    double getLat() const;

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
    QMap<QString, QPixmap> tileCache;
    QNetworkAccessManager *networkManager;
    QPoint lastMousePos;
    bool isDragging;

    void loadTile(int x, int y, int z);
    QPointF latLonToTilePos(double lon, double lat, int z);
    void updateVisibleTiles();
    QPointF screenPosToLatLon(const QPoint &pos);
};

#endif

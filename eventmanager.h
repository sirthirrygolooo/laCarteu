// eventmanager.h
#ifndef EVENTMANAGER_H
#define EVENTMANAGER_H

#include <QObject>
#include "mapdisplay.h"

class EventManager : public QObject {
    Q_OBJECT

public:
    EventManager(MapDisplay *mapDisplay, QObject *parent = nullptr);

    public slots:
        void onZoomIn();
    void onZoomOut();
    void onMapMoved(double lon, double lat);

private:
    MapDisplay *mapDisplay;
};

#endif // EVENTMANAGER_H

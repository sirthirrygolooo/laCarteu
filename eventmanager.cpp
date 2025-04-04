// eventmanager.cpp
#include "eventmanager.h"

EventManager::EventManager(MapDisplay *mapDisplay, QObject *parent)
    : QObject(parent), mapDisplay(mapDisplay) {
    connect(mapDisplay, &MapDisplay::mapMoved, this, &EventManager::onMapMoved);
}

void EventManager::onZoomIn() {
    int newZoom = mapDisplay->getZoom() + 1;
    if (newZoom <= 18) {
        mapDisplay->setZoom(newZoom);
    }
}

void EventManager::onZoomOut() {
    int newZoom = mapDisplay->getZoom() - 1;
    if (newZoom >= 0) {
        mapDisplay->setZoom(newZoom);
    }
}


void EventManager::onMapMoved(double lon, double lat) {
    // Mettre à jour l'état de l'application ou effectuer d'autres actions
}

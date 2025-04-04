// tilemanager.cpp
#include "tilemanager.h"
#include <QFile>
#include <QDebug>

TileManager::TileManager(QObject *parent)
    : QObject(parent), networkManager(new QNetworkAccessManager(this)) {}

QPixmap TileManager::getTile(int x, int y, int z) {
    QString tileKey = getTileKey(x, y, z);
    if (tileCache.contains(tileKey)) {
        return tileCache[tileKey];
    }

    QString tilePath = getTilePath(x, y, z);
    if (QFile::exists(tilePath)) {
        QPixmap pixmap(tilePath);
        tileCache[tileKey] = pixmap;
        return pixmap;
    }

    requestTile(x, y, z);
    return QPixmap();
}

void TileManager::requestTile(int x, int y, int z) {
    QString tileKey = getTileKey(x, y, z);
    QString urlString = QString("https://tile.openstreetmap.org/%1/%2/%3.png").arg(z).arg(x).arg(y);
    QNetworkRequest request{QUrl(urlString)};
    request.setRawHeader("User-Agent", "laCarteuuu/1.0");

    QNetworkReply *reply = networkManager->get(request);
    connect(reply, &QNetworkReply::finished, this, &TileManager::onTileDownloaded);
}


void TileManager::onTileDownloaded() {
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    if (reply) {
        if (reply->error() == QNetworkReply::NoError) {
            QByteArray data = reply->readAll();
            QPixmap pixmap;
            pixmap.loadFromData(data);

            if (!pixmap.isNull()) {
                QString tileKey = reply->request().url().path();
                tileCache[tileKey] = pixmap;
                QString tilePath = getTilePath(tileKey);
                QDir().mkpath(QFileInfo(tilePath).absolutePath());
                pixmap.save(tilePath);
                emit tileUpdated(tileKey);
            }
        }
        reply->deleteLater();
    }
}

QString TileManager::getTilePath(const QString &tileKey) {
    QStringList parts = tileKey.split('/');
    int z = parts[1].toInt();
    int x = parts[2].toInt();
    int y = parts[3].split('.').first().toInt();
    return getTilePath(x, y, z);
}

QString TileManager::getTilePath(int x, int y, int z) {
    return QString("tiles/%1/%2/%3.png").arg(z).arg(x).arg(y);
}


QString TileManager::getTileKey(int x, int y, int z) {
    return QString("%1-%2-%3").arg(z).arg(x).arg(y);
}

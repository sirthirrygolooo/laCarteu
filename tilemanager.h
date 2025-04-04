// tilemanager.h
#ifndef TILEMANAGER_H
#define TILEMANAGER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QMap>
#include <QPixmap>
#include <QDir>

class TileManager : public QObject {
    Q_OBJECT

public:
    TileManager(QObject *parent = nullptr);
    QPixmap getTile(int x, int y, int z);
    void requestTile(int x, int y, int z);

private slots:
    void onTileDownloaded();

signals:
    void tileUpdated(const QString &tileKey);

private:
    QNetworkAccessManager *networkManager;
    QMap<QString, QPixmap> tileCache;
    QString getTilePath(int x, int y, int z);
    QString getTileKey(int x, int y, int z);
    QString getTilePath(const QString &tileKey);
};

#endif // TILEMANAGER_H

#ifndef PLAYLISTMANAGER_H
#define PLAYLISTMANAGER_H

#include <QObject>
#include <QDataStream>
#include <QSettings>
#include <QStandardPaths>
#include <QUuid>
#include <QStandardItemModel>
#include <QFile>
#include <QDir>

#include "VMedia.h"

class PlaylistManager : public QObject
{
    Q_OBJECT
public:
    explicit PlaylistManager(QObject *parent = nullptr);
    void addNew(const QString &aname = QString());
    QStandardItemModel* retrieve(const QString &pname = QString());
    bool remove(const QString &pname);
    QStringList getAll() const;
    void retranslateHeaders();
    bool rename(const QString &oldName, const QString &newName);
    QStringList getColumnNames() const;
    bool addEntryToPlaylist(const QString &rec, const QString &pname = QString());
    bool addEntriesToPlaylist(const QStringList &recs, const QString &pname = QString());
    int itemCountForPlaylist(const QString &playlist, bool isLive = false) const;
    QString getLiveTitle() const;
    void sync();
    void setPlaying(const QString &item, const QString &pname);
    void clearPlaying(const QString &pname);

public slots:
    void clearPlaylist(const QString &pname);

private:
    QHash<QString,QStandardItemModel*> mPlaylists;
    QStandardItemModel* live;
    QString liveTitle;
    const QString dataFile = "db.pap";
    VMedia mediaRader;

    void setupHeaders();
    void initPlaylists();
    QString uniqueName(const QString &aname) const;

};

#endif // PLAYLISTMANAGER_H

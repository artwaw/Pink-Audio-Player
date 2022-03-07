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

/*!
 * \class PlaylistManager
 * \brief Class managing the backend of the playlists
 *
 * Class internally uses QStandardItemModel to hold two separate sets of data:
 * one for the "live view" playlist and other one, in form of QHash table, for
 * the other playlists.
 * I considered using in-memory Sqlite db, which would be faster and more memory
 * efficient however refreshing the view would lead to problems with maintaining
 * selection.
 *
 * Internally class calls the wrapper around the TagLib to read the metadata from
 * the files.
 *
 * The class provides usuall getter methods to manage the content of each of the
 * playlists.
 * \sa VMedia
 */
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
    QHash<QString,QStandardItemModel*> mPlaylists; //!< Internal playlist holder structure for user generated playlists
    QStandardItemModel* live; //!< "Live view" playlist model
    QString liveTitle; //!< "Live view" playlist title
    const QString dataFile = "db.pap"; //!< database file name
    VMedia mediaRader; //!< Tag reading interface

    void setupHeaders();
    void initPlaylists();
    QString uniqueName(const QString &aname) const;

};

#endif // PLAYLISTMANAGER_H

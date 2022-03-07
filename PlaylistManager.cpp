#include "PlaylistManager.h"

/*!
 * \brief C'tor, initializes internal structures.
 * \param parent Optional parent pointer
 */
PlaylistManager::PlaylistManager(QObject *parent) : QObject(parent)
{
    initPlaylists();
    setupHeaders();
}

/*!
 * \brief Preallocates new playlist structures.
 * \param aname QString ID for the playlist
 */
void PlaylistManager::addNew(const QString &aname) {
    if (aname.isEmpty()) { return; }
    QStandardItemModel *proxy = new QStandardItemModel(0,10,this);
    mPlaylists.insert(uniqueName(aname),proxy);
}

/*!
 * \brief Retrieves pointer to the playlist by name
 * \param pname Unique QString identifier
 * \return Pointer to QStandardItemModel for the playlist.
 */
QStandardItemModel* PlaylistManager::retrieve(const QString &pname) {
    if (pname.isEmpty()) { return live; }
    return mPlaylists.value(pname);
}

/*!
 * \brief Removes the playlist and deallocates resources.
 * \param pname Unique name of the playlist
 * \return FALSE if the playlist name could not be found, otherwise TRUE
 */
bool PlaylistManager::remove(const QString &pname) {
    if (pname.isEmpty()) { return true; }
    QStandardItemModel *proxy = retrieve(pname);
    bool ret = mPlaylists.remove(pname);
    if (proxy!=nullptr) { delete proxy; }
    mPlaylists.squeeze();
    return ret;
}

/*!
 * \brief Returns all stored, user created playlist names
 * \return QStringList with names. Can be empty if only "live view" playlist is present.
 */
QStringList PlaylistManager::getAll() const {
    return mPlaylists.keys();
}

/*!
 * \brief Routine forces resetting header names. Called upon a language change event.
 */
void PlaylistManager::retranslateHeaders() {
    setupHeaders();
}

/*!
 * \brief Renames existing playlist.
 * \param oldName Old name of the playlist
 * \param newName New name of the playlist
 * \return Always TRUE
 * \todo There is no error checking. I don't foresee any edge cases but some error checking would be in order.
 */
bool PlaylistManager::rename(const QString &oldName, const QString &newName) {
    if (oldName.isEmpty()||newName.isEmpty()) { return true; }
    if (oldName == newName) { return true; }
    QStandardItemModel* item = mPlaylists.value(oldName);
    mPlaylists.remove(oldName);
    mPlaylists.insert(newName,item);
    return true;
}

/*!
 * \brief Retrives all the column names in the currently set locale.
 * \return QStringList with the column names for the current locale.
 */
QStringList PlaylistManager::getColumnNames() const {
    const QStringList headers = {
        tr("Playing"), tr("Title"), tr("Artist"), tr("Duration"), tr("Genre"), tr("Comment"), tr("Album"), tr("Track"),
        tr("Date"), tr("File path")
    };
    return headers;
}

/*!
 * \brief Adds element to the specified playlist.
 * \param rec Absolute file path.
 * \param pname Playlist name to which the file is being added.
 * \return Always TRUE.
 */
bool PlaylistManager::addEntryToPlaylist(const QString &rec, const QString &pname) {
    if (rec.isEmpty()) { return false; }
    mediaRader.getInfo(rec);
    QStandardItemModel* model;
    pname.isEmpty()?model=live:model=mPlaylists.value(pname);
    QList<QStandardItem*> items;
    items.append(new QStandardItem(QString()));
    items.append(new QStandardItem(mediaRader.getTitle()));
    items.append(new QStandardItem(mediaRader.getArtist()));
    items.append(new QStandardItem(mediaRader.getDuration()));
    items.append(new QStandardItem(mediaRader.getGenre()));
    items.append(new QStandardItem(mediaRader.getComment()));
    items.append(new QStandardItem(mediaRader.getAlbum()));
    items.append(new QStandardItem(mediaRader.getTrack()));
    items.append(new QStandardItem(mediaRader.getYear()));
    items.append(new QStandardItem(rec));
    model->appendRow(items);
    return true;
}

/*!
 * \brief Adds multiple local entries to the playlist.
 * \param recs QStringList with absolute file paths
 * \param pname Playlist to which add to.
 * \return Always TRUE
 * \sa PlaylistManager::addEntryToPlaylist()
 */
bool PlaylistManager::addEntriesToPlaylist(const QStringList &recs, const QString &pname) {
    if (recs.isEmpty()) { return false; }
    bool ret = true;
    for (auto x=0;x<recs.size();++x) {
        ret = ret && addEntryToPlaylist(recs.at(x),pname);
    }
    return ret;
}

/*!
 * \brief Returns number of items in the current playlist.
 * \param playlist Playlist in question. Can be empty string.
 * \param isLive Indication if current is "live view".
 * \return An int with item count.
 */
int PlaylistManager::itemCountForPlaylist(const QString &playlist, bool isLive) const {
    return isLive?live->rowCount():mPlaylists.value(playlist)->rowCount();
}

/*!
 * \brief Returns the label with the name of the "live view".
 * \return QString with tab label.
 */
QString PlaylistManager::getLiveTitle() const {
    return liveTitle;
}

/*!
 * \brief Writes playlists and their content to the db file.
 *
 * Internally, QDataStream is used to read from the binary file. Stream versioning is used
 * as a means to introduce changes between the versions.
 * The file is closed once the write is done.
 * \sa PlaylistManager::initPlaylists()
 * \todo Write it ;)
 */
void PlaylistManager::sync() {
    //
}

/*!
 * \brief Sets the "currently playling" indicator on the currently played item.
 * \param item Absolute path of the item.
 * \param pname Playlist to which item belogns to, active playlist.
 * \sa PlaylistManager::clearPlaying()
 */
void PlaylistManager::setPlaying(const QString &item, const QString &pname) {
    QStandardItemModel *model = pname.isEmpty()?live:mPlaylists.value(pname);
    for (auto x=0;x<model->rowCount();++x) {
        if (model->data(model->index(x,9)).toString()==item) {
            model->setItem(x,0,new QStandardItem(">"));
            break;
        }
    }
}

/*!
 * \brief Clears the "currently playing" idicator from playlist.
 * \param pname Currently active playlist name.
 * \sa PlaylistManager::setPlaying()
 */
void PlaylistManager::clearPlaying(const QString &pname) {
    QStandardItemModel *model = pname.isEmpty()?live:mPlaylists.value(pname);
    for (auto x=0;x<model->rowCount();++x) {
        if (!model->data(model->index(x,0)).toString().isEmpty()) {
            model->setItem(x,0,new QStandardItem());
            break;
        }
    }
}

/*!
 * \brief Clears (removes) all the items from the selected playlist.
 * \param pname Playlist name to clear, if empty "live view" is used.
 */
void PlaylistManager::clearPlaylist(const QString &pname = QString()) {
    pname.isEmpty()?live->setRowCount(0):mPlaylists.value(pname)->setRowCount(0);
}

/*!
 * \brief Method assigning translated headers to the views
 * \sa PlaylistManager::getColumnNames()
 */
void PlaylistManager::setupHeaders() {
    const QStringList headers = getColumnNames();
    live->setHorizontalHeaderLabels(headers);
    for (auto it=mPlaylists.constBegin();it!=mPlaylists.constEnd();++it) {
        it.value()->setHorizontalHeaderLabels(headers);
    }
}

/*!
 * \brief Reads the stored playists and their contents from the db file.
 *
 * Internally, QDataStream is used to read from the binary file. Stream versioning is used
 * as a means to introduce changes between the versions.
 * The file is closed once the read is done.
 *
 * \sa PlaylsitManager::sync()
 * \todo Remove Q_ASSERT, make error checking.
 */
void PlaylistManager::initPlaylists() {
    QDir fpath(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation));
    if (!fpath.exists()) {
        fpath.mkpath(fpath.absolutePath());
    }
    QFile file(fpath.absolutePath()+"/"+dataFile);
    QDataStream stream;
    if (!file.exists()) {
        Q_ASSERT(file.open(QIODevice::WriteOnly));
        stream.setDevice(&file);
        stream.setVersion(QDataStream::Qt_6_2);
        stream << QString("Live view");
        stream << 0;
        stream << 0;
        file.close();
    }
    live = new QStandardItemModel(this);
    Q_ASSERT(file.open(QIODevice::ReadOnly));
    stream.setDevice(&file);
    stream.setVersion(QDataStream::Qt_6_2);
    stream >> liveTitle;
    int liveCount;
    stream >> liveCount;
    QStringList items;
    if (liveCount>0) {
        stream >> items;
        addEntriesToPlaylist(items);
    }
    int all;
    stream >> all;
    if (all>0) {
        QString title;
        for (auto x=0;x<all;++x) {
            stream >> title;
            stream >> items;
            addNew(title);
            addEntriesToPlaylist(items,title);
        }
    }
    file.close();
}

/*!
 * \brief Method returns the unique name for the new playlist.
 * \param aname Proposed name of the new playlist.
 * \return Unique name. Can be same as aname.
 */
QString PlaylistManager::uniqueName(const QString &aname) const {
    if (!mPlaylists.contains(aname)) { return aname; }
    int x=1;
    QString name = QString("%1 %2").arg(aname,QString::number(x));
    while (mPlaylists.contains(name)) {
        ++x;
        name = QString("%1 %2").arg(aname,QString::number(x));
    }
    return name;
}



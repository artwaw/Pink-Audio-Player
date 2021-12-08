#include "PlaylistManager.h"

PlaylistManager::PlaylistManager(QObject *parent) : QObject(parent)
{
    initPlaylists();
    setupHeaders();
}

void PlaylistManager::addNew(const QString &aname) {
    if (aname.isEmpty()) { return; }
    QStandardItemModel *proxy = new QStandardItemModel(0,10,this);
    mPlaylists.insert(uniqueName(aname),proxy);
}

QStandardItemModel* PlaylistManager::retrieve(const QString &pname) {
    if (pname.isEmpty()) { return live; }
    return mPlaylists.value(pname);
}

bool PlaylistManager::remove(const QString &pname) {
    if (pname.isEmpty()) { return true; }
    bool ret = mPlaylists.remove(pname);
    mPlaylists.squeeze();
    return ret;
}

QStringList PlaylistManager::getAll() const {
    return mPlaylists.keys();
}

void PlaylistManager::retranslateHeaders() {
    setupHeaders();
}

bool PlaylistManager::rename(const QString &oldName, const QString &newName) {
    if (oldName.isEmpty()||newName.isEmpty()) { return true; }
    if (oldName == newName) { return true; }
    QStandardItemModel* item = mPlaylists.value(oldName);
    mPlaylists.remove(oldName);
    mPlaylists.insert(newName,item);
    return true;
}

QStringList PlaylistManager::getColumnNames() const {
    const QStringList headers = {
        tr("Playing"), tr("Title"), tr("Artist"), tr("Duration"), tr("Genre"), tr("Comment"), tr("Album"), tr("Track"),
        tr("Date"), tr("File path")
    };
    return headers;
}

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

bool PlaylistManager::addEntriesToPlaylist(const QStringList &recs, const QString &pname) {
    if (recs.isEmpty()) { return false; }
    bool ret = true;
    for (auto x=0;x<recs.size();++x) {
        ret = ret && addEntryToPlaylist(recs.at(x),pname);
    }
    return ret;
}

int PlaylistManager::itemCountForPlaylist(const QString &playlist, bool isLive) const {
    return isLive?live->rowCount():mPlaylists.value(playlist)->rowCount();
}

QString PlaylistManager::getLiveTitle() const {
    return liveTitle;
}

void PlaylistManager::sync() {
    //
}

void PlaylistManager::setPlaying(const QString &item, const QString &pname) {
    QStandardItemModel *model = pname.isEmpty()?live:mPlaylists.value(pname);
    for (auto x=0;x<model->rowCount();++x) {
        if (model->data(model->index(x,9)).toString()==item) {
            model->setItem(x,0,new QStandardItem(">"));
            qDebug() << "set" << x;
            break;
        }
    }
}

void PlaylistManager::clearPlaying(const QString &pname) {
    QStandardItemModel *model = pname.isEmpty()?live:mPlaylists.value(pname);
    for (auto x=0;x<model->rowCount();++x) {
        if (!model->data(model->index(x,0)).toString().isEmpty()) {
            model->setItem(x,0,new QStandardItem());
            qDebug() << "cleared" << x;
            break;
        }
    }
}

void PlaylistManager::clearPlaylist(const QString &pname = QString()) {
    pname.isEmpty()?live->setRowCount(0):mPlaylists.value(pname)->setRowCount(0);
}

void PlaylistManager::setupHeaders() {
    const QStringList headers = getColumnNames();
    live->setHorizontalHeaderLabels(headers);
    for (auto it=mPlaylists.constBegin();it!=mPlaylists.constEnd();++it) {
        it.value()->setHorizontalHeaderLabels(headers);
    }
}

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



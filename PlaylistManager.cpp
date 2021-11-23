#include "PlaylistManager.h"

PlaylistManager::PlaylistManager(QObject *parent) : QObject(parent)
{
    QSqlDatabase db;
    if (QSqlDatabase::connectionNames().contains("playlists")) {
        db = QSqlDatabase::database("playlists",true);
    } else {
        db = QSqlDatabase::addDatabase("QSQLITE","playlists");
        QSettings settings;
        if (!settings.contains("dbname")) {
            settings.setValue("dbname",QStandardPaths::writableLocation(QStandardPaths::AppDataLocation));
            settings.sync();
        }
        db.setDatabaseName(settings.value("dbname").toString());
        Q_ASSUME(db.open());
    }
    QSqlQuery query(db);
    if (!db.tables().contains("names")) {
        query.exec("create table names(id integer primary key,tname text, dname text);");
        query.exec("insert into names(tname,dname) values('SystemDefault','Default');");
    }
    if (!db.tables().contains(tr("playlists"))) {
        createTable();
    }
    playlistModel = new QSqlTableModel(this,db);
    playlistModel->setTable("playlists");
    playlistModel->select();
    setupHeaders();
    initPlaylists();
}

QSortFilterProxyModel* PlaylistManager::addNew(const QString &aname) {
    auto db = QSqlDatabase::database("playlists",true);
    QSqlQuery query(db);
    QString name;
    if (aname.isEmpty()) {
        int x = 1;
        name = tr("New playlist %1").arg(x);
        while (getAll().contains(name)) {
            ++x;
            name = tr("New playlist %1").arg(x);
        }
    } else {
        name = aname;
    }
    QString tname = QUuid::createUuid().toString(QUuid::Id128);
    QString q = QString("insert into names(tname,dname) values('%1','%2');").arg(tname,name);
    Q_ASSUME(query.exec(q));
    QSortFilterProxyModel *proxy = new QSortFilterProxyModel(this);
    proxy->setSourceModel(playlistModel);
    proxy->setFilterFixedString(tname);
    proxy->setFilterKeyColumn(playlistModel->record().indexOf("playlist"));
    mPlaylists.append(proxy);
    return proxy;
}

QSortFilterProxyModel* PlaylistManager::retrieve(const QString pname) {
    if (pname.isEmpty()) { return nullptr; }
    QString tname = getTName(pname);
    if (tname.isEmpty()) { return nullptr; }
    for (auto x=0;x<mPlaylists.size();++x) {
        if (mPlaylists.at(x)->filterRegularExpression().pattern()==tname) {
            return mPlaylists.at(x);
        }
    }
    return nullptr;
}

bool PlaylistManager::remove(const QString &pname) {
    if (pname.isEmpty()) { return true; }
    auto db = QSqlDatabase::database("playlists",true);
    QSqlQuery query(db);
    bool res = true;
    QString tname = getTName(pname);
    res = res && query.exec("delete from playlists where playlist like '"+tname+"';");
    res = res && query.exec("delete from names where tname like '"+tname+"';");
    for (auto x=0;x<mPlaylists.size();++x) {
        if (mPlaylists.at(x)->filterRegularExpression().pattern()==tname) {
            mPlaylists.removeAt(x);
        }
    }
    mPlaylists.squeeze();
    return res;
}

QStringList PlaylistManager::getAll() const {
    QStringList res;
    auto db = QSqlDatabase::database("playlists",true);
    QSqlQuery query(db);
    query.exec("select dname from names;");
    if (query.lastError().type()!=QSqlError::NoError) { return QStringList(); }
    while (query.next()) {
        res.append(query.value(0).toString());
    }
    return res;
}

void PlaylistManager::retranslateHeaders() {
    setupHeaders();
}

bool PlaylistManager::rename(const QString &oldName, const QString &newName) {
    if (oldName.isEmpty()||newName.isEmpty()) { return true; }
    if (oldName == newName) { return true; }
    bool res = true;
    auto db = QSqlDatabase::database("playlists",true);
    QSqlQuery query(db);
    res = res && query.exec("update names set dname ='"+newName+"' where dname like '"+oldName+"';");
    res = res && query.exec("update playlists set playlist = '"+newName+"' where playlist like '"+oldName+"';");
    QSortFilterProxyModel *model = retrieve(oldName);
    model->setFilterFixedString(newName);
    return res;
}

QStringList PlaylistManager::getColumnNames() const {
    QStringList res;
    for (auto x=1;x<17;++x) {
        res.append(playlistModel->headerData(x,Qt::Horizontal).toString());
    }
    return res;
}

void PlaylistManager::setupHeaders() {
    playlistModel->setHeaderData(1,Qt::Horizontal,tr("Title"));
    playlistModel->setHeaderData(2,Qt::Horizontal,tr("Artist"));
    playlistModel->setHeaderData(3,Qt::Horizontal,tr("Genre"));
    playlistModel->setHeaderData(4,Qt::Horizontal,tr("Copyright"));
    playlistModel->setHeaderData(5,Qt::Horizontal,tr("Album"));
    playlistModel->setHeaderData(6,Qt::Horizontal,tr("Track"));
    playlistModel->setHeaderData(7,Qt::Horizontal,tr("Description"));
    playlistModel->setHeaderData(8,Qt::Horizontal,tr("Rating"));
    playlistModel->setHeaderData(9,Qt::Horizontal,tr("Date"));
    playlistModel->setHeaderData(10,Qt::Horizontal,tr("Setting"));
    playlistModel->setHeaderData(11,Qt::Horizontal,tr("Url"));
    playlistModel->setHeaderData(12,Qt::Horizontal,tr("Language"));
    playlistModel->setHeaderData(13,Qt::Horizontal,tr("Publisher"));
    playlistModel->setHeaderData(14,Qt::Horizontal,tr("Encoded by"));
    playlistModel->setHeaderData(15,Qt::Horizontal,tr("Artwork url"));
    playlistModel->setHeaderData(16,Qt::Horizontal,tr("Track ID"));
}

bool PlaylistManager::createTable() const {
    auto db = QSqlDatabase::database("playlists",true);
    QSqlQuery query(db);
    return query.exec("create table playlists(id integer primary key,"
                            "title text,artist text, genre text, copyright text, album text, track int, description text,"
                            "rating text, date text, setting text, url text, language text, publisher text,"
                            "encoded text, artwork text, trackid text, playlist text, fpath text, prow int);");
}

void PlaylistManager::initPlaylists() {
    const QStringList all = getAll();
    for (auto x=0;x<all.size();++x) {
        QSortFilterProxyModel *model = new QSortFilterProxyModel(this);
        model->setSourceModel(playlistModel);
        model->setFilterKeyColumn(playlistModel->record().indexOf("playlist"));
        model->setFilterFixedString(getTName(all.at(x)));
        mPlaylists.append(model);
    }
}

QString PlaylistManager::getTName(const QString &dname) const {
    auto db = QSqlDatabase::database("playlists",true);
    QSqlQuery query(db);
    query.exec("select tname from names where dname like '"+dname+"';");
    if (query.lastError().type()!=QSqlError::NoError) { return QString(); }
    query.next();
    return query.value(0).toString();
}

QString PlaylistManager::getDName(const QString &tname) const {
    auto db = QSqlDatabase::database("playlists",true);
    QSqlQuery query(db);
    query.exec("select dname from names where tname like '"+tname+"';");
    if (query.lastError().type()!=QSqlError::NoError) { return QString(); }
    query.next();
    return query.value(0).toString();
}

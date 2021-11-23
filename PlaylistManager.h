#ifndef PLAYLISTMANAGER_H
#define PLAYLISTMANAGER_H

#include <QObject>
#include <QSqlTableModel>
#include <QSortFilterProxyModel>
#include <QSqlQuery>
#include <QSqlDatabase>
#include <QSqlRecord>
#include <QSqlError>
#include <QSettings>
#include <QStandardPaths>
#include <QUuid>

class PlaylistManager : public QObject
{
    Q_OBJECT
public:
    explicit PlaylistManager(QObject *parent = nullptr);
    QSortFilterProxyModel* addNew(const QString &aname = QString());
    QSortFilterProxyModel* retrieve(const QString pname);
    bool remove(const QString &pname);
    QStringList getAll() const;
    void retranslateHeaders();
    bool rename(const QString &oldName, const QString &newName);
    QStringList getColumnNames() const;

private:
    QList<QSortFilterProxyModel*> mPlaylists;
    QSqlTableModel *playlistModel;

    void setupHeaders();
    bool createTable() const;
    void initPlaylists();
    QString getTName(const QString &dname) const;
    QString getDName(const QString &tname) const;
};

#endif // PLAYLISTMANAGER_H

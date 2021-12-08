#ifndef PLAYER_H
#define PLAYER_H

#include <QMainWindow>
#include <QActionGroup>
#include <QTranslator>
#include <QMessageBox>
#include <QTableView>
#include <QInputDialog>
#include <QFileDialog>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QMediaMetaData>
#include <QRandomGenerator>

#include "PlaylistManager.h"
#include "PlayingDelegate.h"
#include "VMedia.h"

QT_BEGIN_NAMESPACE
    namespace Ui { class Player; }
QT_END_NAMESPACE

class Player : public QMainWindow
{
    Q_OBJECT
public:
    Player(QWidget *parent = nullptr);
    ~Player();

private:
    Ui::Player *ui;
    //menu file actions
    QAction *fileOpen;
    QAction *fileAddFile;
    QAction *fileAddDir;
    QAction *fileAddLoc;
    QAction *fileNewPlaylist;
    QAction *fileOpenPlaylist;
    QAction *fileSavePlaylist;
    QAction *fileExit;
    //menu edit
    QAction *clear;
    QAction *selectAll;
    QAction *search;
    QAction *removeDuplicates;
    QAction *removeDeadItems;
    QAction *prefs;
    //menu playback
    QAction *playNext;
    QAction *playPrev;
    QAction *playPause;
    QActionGroup *playOrder;
    QAction *orderDefault;
    QAction *orderRepeatAll;
    QAction *orderSingleTrack;
    QAction *orderRepeatTrack;
    QAction *orderShuflle;
    QAction *orderShuffleRepeat;
    QAction *stopAfterCurrent;
    QActionGroup *followOrder;
    QAction *playFollowCursor;
    QAction *playFollowsCursor;
    //menu library
    QAction *libClear;
    QAction *libRebuild;
    QAction *libUpdate;
    //menu language
    QActionGroup *languages;
    QAction *langPol;
    QAction *langEng;
    QAction *langDeu;
    //menu help
    QAction *helpAbout;
    QAction *helpIndex;
    //context menus
    QMenu *columnContextMenu;
    QMenu *tabBarContextMenu;

    QTranslator qtTranslator;
    QTranslator myTranslator;
    PlaylistManager playlists;
    QMediaPlayer player;
    QAudioOutput output;
    QStringList extensions;
    QPair<QTableView*,int> currentlyPlaying;

    void initActions();
    void setLanguage(const QString &lang);
    void restorePlaylists();
    void decoratePlaylist(QTableView *view);
    void setPlaylistColumnVisibility();
    QString getUniquePlaylistName(const QString &pname = QString()) const;
    void writePlaylistNames();
    bool playlistNameUsed(const QString &aname = QString()) const;
    void setupPlayer();
    QString currentPName() const;
    int nextTrack() const;
    void playNextTrack();

private slots:
    void doFileOpen();
    void doAddFile();
    void doAddDir();
    void doAddLoc();
    void doNewPlaylist();
    void doOpenPlaylist();
    void doSavePlaylist();
    void doClearPlaylist();
    void doSearch();
    void doRemoveDuplicates();
    void doKillTheDead();
    void clearLibrary();
    void rebuildLibrary();
    void updateLibrary();
    void doAbout();
    void doHelp();
    void columnContextMenuRequested(const QPoint &point);
    void restorePlaylistColumns();
    void playlistColumnVisibilityChanged();
    void playlistCloseRequested(int index);
    void playlistChanged(int index);
    void renameTab(int index);
    void tabBarContextMenuRequested(const QPoint &point);
    void mediaStatusChanged(QMediaPlayer::MediaStatus status);
    void mediaPlaybackStatusChanged(QMediaPlayer::PlaybackState state);

protected:
    void closeEvent(QCloseEvent *event) override;
    void changeEvent(QEvent *e) override;
};
#endif // PLAYER_H

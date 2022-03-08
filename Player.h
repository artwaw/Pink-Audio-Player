/*
 * Pink Audio Player - simple audio file player.
 * Copyright (C) <2020-2021>  Artur Wawrowski
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
 * USA
 *
 * Author can be contacted through github issues at
 * https://github.com/artwaw/Pink-Audio-Player
*/

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
#include <QFileSystemModel>
#include <QMediaFormat>

#include "PlaylistManager.h"
#include "PlayingDelegate.h"
#include "PrefsDialog.h"
#include "VMedia.h"

QT_BEGIN_NAMESPACE
    namespace Ui { class Player; }
QT_END_NAMESPACE

 /*!
  * \class Player
 * \brief Main class of the application.
 *
 * This class hosts the main even loop, main UI and integrates all user actions.
 * Member actions are grouped and prefixed.
 */
class Player : public QMainWindow
{
    Q_OBJECT
public:
    Player(QWidget *parent = nullptr);

    /*!
     * \brief Default d'tor
     */
    ~Player();

private:
    Ui::Player *ui; //! Main user interface pointer.
    //menu file actions
    QAction *fileOpen; //! File|Open file
    QAction *fileAddFile; //! File|Add file
    QAction *fileAddDir; //! File|Add folder
    QAction *fileAddLoc; //! File|Add location (i.e. network stream)
    QAction *fileNewPlaylist; //! File|Add new, empty playlist
    QAction *fileOpenPlaylist; //! File|Open saved playlist in the new window
    QAction *fileSavePlaylist; //! File|Save current playlist
    QAction *fileExit; //! File|Close the program
    //menu edit
    QAction *clear; //! Edit|Clear the current playlist
    QAction *selectAll; //! Edit|Select all items in the current playlist
    QAction *search; //! Edit|Search in current playlist
    QAction *removeDuplicates; //! Edit|Find and remove duplicates
    QAction *removeDeadItems; //! Edit|Remove dead (non existent) entries
    QAction *prefs; //! Edit|Preferences dialog
    //menu playback
    QAction *playNext; //! Playback|Play next entry
    QAction *playPrev; //! Playback|Play previous entry
    QAction *playPause; //! Playback|Pause or renew play
    QActionGroup *playOrder; //! Action group holding play order actions
    QAction *orderDefault; //! Order|Play tracks one by one
    QAction *orderRepeatAll; //! Order|Play tracks one by one and don't stop at the end
    QAction *orderSingleTrack; //! Order|Play single track once
    QAction *orderRepeatTrack; //! Order|Repeat single track
    QAction *orderShuffleRepeat; //! Order|Shuffle tracks and play all on repeat
    QAction *stopAfterCurrent; //! Action flag to stop after playing current track
    QActionGroup *followOrder; //! Action group controlling visual cursor follow order
    QAction *playFollowCursor; //! Action flag selecting that next track played is the one selected
    QAction *playFollowsCursor; //! Action flag selecting that next track is from the order (and selection follows)
    //menu library
    QAction *libClear; //! Library|Clear all the items from the db
    QAction *libRebuild; //! Library|Triggers immediate rebuilding of the db
    QAction *libUpdate; //! Library|Triggers immediate update of the library
    //menu language
    QActionGroup *languages; //! Action group for selecting languages
    QAction *langPol; //! Polish language selector
    QAction *langEng; //! English langugae selector
    QAction *langDeu; //! German language selector
    //menu help
    QAction *helpAbout; //! Action triggering the display of "About" box
    QAction *helpIndex; //! Action triggerign the display of the help index
    //context menus
    QMenu *columnContextMenu; //! Context menu for playlist columns visibility selection
    QMenu *tabBarContextMenu; //! Context menu for adding/renaming/etc of playlists
    QMenu *playlistContextMenu; //! Context menu for manipulating items in the playlist

    QTranslator qtTranslator; //! Translator for Qt parts
    QTranslator myTranslator; //! Translator for custom parts
    PlaylistManager playlists; //! Playlist manager instance
    QMediaPlayer player; //! Multimedia player instance
    QAudioOutput output; //! Output engine instance
    QStringList extensions; //! Ready list of file extensions handled
    QPair<QTableView*,int> currentlyPlaying; //! Proxy item holding currently played item's index (row) and plalist it comes from
    QFileSystemModel filetree; //! Filesystem view backend

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
    int nextTrack(const bool prev = false) const;
    int prevTrack() const;
    void setupFileView();

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
    void playNextTrack(const int t_next);
    void playlistContextMenuRequested(const QPoint &point);
    void playThisTrack(const QModelIndex &idx);
    void editTags(const QModelIndex &idx);
    void editMultipleTracks(const QModelIndexList &idxs);

protected:
    void closeEvent(QCloseEvent *event) override;
    void changeEvent(QEvent *e) override;
};
#endif // PLAYER_H

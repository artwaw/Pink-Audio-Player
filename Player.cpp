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

#include "Player.h"
#include "./ui_Player.h"

/*!
 * \brief Main class' c'tor.
 *
 *
 * \param parent Optional parent parameter.
 */
Player::Player(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Player)
{
    ui->setupUi(this);

    QSettings settings;
    initActions();
    if (settings.value("lang").toString()=="pl_PL") {
        langPol->setChecked(true);
    } else if (settings.value("lang").toString()=="de_DE") {
        langDeu->setChecked(true);
    } else {
        langEng->setChecked(true);
    }
    setLanguage(settings.value("lang","en_GB").toString());
    restoreGeometry(settings.value("geo").toByteArray());
    restoreState(settings.value("state").toByteArray());
    ui->splitter->restoreState(settings.value("splitter").toByteArray());
    restorePlaylists();
    ui->playlistView->tabBar()->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->playlistView->tabBar(),&QTabBar::customContextMenuRequested,this,&Player::tabBarContextMenuRequested);
    connect(ui->metaFrame,&QFrame::customContextMenuRequested,this,&Player::trackInfoContextMenuRequested);
    setupPlayer();
    setupFileView();
    setWindowTitle(QCoreApplication::applicationName());
    setWindowIcon(QIcon(":/img/pap-small.png"));
    setWindowIconText(QCoreApplication::applicationName());
}

Player::~Player()
{
    delete ui;
}

/*!
 * \brief Menu and action items init.
 */
void Player::initActions() {
    QMenu *fileMenu = ui->menubar->addMenu(tr("&File"));
    fileOpen = new QAction("&Open...",this);
    fileOpen->setShortcut(QKeySequence::Open);
    connect(fileOpen,&QAction::triggered,this,&Player::doFileOpen);
    fileMenu->addAction(fileOpen);
    fileMenu->addSeparator();
    fileAddFile = new QAction(tr("&Add file..."),this);
    fileAddFile->setShortcut(Qt::META | Qt::Key_O);
    connect(fileAddFile,&QAction::triggered,this,&Player::doAddFile);
    fileMenu->addAction(fileAddFile);
    fileAddDir = new QAction(tr("Add &folder..."),this);
    fileAddDir->setShortcut(Qt::META | Qt::SHIFT | Qt::Key_O);
    connect(fileAddDir,&QAction::triggered,this,&Player::doAddDir);
    fileMenu->addAction(fileAddDir);
    fileAddLoc = new QAction(tr("Add &location..."),this);
    fileAddLoc->setShortcut(Qt::META | Qt::ALT | Qt::Key_O);
    connect(fileAddLoc,&QAction::triggered,this,&Player::doAddLoc);
    fileMenu->addAction(fileAddLoc);
    fileMenu->addSeparator();
    fileNewPlaylist = new QAction(tr("New &playlist"),this);
    fileNewPlaylist->setShortcut(QKeySequence::New);
    connect(fileNewPlaylist,&QAction::triggered,this,&Player::doNewPlaylist);
    fileMenu->addAction(fileNewPlaylist);
    fileOpenPlaylist = new QAction(tr("Op&en playlist..."));
    fileOpenPlaylist->setShortcut(Qt::Key_P);
    connect(fileOpenPlaylist,&QAction::triggered,this,&Player::doOpenPlaylist);
    fileMenu->addAction(fileOpenPlaylist);
    fileSavePlaylist = new QAction(tr("S&ave playlist..."),this);
    fileSavePlaylist->setShortcut(Qt::SHIFT | Qt::Key_P);
    connect(fileSavePlaylist,&QAction::triggered,this,&Player::doSavePlaylist);
    fileMenu->addAction(fileSavePlaylist);
    fileMenu->addSeparator();
    fileExit = new QAction(tr("&Quit"),this);
    fileExit->setShortcut(QKeySequence::Quit);
    connect(fileExit,&QAction::triggered,qApp,&QApplication::quit);
    QMenu *editMenu = ui->menubar->addMenu(tr("&Edit"));
    clear = new QAction(tr("&Clear"),this);
    clear->setShortcut(Qt::META | Qt::Key_Delete);
    connect(clear,&QAction::triggered,this,&Player::doClearPlaylist);
    editMenu->addAction(clear);
    selectAll = new QAction(tr("Select &all"),this);
    selectAll->setShortcut(QKeySequence::SelectAll);
    connect(selectAll,&QAction::triggered,this,[&](){currentlyPlaying.first->selectAll();});
    editMenu->addAction(selectAll);
    search = new QAction(tr("&Search..."),this);
    search->setShortcut(QKeySequence::Find);
    connect(search,&QAction::triggered,this,&Player::doSearch);
    editMenu->addAction(search);
    editMenu->addSeparator();
    removeDuplicates = new QAction(tr("Remove d&uplicates"),this);
    connect(removeDuplicates,&QAction::triggered,this,&Player::doRemoveDuplicates);
    editMenu->addAction(removeDuplicates);
    removeDeadItems = new QAction(tr("Remove dead items"),this);
    connect(removeDeadItems,&QAction::triggered,this,&Player::doKillTheDead);
    editMenu->addAction(removeDeadItems);
    editMenu->addSeparator();
    prefs = new QAction(tr("&Preferences"),this);
    prefs->setShortcut(QKeySequence::Preferences);
    connect(prefs,&QAction::triggered,this,[=](){
        PrefsDialog *dialog = new PrefsDialog(this);
        dialog->open();
        connect(dialog,&PrefsDialog::accepted,dialog,&PrefsDialog::deleteLater);
    });
    editMenu->addAction(prefs);
    QMenu *playMenu = ui->menubar->addMenu(tr("&Playback"));
    playNext = new QAction(tr("&Next"),this);
    playNext->setShortcut(QKeySequence::FindNext);
    connect(playNext,&QAction::triggered,ui->nextBtn,&QPushButton::clicked);
    playMenu->addAction(playNext);
    playPrev = new QAction(tr("Pre&v"),this);
    playPrev->setShortcut(QKeySequence::FindPrevious);
    connect(playPrev,&QAction::triggered,ui->prevBtn,&QPushButton::clicked);
    playMenu->addAction(playPrev);
    playPause = new QAction(tr("&Play/Pause"),this);
    playPause->setShortcut(Qt::Key_Space);
    connect(playPause,&QAction::triggered,ui->playBtn,&QPushButton::clicked);
    playMenu->addAction(playPause);
    playMenu->addSeparator();
    QMenu *orderMenu = playMenu->addMenu(tr("Playback order"));
    playOrder = new QActionGroup(this);
    orderDefault = new QAction(tr("Default"),this);
    orderDefault->setCheckable(true);
    connect(orderDefault,&QAction::triggered,this,[=](){ui->orderBox->setCurrentIndex(0);});
    playOrder->addAction(orderDefault);
    orderRepeatAll = new QAction(tr("Repeat all"),this);
    orderRepeatAll->setCheckable(true);
    connect(orderDefault,&QAction::triggered,this,[=](){ui->orderBox->setCurrentIndex(1);});
    playOrder->addAction(orderRepeatAll);
    orderSingleTrack = new QAction(tr("Play single track"),this);
    orderSingleTrack->setCheckable(true);
    connect(orderDefault,&QAction::triggered,this,[=](){ui->orderBox->setCurrentIndex(2);});
    playOrder->addAction(orderSingleTrack);
    orderRepeatTrack = new QAction(tr("Repeat track"),this);
    orderRepeatTrack->setCheckable(true);
    connect(orderDefault,&QAction::triggered,this,[=](){ui->orderBox->setCurrentIndex(3);});
    playOrder->addAction(orderRepeatTrack);
    orderShuffleRepeat = new QAction(tr("Shuffle repeat"));
    orderShuffleRepeat->setCheckable(true);
    connect(orderDefault,&QAction::triggered,this,[=](){ui->orderBox->setCurrentIndex(4);});
    playOrder->addAction(orderShuffleRepeat);
    orderMenu->addActions(playOrder->actions());
    playMenu->addSeparator();
    stopAfterCurrent = new QAction(tr("Stop after current"),this);
    stopAfterCurrent->setCheckable(true);
    playMenu->addAction(stopAfterCurrent);
    followOrder = new QActionGroup(this);
    playFollowCursor = new QAction(tr("Cursor follows playback"),this);
    playFollowCursor->setCheckable(true);
    followOrder->addAction(playFollowCursor);
    playFollowsCursor = new QAction(tr("Playback follows cursor"),this);
    playFollowsCursor->setCheckable(true);
    followOrder->addAction(playFollowsCursor);
    followOrder->setExclusionPolicy(QActionGroup::ExclusionPolicy::ExclusiveOptional);
    playMenu->addActions(followOrder->actions());
    QMenu *libMenu = ui->menubar->addMenu(tr("&Library"));
    libClear = new QAction(tr("Clear library"));
    connect(libClear,&QAction::triggered,this,&Player::clearLibrary);
    libMenu->addAction(libClear);
    libRebuild = new QAction(tr("Rebuild library"),this);
    connect(libRebuild,&QAction::triggered,this,&Player::rebuildLibrary);
    libMenu->addAction(libRebuild);
    libUpdate = new QAction(tr("Update library"),this);
    connect(libUpdate,&QAction::triggered,this,&Player::updateLibrary);
    libMenu->addAction(libUpdate);
    QMenu *langMenu = ui->menubar->addMenu(tr("La&nguages"));
    languages = new QActionGroup(this);
    langPol = new QAction("&Polski",this);
    langPol->setIcon(QIcon(":/img/pl_PL.png"));
    connect(langPol,&QAction::triggered,this,[=](){setLanguage("pl_PL");});
    languages->addAction(langPol);
    langEng = new QAction("&English",this);
    langEng->setIcon(QIcon(":/img/en_GB.png"));
    connect(langEng,&QAction::triggered,this,[=](){setLanguage("en_GB");});
    languages->addAction(langEng);
    langDeu = new QAction("&Deutsch",this);
    langDeu->setIcon(QIcon(":/img/de_DE.png"));
    connect(langDeu,&QAction::triggered,this,[=](){setLanguage("de_DE");});
    languages->addAction(langDeu);
    langMenu->addActions(languages->actions());
    QMenu *helpMenu = ui->menubar->addMenu(tr("&Help"));
    helpAbout = new QAction(tr("&About..."),this);
    connect(helpAbout,&QAction::triggered,this,&Player::doAbout);
    helpMenu->addAction(helpAbout);
    helpIndex = new QAction(tr("&Index"),this);
    helpIndex->setShortcut(QKeySequence::HelpContents);
    connect(helpIndex,&QAction::triggered,this,&Player::doHelp);
    helpMenu->addAction(helpIndex);
    columnContextMenu = new QMenu(this);
    tabBarContextMenu = new QMenu(this);
    playlistContextMenu = new QMenu(this);
    trackInfoMenu = new QMenu(this);
}

/*!
 * \brief Language change action chain init.
 * \param lang ISO locale inidicator
 */
void Player::setLanguage(const QString &lang) {
    QString loc;
    QSettings settings;
    if (lang.isEmpty()) {
        loc = QLocale::system().name();
        if (loc=="en_US") { loc="en_GB"; }
    } else {
        loc = lang;
    }
    if (qtTranslator.load("qtbase_"+loc.chopped(3),":/i18n")) { qApp->installTranslator(&qtTranslator); }
    if (myTranslator.load("PAP_"+loc,":/i18n")) { qApp->installTranslator(&myTranslator); }
    settings.setValue("lang",loc);
    settings.sync();
}

/*!
 * \brief Routine restoring playlists and their items.
 * \todo Sorting
 */
void Player::restorePlaylists() {
    QSettings settings;
    settings.beginGroup("playlists");
    QStringList all = playlists.getAll();
    QStandardItemModel *model = playlists.retrieve();
    if (!model) {
        QMessageBox::critical(this,tr("Critical error"),tr("Default playlist can't be retrieved. This is a bug. Please contact the developer."));
        close();
        return;
    }
    ui->liveTable->setModel(model);
    ui->liveTable->setShowGrid(false);
    ui->playlistView->setTabText(0,playlists.getLiveTitle());
    decoratePlaylist(ui->liveTable);
    if (all.size()>1) {
        for (auto x=1;x<all.size();++x) {
            QStandardItemModel *model = playlists.retrieve(all.at(x));
            if (model) {
                QWidget *widget = new QWidget();
                QGridLayout *layout = new QGridLayout(widget);
                layout->setContentsMargins(3,3,3,3);
                QTableView *table = new QTableView(widget);
                layout->addWidget(table);
                table->setModel(model);
                ui->liveTable->setShowGrid(false);
                decoratePlaylist(table);
                ui->playlistView->addTab(widget,all.at(x));
            }
        }
    }
    //sorting them here
    ui->playlistView->setCurrentIndex(settings.value("playlistIndex",0).toInt());
    currentlyPlaying.first = ui->playlistView->currentWidget()->findChild<QTableView*>();
    currentlyPlaying.second = -1;
    settings.endGroup();
    connect(ui->playlistView->tabBar(),&QTabBar::tabBarDoubleClicked,this,&Player::renameTab);
    connect(ui->playlistView->tabBar(),&QTabBar::tabCloseRequested,this,&Player::playlistCloseRequested);
    connect(ui->playlistView->tabBar(),&QTabBar::currentChanged,this,&Player::playlistChanged);
}

/*!
 * \brief Routing putting headers into playlists
 * \param view Playlist tab pointer to be decorated
 */
void Player::decoratePlaylist(QTableView *view) {
    if (!view||!view->model()) { return; }
    QSettings settings;
    settings.beginGroup("playlists");
    int x = settings.beginReadArray("visibility");
    if (x==0) {
        settings.endArray();
        settings.beginWriteArray("visibility",10);
        for (auto y=0;y<9;++y) {
            settings.setArrayIndex(y); settings.setValue("v",true);
        }
        settings.setArrayIndex(9); settings.setValue("v",false);
        settings.endArray();
        settings.sync();
        x = settings.beginReadArray("visibility");
    }
    for (auto index=1;index<x+1;++index) {
        settings.setArrayIndex(index);
        view->setColumnHidden(index,!settings.value("v").toBool());
    }
    settings.endArray();
    settings.endGroup();
    PlayingDelegate *delegate = new PlayingDelegate(this);
    view->setItemDelegateForColumn(0,delegate);
    view->horizontalHeader()->setContextMenuPolicy(Qt::CustomContextMenu);
    view->setShowGrid(false);
    view->setSelectionBehavior(QAbstractItemView::SelectRows);
    view->setEditTriggers(QTableView::NoEditTriggers);
    view->setSelectionMode(QAbstractItemView::ExtendedSelection);
    view->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(view->horizontalHeader(),&QHeaderView::customContextMenuRequested,this,&Player::columnContextMenuRequested);
    connect(view,&QTableView::customContextMenuRequested,this,&Player::playlistContextMenuRequested);
    connect(view,&QTableView::doubleClicked,this,&Player::playThisTrack);
}

/*!
 * \brief Routine applying column visibility settings to all playlists
 * \todo Remove debug code
 */
void Player::setPlaylistColumnVisibility() {
    QSettings settings; settings.beginGroup("playlists");
    settings.beginReadArray("visibility");
    for (auto x=0;x<ui->playlistView->count();++x) {
        QTableView *page = ui->playlistView->widget(x)->findChild<QTableView*>();
        if (!page) { qDebug() << "findChild failed"; return; }
        for (auto y=0;y<10;++y) {
            settings.setArrayIndex(y);
            page->setColumnHidden(y,!settings.value("v").toBool());
        }
    }
    settings.endArray();
    settings.endGroup();
}

/*!
 * \brief Routinge returning the unique playlist name
 * \param pname Internal playlist id string
 * \return Unique, visible playlist name
 */
QString Player::getUniquePlaylistName(const QString &pname) const {
    QString name;
    if (pname.isEmpty()) { name = tr("New playlist"); } else { name = pname; }
    if (playlistNameUsed(pname)) {
        int x = 1;
        while (playlistNameUsed(name)) {
            name = QString(pname+" %1").arg(QString::number(x));
            ++x;
        }
    }
    return name;
}

/*!
 * \brief Routine storing visible playlist names
 */
void Player::writePlaylistNames() {
    QSettings settings; settings.beginGroup("playlists");
    settings.beginWriteArray("order");
    for (auto x=0;x<ui->playlistView->tabBar()->count();++x) {
        settings.setArrayIndex(x);
        settings.setValue("name",ui->playlistView->tabBar()->tabText(x));
    }
    settings.endArray(); settings.endGroup(); settings.sync();
}

/*!
 * \brief Routine checking of the playlist name (displayed) is already in use
 * \param aname Name to be tested
 * \return TRUE if used already
 */
bool Player::playlistNameUsed(const QString &aname) const {
    if (aname.isEmpty()) { return false; }
    bool used = false;
    for (auto x=0;x<ui->playlistView->tabBar()->count();++x) {
        used = used || (ui->playlistView->tabBar()->tabText(x) == aname);
    }
    return used;
}

/*!
 * \brief Routine setting up player visuals and actions
 * \todo Refine acceptable extension list, currently hardcoded from educated guess.
 */
void Player::setupPlayer() {
    player.setAudioOutput(&output);
    ui->volumeSlider->setValue(QAudio::convertVolume(output.volume(),QAudio::LinearVolumeScale,QAudio::LogarithmicVolumeScale)*100);
    ui->position->setTracking(false);
    connect(&player,&QMediaPlayer::playbackStateChanged,this,&Player::mediaPlaybackStatusChanged);
    connect(&player,&QMediaPlayer::mediaStatusChanged,this,&Player::mediaStatusChanged);
    connect(&player,&QMediaPlayer::positionChanged,ui->position,&QSlider::setValue);
    connect(&player,&QMediaPlayer::positionChanged,this,[&](qint64 pos){
        QTime time(0,0,0);
        time = time.addMSecs(pos);
        ui->timeLabel->setText(time.toString(time.hour()>0?"hh:mm:ss":"mm:ss"));
    });
    connect(ui->position,&QSlider::sliderMoved,&player,&QMediaPlayer::setPosition);
    connect(ui->stopBtn,&QPushButton::clicked,&player,&QMediaPlayer::stop);
    connect(&player,&QMediaPlayer::durationChanged,this,[&](){
        ui->position->setRange(0,player.duration());
    });
    connect(ui->playBtn,&QPushButton::clicked,this,[&](){
        if (player.playbackState()==QMediaPlayer::PausedState) {
            player.play();
            return;
        }
        if (player.playbackState()==QMediaPlayer::StoppedState) {
            if (player.mediaStatus()==QMediaPlayer::LoadedMedia) {
                player.play();
            } else {
                playNextTrack(nextTrack());
            }
            return;
        }
        if (player.playbackState()==QMediaPlayer::PlayingState) {
            player.pause();
        }
    });
    connect(ui->nextBtn,&QPushButton::clicked,this,[&](){playNextTrack(nextTrack());});
    connect(ui->prevBtn,&QPushButton::clicked,this,[&](){playNextTrack(nextTrack(true));});
    ui->muteBtn->setIcon(QIcon(":/img/volume-up-4-512.png"));
    connect(&output,&QAudioOutput::mutedChanged,this,[&](){
        ui->muteBtn->setIcon(QIcon(output.isMuted()?":/img/mute-2-512.png":":/img/volume-up-4-512.png"));
    });
    connect(ui->muteBtn,&QPushButton::toggled,&output,&QAudioOutput::setMuted);
    ui->volumeSlider->setRange(0,100);
    connect(ui->volumeSlider,&QSlider::valueChanged,this,[&](int vol){
        qreal logVol = QAudio::convertVolume(float(vol)/100.0, QAudio::LogarithmicVolumeScale,QAudio::LinearVolumeScale);
        output.setVolume(logVol);
    });
    QMediaFormat format;
    // this needs to reworked to verify against QMediaFormat::supportedFileFormats, see
    // https://bugreports.qt.io/browse/QTBUG-99011?jql=project%20%3D%20QTBUG%20AND%20text%20~%20%22QMediaFormat%22
    extensions.append({"*.m4a","*.mp4","*.3gp","*.m4b","*.m4p","*.m4r","*.m4v","*.aac","*.mp3","*.wav","*.wave","*.caf",
    "*.ogg","*.flac"});
    //extensions.removeDuplicates();
}

/*!
 * \brief Returns the name of currently active playlist
 * \return Playlist visible name or QString() if not found
 */
QString Player::currentPName() const {
    return ui->playlistView->currentIndex()==0?QString():ui->playlistView->tabText(ui->playlistView->currentIndex());
}

/*!
 * \brief Routine resposible for calculating the next track to be played.
 *
 * The routine takes into the account the playback mode setting.
 * \param prev Jump direction, TRUE if jump backward. Only applicable if playback setting != random and playback doesn't follow the cursor.
 * \return Model row of the next track or -1.
 */
int Player::nextTrack(const bool prev) const {
    if (prev) { return prevTrack(); }
    if (!currentlyPlaying.first) { return -1; }
    if (playFollowsCursor->isChecked()) {
        if (!currentlyPlaying.first->selectionModel()->hasSelection()) {
            return -1;
        }
        return currentlyPlaying.first->selectionModel()->selectedIndexes().at(0).row();
    }
    if (ui->orderBox->currentIndex()==2) {
        return -1;
    }
    if (ui->orderBox->currentIndex()==0) {
        if (currentlyPlaying.second==currentlyPlaying.first->model()->rowCount()-1) {
            return -1;
        }
        return currentlyPlaying.second+1;
    }
    if (ui->orderBox->currentIndex()==1) {
        if (currentlyPlaying.second==currentlyPlaying.first->model()->rowCount()-1) {
            return 0;
        }
        return currentlyPlaying.second+1;
    }
    if (ui->orderBox->currentIndex()==3) {
        return currentlyPlaying.second;
    }
    if (ui->orderBox->currentIndex()==4) {
        return QRandomGenerator::global()->bounded(currentlyPlaying.first->model()->rowCount());
    }
    return -1;
}

/*!
 * \brief Routine returns previous track in order, if applicable.
 * \return Previous track number or -1
 */
int Player::prevTrack() const {
    if (!currentlyPlaying.first) { return -1; }
    if (ui->orderBox->currentIndex()==2) {
        return -1;
    }
    if (ui->orderBox->currentIndex()==0) {
        if (currentlyPlaying.second==0) {
            return -1;
        }
        return currentlyPlaying.second-1;
    }
    if (ui->orderBox->currentIndex()==1) {
        if (currentlyPlaying.second==0) {
            return currentlyPlaying.first->model()->rowCount()-1;
        }
        return currentlyPlaying.second-1;
    }
    if (ui->orderBox->currentIndex()==3) {
        return currentlyPlaying.second;
    }
    if (ui->orderBox->currentIndex()==4) {
        return QRandomGenerator::global()->bounded(currentlyPlaying.first->model()->rowCount());
    }
    return -1;
}

/*!
 * \brief Routine setting up the file model and initialising the display parts of the view.
 * \todo Make appstore compliant (iCloud and attached locations not yet visible).
 */
void Player::setupFileView() {
    QSettings settings;
    filetree.setRootPath(settings.value("filetreepath",QStandardPaths::standardLocations(QStandardPaths::MusicLocation).at(0)).toString());
    filetree.setNameFilters(extensions);
    filetree.setNameFilterDisables(true);
    ui->fileView->setModel(&filetree);
    ui->fileView->setRootIndex(filetree.index(settings.value("filetreepath",QStandardPaths::standardLocations(QStandardPaths::MusicLocation).at(0)).toString()));
    connect(ui->files,&QTabWidget::currentChanged,this,[&](int index){
        ui->viewBox->setEnabled(index==0);
    });
}

/*!
 * \brief Menu action. Clears the current playlist and opens the file.
 *
 * If user cancels the file open action nothing changes.
 */
void Player::doFileOpen() {
    QString file = QFileDialog::getOpenFileName(this,"File",QStandardPaths::standardLocations(QStandardPaths::HomeLocation).at(0),extensions.join(' '));
    if (file.isEmpty()) { return; }
    player.setSource(QUrl::fromLocalFile(file));
    const QString pname = currentPName();
    playlists.clearPlaylist(pname);
    playlists.addEntryToPlaylist(file,pname);
    playNextTrack(nextTrack());
}

/*!
 * \brief Menu action. Adds selected, readable files to the current playlist.
 * No depth scan on folders.
 */
void Player::doAddFile() {
    QStringList files = QFileDialog::getOpenFileNames(this,tr("File(s) to add"),QStandardPaths::standardLocations(QStandardPaths::HomeLocation).at(0),extensions.join(' '));
    if (files.isEmpty()) { return; }
    const QString pname = currentPName();
    for (auto x=0;x<files.size();++x) {
        playlists.addEntryToPlaylist(files.at(x),pname);
    }
}

/*!
 * \brief Menu action. Adds all readable files from the selected folder to the current playlist.
 *
 * Currently only 0-level files are added.
 * \todo Deep folder scan and add for child folders.
 */
void Player::doAddDir() {
    QString folder = QFileDialog::getExistingDirectory(this,tr("Add folder"),QStandardPaths::standardLocations(QStandardPaths::HomeLocation).at(0));
    if (folder.isEmpty()) { return; }
    QStringList files;
    QDirIterator iterator(folder,extensions,QDir::NoDotAndDotDot|QDir::Files|QDir::Readable,QDirIterator::Subdirectories);
    while (iterator.hasNext()) {
        files.append(iterator.next());
    }
    playlists.addEntriesToPlaylist(files,currentPName());
}

/*!
 * \brief Menu action. Adds streamable, network location to the current playlist.
 * \todo IMPLEMENT
 */
void Player::doAddLoc() {

}

/*!
 * \brief Menu action. Creates a new playlist with unique display name.
 */
void Player::doNewPlaylist() {
    QString pname = getUniquePlaylistName(QInputDialog::getText(this,tr("Playlist name"),tr("New playlist name:")));
    playlists.addNew(pname);
    QWidget *widget = new QWidget();
    QGridLayout *layout = new QGridLayout(widget);
    layout->setContentsMargins(3,3,3,3);
    QTableView *table = new QTableView();
    layout->addWidget(table);
    table->setModel(playlists.retrieve(pname));
    decoratePlaylist(table);
    table->setItemDelegateForColumn(0,new PlayingDelegate(this));
    ui->playlistView->addTab(widget,pname);
    writePlaylistNames();
}

/*!
 * \brief Menu action. Loads playlist from file.
 * \todo IMPLEMENT
 */
void Player::doOpenPlaylist() {

}

/*!
 * \brief Menu action. Saves playlist to file.
 * \todo IMPLEMENT
 */
void Player::doSavePlaylist() {

}

/*!
 * \brief Menu action. Clears all items of the current playlist.
 */
void Player::doClearPlaylist() {
    playlists.retrieve(currentPName())->clear();
}

void Player::doSearch() {

}

void Player::doRemoveDuplicates() {

}

void Player::doKillTheDead() {

}

void Player::clearLibrary() {

}

void Player::rebuildLibrary() {

}

void Player::updateLibrary() {

}

void Player::doAbout() {

}

void Player::doHelp() {

}

/*!
 * \brief Pops up context menu for the playlist header allowing the user to change the column visibility.
 * \param point Coordinates of the click within the header view.
 */
void Player::columnContextMenuRequested(const QPoint &point) {
    columnContextMenu->clear();
    QSettings settings;
    settings.beginGroup("playlists");
    settings.beginReadArray("visibility");
    const QStringList titles = playlists.getColumnNames();
    for (auto x=0;x<10;++x) {
        settings.setArrayIndex(x);
        QAction *action = new QAction(titles.at(x),this);
        action->setCheckable(true);
        action->setChecked(settings.value("v").toBool());
        columnContextMenu->addAction(action);
        connect(action,&QAction::triggered,this,&Player::playlistColumnVisibilityChanged);
    }
    settings.endArray(); settings.endGroup();
    columnContextMenu->addSeparator();
    QAction *action = new QAction(tr("Reset"),this);
    connect(action,&QAction::triggered,this,&Player::restorePlaylistColumns);
    columnContextMenu->addAction(action);
    columnContextMenu->popup(ui->playlistView->currentWidget()->mapToGlobal(point));
}

/*!
 * \brief Restores the default column visibility.
 * \sa Player::setPlaylistColumnVisbility()
 */
void Player::restorePlaylistColumns() {
    QSettings settings;
    settings.beginGroup("playlists");
    settings.beginWriteArray("visibility",10);
    for (auto y=0;y<9;++y) {
        settings.setArrayIndex(y); settings.setValue("v",true);
    }
    settings.setArrayIndex(9); settings.setValue("v",false);
    settings.endArray();
    settings.endGroup();
    settings.sync();
    setPlaylistColumnVisibility();
}

/*!
 * \brief Writes down the new playlist columns visibility setup and applies it to all playlists.
 * \sa Player::setPlaylistColumnVisbility()
 */
void Player::playlistColumnVisibilityChanged() {
    QSettings settings;
    settings.beginGroup("playlists");
    settings.beginWriteArray("visibility");
    settings.setArrayIndex(0); settings.setValue("v",true);
    for (auto x=1;x<10;++x) {
        settings.setArrayIndex(x);
        settings.setValue("v",columnContextMenu->actions().at(x)->isChecked());
    }
    settings.endArray(); settings.sync(); settings.endGroup();
    setPlaylistColumnVisibility();
}

/*!
 * \brief Removes the selected playlist and releases the view.
 * \param index Index of the playlist view to be closed.
 */
void Player::playlistCloseRequested(int index) {
    if (index>0) {
        playlists.remove(ui->playlistView->tabBar()->tabText(index));
        ui->playlistView->removeTab(index);
        writePlaylistNames();
    }
}

/*!
 * \brief Stores the newly selected current playlist index.
 * \param index Index of the new playlist in the view.
 */
void Player::playlistChanged(int index) {
    QSettings settings; settings.beginGroup("playlists");
    settings.setValue("playlistIndex",index);
    settings.endGroup(); settings.sync();
}

/*!
 * \brief Handles event where user renames the playlist.
 * \param index The index of the playlist for which the visible name changed.
 */
void Player::renameTab(int index) {
    QString oldName = ui->playlistView->tabText(index);
    QString newName = QInputDialog::getText(this,tr("Rename this playlist"),tr("New playlist name:"),QLineEdit::Normal,oldName);
    if (newName==oldName||newName.isEmpty()) { return; }
    newName = getUniquePlaylistName(newName);
    ui->playlistView->setTabText(index,newName);
    playlists.rename(oldName,newName);
    writePlaylistNames();
}

/*!
 * \brief Pops up context menu for playlist tabs.
 * \param point Coursor coordinates in the tab
 */
void Player::tabBarContextMenuRequested(const QPoint &point) {
    tabBarContextMenu->clear();
    tabBarContextMenu->addAction(fileNewPlaylist);
    tabBarContextMenu->addAction(fileOpenPlaylist);
    tabBarContextMenu->addAction(fileSavePlaylist);
    QAction *closePlaylist = new QAction(tr("Close active playlist"),this);
    connect(closePlaylist,&QAction::triggered,this,[=](){
        playlistCloseRequested(ui->playlistView->currentIndex());
    });
    tabBarContextMenu->addAction(closePlaylist);
    tabBarContextMenu->addSeparator();
    QAction *renamePlaylist = new QAction(tr("Rename active playlist","Option to rename playlist tab"),this);
    connect(renamePlaylist,&QAction::triggered,this,[=](){
        renameTab(ui->playlistView->currentIndex());
    });
    tabBarContextMenu->addAction(renamePlaylist);
    tabBarContextMenu->actions().at(3)->setDisabled(ui->playlistView->currentIndex()==0);
    tabBarContextMenu->popup(ui->playlistView->tabBar()->mapToGlobal(point));
}

/*!
 * \brief Routine handling media status change. Either clears current file indicator or jumps to the next track.
 * \param status New status of QMediaPlayer::MediaStatus
 * \sa Player::mediaPlaybackStatusChanged()
 */
void Player::mediaStatusChanged(QMediaPlayer::MediaStatus status) {
    if (status==QMediaPlayer::EndOfMedia||player.playbackState()==QMediaPlayer::StoppedState) {
        playlists.clearPlaying(currentPName());
    }
    if (status==QMediaPlayer::EndOfMedia) {
        nextTrack()>-1?playNextTrack(nextTrack()):player.stop();
    }
}

/*!
 * \brief Routine handling change in playback status (pause/stop etc). Updates button icons etc.
 * \param state New status of QMediaPlayer::PlaybackStatus
 * \sa Player::mediaStatusChanged()
 */
void Player::mediaPlaybackStatusChanged(QMediaPlayer::PlaybackState state) {
    if (state==QMediaPlayer::PausedState||state==QMediaPlayer::StoppedState) {
        ui->playBtn->setIcon(QIcon(":/img/play-7-512.png"));
        ui->playBtn->setChecked(false);
    } else if (state==QMediaPlayer::PlayingState) {
        ui->playBtn->setIcon(QIcon(":/img/media-pause-512.png"));
        ui->playBtn->setChecked(true);
    }
    if (state==QMediaPlayer::StoppedState) {
        ui->position->setValue(0);
        playlists.clearPlaying(currentPName());
    }
}

/*!
 * \brief Routine actively switching playback to the next track in the selected order.
 * \param t_next Row of the playlist with the next track in order.
 * \sa Player::nextTrack()
 * \sa Player::prevTrack()
 */
void Player::playNextTrack(const int t_next) {
    if (t_next<0) { return; }
    const QString file = currentlyPlaying.first->model()->data(currentlyPlaying.first->model()->index(t_next,9)).toString();
    player.setSource(file);
    player.play();
    playlists.clearPlaying(currentPName());
    playlists.setPlaying(file,currentPName());
    ui->artistLabel->setText(currentlyPlaying.first->model()->data(currentlyPlaying.first->model()->index(t_next,2)).toString());
    ui->titleLabel->setText(currentlyPlaying.first->model()->data(currentlyPlaying.first->model()->index(t_next,1)).toString());
    if (playFollowCursor->isChecked()) {
        currentlyPlaying.first->selectRow(t_next);
    }
    currentlyPlaying.second = t_next;
}

/*!
 * \brief Pops up the context menu for the playlist.
 * \param point Coordinates of the coursor within the playlist view.
 */
void Player::playlistContextMenuRequested(const QPoint &point) {
    playlistContextMenu->clear();
    QAction *playFileAction = new QAction(tr("Play this"),this);
    playFileAction->setEnabled(currentlyPlaying.first->selectionModel()->selectedIndexes().size()==1);
    connect(playFileAction,&QAction::triggered,this,[&](){
        playThisTrack(currentlyPlaying.first->selectionModel()->selectedIndexes().at(0));
    });
    playlistContextMenu->addAction(playFileAction);
    playlistContextMenu->addSeparator();
    QAction *editSingleFile = new QAction(tr("Edit single track"),this);
    editSingleFile->setEnabled(currentlyPlaying.first->selectionModel()->selectedIndexes().size()==1);
    connect(editSingleFile,&QAction::triggered,this,[&](){
        editTags(currentlyPlaying.first->indexAt(playlistContextMenu->pos()));
    });
    playlistContextMenu->addAction(editSingleFile);
    QAction *editMultiple = new QAction(tr("Edit selected tracks"),this);
    editMultiple->setEnabled(currentlyPlaying.first->selectionModel()->selectedIndexes().size()>1);
    connect(editMultiple,&QAction::triggered,this,[&](){
        editMultipleTracks(currentlyPlaying.first->selectionModel()->selectedIndexes());
    });
    playlistContextMenu->addAction(editMultiple);
    playlistContextMenu->addSeparator();
    QAction *removeTrack = new QAction(tr("Remove track"),this);
    removeTrack->setEnabled(currentlyPlaying.first->selectionModel()->selectedIndexes().size()==1);
    connect(removeTrack,&QAction::triggered,this,[&](){
        currentlyPlaying.first->model()->removeRow(currentlyPlaying.first->indexAt(playlistContextMenu->pos()).row());
    });
    playlistContextMenu->addAction(removeTrack);
    QAction *removeSelectedTracks = new QAction(tr("Remove selected"),this);
    removeSelectedTracks->setEnabled(currentlyPlaying.first->selectionModel()->hasSelection());
    connect(removeSelectedTracks,&QAction::triggered,this,[&](){
        while (currentlyPlaying.first->selectionModel()->hasSelection()) {
            currentlyPlaying.first->model()->removeRow(currentlyPlaying.first->selectionModel()->selectedRows().first().row());
        }
    });
    playlistContextMenu->addAction(removeSelectedTracks);
    playlistContextMenu->addAction(clear);
    playlistContextMenu->addSeparator();
    playlistContextMenu->addAction(selectAll);
    playlistContextMenu->popup(currentlyPlaying.first->mapToGlobal(point));
}

/*!
 * \brief Routine selecting the clicked track for the immediate playback.
 * \param idx Model index with the clicked row.
 */
void Player::playThisTrack(const QModelIndex &idx) {
    currentlyPlaying.first = ui->playlistView->currentWidget()->findChild<QTableView*>();
    playNextTrack(idx.row());
}

/*!
 * \brief Routine for editing single file tags
 * \param idx Model index for the track in question
 * \todo IMPLEMENT
 */
void Player::editTags(const QModelIndex &idx) {Q_UNUSED(idx)}

/*!
 * \brief Routine for editing multiple file tags at once
 * \param idxs List of model indexes for the tracks in question
 * \todo IMPLEMENT
 */
void Player::editMultipleTracks(const QModelIndexList &idxs) {Q_UNUSED(idxs)}

/*!
 * \brief Pops up the context menu for the trackinfo.
 * \param point Coordinates of the coursor within the metaFrame view.
 */
void Player::trackInfoContextMenuRequested(const QPoint &point) {
    trackInfoMenu->clear();
    QAction *trackInfo = new QAction(tr("Copy to clipboard"),this);
    trackInfo->setEnabled(player.hasAudio());
    connect(trackInfo,&QAction::triggered,this,[=](){
        QClipboard *clipboard = QApplication::clipboard();
        clipboard->setText(ui->artistLabel->text()+" \""+ui->titleLabel->text()+"\"");
    });
    trackInfoMenu->addAction(trackInfo);
    trackInfoMenu->popup(ui->metaFrame->mapToGlobal(point));
}

/*!
 * \brief Routine called when exiting the program. Saves the program state (and exisitng playlists).
 * \param event QCloseEvent object pointer for the event.
 */
void Player::closeEvent(QCloseEvent *event) {
    Q_UNUSED(event)
    QSettings settings;
    settings.setValue("geo",saveGeometry());
    settings.setValue("state",saveState());
    settings.setValue("splitter",ui->splitter->saveState());
    writePlaylistNames();
}


/*!
 * \brief Language change event handler.
 * \param e QEvent with details of the event.
 */
void Player::changeEvent(QEvent *e) {
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        playlists.retranslateHeaders();
        break;
    default:
        break;
    }
}

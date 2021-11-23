#include "Player.h"
#include "./ui_Player.h"

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
}

Player::~Player()
{
    delete ui;
}

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
    connect(clear,&QAction::triggered,this,[=](){
// reinterpret_cast<QTableView*>(ui->playlistView->currentWidget()->children().at(0))->model()
    });
    editMenu->addAction(clear);
    selectAll = new QAction(tr("Select &all"),this);
    selectAll->setShortcut(QKeySequence::SelectAll);
    connect(selectAll,&QAction::triggered,this,[=](){
        reinterpret_cast<QTableView*>(ui->playlistView->currentWidget()->children().at(0))->selectAll();
    });
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

    });
    editMenu->addAction(prefs);
    QMenu *playMenu = ui->menubar->addMenu(tr("&Playback"));
    playNext = new QAction(tr("&Next"),this);
    playNext->setShortcut(QKeySequence::FindNext);
    playMenu->addAction(playNext);
    playPrev = new QAction(tr("Pre&v"),this);
    playPrev->setShortcut(QKeySequence::FindPrevious);
    playMenu->addAction(playPrev);
    playPause = new QAction(tr("&Play/Pause"),this);
    playPause->setShortcut(Qt::Key_Space);
    playMenu->addAction(playPause);
    playMenu->addSeparator();
    QMenu *orderMenu = playMenu->addMenu(tr("Playback order"));
    playOrder = new QActionGroup(this);
    orderDefault = new QAction(tr("Default"),this);
    orderDefault->setCheckable(true);
    playOrder->addAction(orderDefault);
    orderRepeatAll = new QAction(tr("Repeat all"),this);
    orderRepeatAll->setCheckable(true);
    playOrder->addAction(orderRepeatAll);
    orderSingleTrack = new QAction(tr("Play single track"),this);
    orderSingleTrack->setCheckable(true);
    playOrder->addAction(orderSingleTrack);
    orderRepeatTrack = new QAction(tr("Repeat track"),this);
    orderRepeatTrack->setCheckable(true);
    playOrder->addAction(orderRepeatTrack);
    orderShuflle = new QAction(tr("Shuffle"),this);
    orderShuflle->setCheckable(true);
    playOrder->addAction(orderShuflle);
    orderShuffleRepeat = new QAction(tr("Shuffle repeat"));
    orderShuffleRepeat->setCheckable(true);
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
    for (auto x=0;x<16;++x) {
        QAction *action = new QAction(this);
        action->setCheckable(true);
        columnContextMenu->addAction(action);
        connect(action,&QAction::triggered,this,&Player::playlistColumnVisibilityChanged);
    }
    columnContextMenu->addSeparator();
    QAction *action = new QAction(tr("Reset"),this);
    connect(action,&QAction::triggered,this,&Player::restorePlaylistColumns);
    columnContextMenu->addAction(action);
}

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

void Player::restorePlaylists() {
    QSortFilterProxyModel *model = playlists.retrieve(ui->playlistView->tabBar()->tabText(0));
    if (!model) {
        QMessageBox::critical(this,tr("Critical error"),tr("Default playlist can't be retrieved. This is a bug. Please contact the developer."));
        close();
        return;
    }
    ui->liveTable->setModel(model);
    decoratePlaylist(ui->liveTable);
    QSettings settings;
    settings.beginGroup("playlists");
    int index = settings.beginReadArray("playlists");
    for (auto x=0;x<index;++x) {
        settings.setArrayIndex(x);
        QSortFilterProxyModel *model = playlists.retrieve(settings.value("Playlist").toString());
        if (model) {
            QWidget *widget = new QWidget();
            QGridLayout *layout = new QGridLayout(widget);
            layout->addWidget(widget);
            QTableView *table = new QTableView(widget);
            layout->addWidget(table);
            table->setModel(model);
            decoratePlaylist(table);
            ui->playlistView->addTab(widget,settings.value("Playlist").toString());
        }
    }
    settings.endArray();
    ui->playlistView->setCurrentIndex(settings.value("playlist").toInt());
    settings.endGroup();
}

void Player::decoratePlaylist(QTableView *view) {
    if (!view||!view->model()) { return; }
    QSettings settings;
    settings.beginGroup("playlists");
    int x = settings.beginReadArray("visibility");
    if (x==0) {
        settings.endArray();
        settings.beginWriteArray("visibility",19);
        settings.setArrayIndex(0); settings.setValue("v",false);
        for (auto y=1;y<17;++y) {
            settings.setArrayIndex(y); settings.setValue("v",true);
        }
        settings.setArrayIndex(17); settings.setValue("v",false);
        settings.setArrayIndex(18); settings.setValue("v",false);
        settings.setArrayIndex(19); settings.setValue("v",false);
        settings.endArray();
        settings.sync();
        x = settings.beginReadArray("visibility");
    }
    for (auto index=0;index<x+1;++index) {
        settings.setArrayIndex(index);
        view->setColumnHidden(index,!settings.value("v").toBool());
    }
    settings.endArray();
    settings.endGroup();
    view->horizontalHeader()->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(view->horizontalHeader(),&QHeaderView::customContextMenuRequested,this,&Player::columnContextMenuRequested);
}

void Player::doFileOpen() {

}

void Player::doAddFile() {

}

void Player::doAddDir() {

}

void Player::doAddLoc() {

}

void Player::doNewPlaylist() {

}

void Player::doOpenPlaylist() {

}

void Player::doSavePlaylist() {

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

void Player::columnContextMenuRequested(const QPoint &point) {
    QSettings settings;
    settings.beginGroup("playlists");
    settings.beginReadArray("visibility");
    for (auto x=1;x<17;++x) {
        settings.setArrayIndex(x);
        columnContextMenu->actions().at(x-1)->setChecked(settings.value("v").toBool());
    }
    settings.endArray(); settings.endGroup();
    const QStringList titles = playlists.getColumnNames();
    for (auto x=0;x<titles.size();++x) {
        columnContextMenu->actions().at(x)->setText(titles.at(x));
    }
    columnContextMenu->popup(ui->playlistView->currentWidget()->mapToGlobal(point));
}

void Player::restorePlaylistColumns() {
    QSettings settings;
    settings.beginGroup("playlists");
    settings.beginWriteArray("visibility",19);
    settings.setArrayIndex(0); settings.setValue("v",false);
    for (auto y=1;y<17;++y) {
        settings.setArrayIndex(y); settings.setValue("v",true);
    }
    settings.setArrayIndex(17); settings.setValue("v",false);
    settings.setArrayIndex(18); settings.setValue("v",false);
    settings.setArrayIndex(19); settings.setValue("v",false);
    settings.endArray();
    settings.endGroup();
    settings.sync();
    setPlaylistColumnVisibility();
}

void Player::playlistColumnVisibilityChanged() {
    QSettings settings;
    settings.beginGroup("playlists");
    settings.beginWriteArray("visibility");
    settings.setArrayIndex(0); settings.setValue("v",false);
    for (auto x=1;x<17;++x) {
        settings.setArrayIndex(x);
        settings.setValue("v",columnContextMenu->actions().at(x-1)->isChecked());
    }
    settings.setArrayIndex(17); settings.setValue("v",false);
    settings.setArrayIndex(18); settings.setValue("v",false);
    settings.setArrayIndex(19); settings.setValue("v",false);
    settings.endArray(); settings.sync(); settings.endGroup();
    setPlaylistColumnVisibility();
}

void Player::setPlaylistColumnVisibility() {
    QSettings settings; settings.beginGroup("playlists");
    settings.beginReadArray("visibility");
    for (auto x=0;x<ui->playlistView->count();++x) {
        QTableView *page = ui->playlistView->widget(x)->findChild<QTableView*>();
        if (!page) { qDebug() << "findChild failed"; return; }
        for (auto y=0;y<20;++y) {
            settings.setArrayIndex(y);
            page->setColumnHidden(y,!settings.value("v").toBool());
        }
    }
    settings.endArray();
    settings.endGroup();
}

void Player::closeEvent(QCloseEvent *event) {
    Q_UNUSED(event)
    QSettings settings;
    settings.setValue("geo",saveGeometry());
    settings.setValue("state",saveState());
    settings.setValue("splitter",ui->splitter->saveState());
    settings.sync();
}

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

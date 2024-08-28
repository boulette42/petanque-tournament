#include "main_window.h"
#include "ui_main_window.h"
#include "about_dlg.h"
#include "edit_match_dlg.h"
#include "edit_player_dlg.h"
#include "external_window.h"
#include "lock_screen.h"
#include "match.h"
#include "player.h"
#include "player_model.h"
#include "player_result.h"
#include "player_result_model.h"
#include "round_calculator.h"
#include "round_model.h"
#include "select_window_dlg.h"
#include "settings.h"
#include "show_result_dlg.h"
#include "site.h"
#include "site_model.h"
#include "version.h"
#include "team_result_model.h"
#include "tournament.h"
#include "tournament_simulator.h"
#include <qdatetime.h>
#include <qdebug.h>
#include <qevent.h>
#include <qfiledialog.h>
#include <qmessagebox.h>
#include <qtimer.h>

#if QT_VERSION >= QT_VERSION_CHECK(5,15,0)
# define HAS_SET_TAB_VISIBLE
#endif


namespace {

QString last_load_dir;
QString last_save_dir;

QString currentTournamentName()
{
  return QStringLiteral( "petu_data_%1.json" )
    .arg( QDate::currentDate().toString( QLatin1String( "yyyyMMdd" ) ) );
}

bool isSimulateKey( QKeyEvent const* ke )
{
  if ( !global().simulationEnabled() ) return false;
  return ke->key() == Qt::Key_S
    && ( QApplication::keyboardModifiers() & Qt::ControlModifier )
    && ( QApplication::keyboardModifiers() & Qt::ShiftModifier );
}


class SiteCntValidator : public QValidator
{
public:
  SiteCntValidator( QObject* parent ) : QValidator( parent ) { }

  QValidator::State validate( QString& input, int& /*pos*/ ) const override
  {
    if ( ! input.isEmpty() ) {
      bool ok = false;
      int i = input.toInt( &ok );
      if ( ! ok ) return QValidator::Invalid;
      if ( i < 1 || i > 999 ) return QValidator::Invalid;
    }
    return QValidator::Acceptable;
  }
};


enum TabIdx {
  TI_Player = 0,
  TI_Site,
  TI_Round,
  TI_PlayerResult,
  TI_TeamResult
};


class ItemModelAdaptor : public QAbstractItemModel
{
  QAbstractItemModel const* model_;

public:
  explicit ItemModelAdaptor( QAbstractItemModel* model )
    : model_( model )
  {
  }

  bool exportToCsv( QString const& filename ) const
  {
    if ( !model_ ) return false;
    QFile fo(filename);
    if ( !fo.open( QFile::WriteOnly ) ) return false;
    QChar const delimiter( QChar::fromLatin1('\t') );
    QStringList header;
    for ( int c = 0; c < columnCount( ); ++c ) {
      header << model_->headerData( c, Qt::Horizontal, Qt::DisplayRole ).toString();
    }
    fo.write( header.join( delimiter ).toUtf8() );
    fo.write( "\n" );
    for ( int r = 0; r < rowCount(); ++r ) {
      QStringList line;
      for ( int c = 0; c < columnCount(); ++c ) {
        line << data( index( r, c ), Qt::DisplayRole ).toString();
      }
      fo.write( line.join( delimiter ).toUtf8() );
      fo.write( "\n" );
    }
    return true;
  }

  QModelIndex index(int row, int column, QModelIndex const& parent = QModelIndex()) const override
  { return model_->index( row, column, parent ); }
  QModelIndex parent( QModelIndex const& mi ) const override
  { return model_->parent( mi ); }
  int rowCount( QModelIndex const& parent = QModelIndex() ) const override
  { return model_->rowCount( parent ); }
  int columnCount( QModelIndex const& parent = QModelIndex() ) const override
  { return model_->columnCount( parent ); }
  QVariant data( QModelIndex const& mi, int role ) const override
  { return model_->data( mi, role ); }
};

}  // anonymous namespace


class TabWidgetHelper
{
  QTabWidget* tab_widget_ = nullptr;
  struct TabHidden {
    QWidget* widget = nullptr;
    QString label;
    bool hidden = false;
  };
  QVector<TabHidden> tabs_;

public:
  explicit TabWidgetHelper(QTabWidget* tab_widget)
  {
    tab_widget_ = tab_widget;
#ifndef HAS_SET_TAB_VISIBLE
    if (tab_widget) {
      int const size = tab_widget->count();
      tabs_.resize(size);
      for (int i = 0; i < size; ++i) {
        tabs_[i].widget = tab_widget->widget(i);
        tabs_[i].label = tab_widget->tabText(i);
      }
    }
#endif
  }

  void setTabVisible(int index, bool visible)
  {
#ifdef HAS_SET_TAB_VISIBLE
    tab_widget_->setTabVisible(index, visible);
#else
    if (index < 0 || tabs_.size() <= index) return;
    if (visible) showWidget(index);
    else         hideWidget(index);
#endif
  }

private:
  void hideWidget(int index)
  {
    if (tabs_[index].hidden) return;
    int ipos = effectiveIndex(index);
    tab_widget_->removeTab(ipos);
    tabs_[index].hidden = true;
  }

  void showWidget(int index)
  {
    if (index < 0 || tabs_.size() <= index) return;
    if (!tabs_[index].hidden) return;
    tab_widget_->setUpdatesEnabled(false);
    int ipos = effectiveIndex(index);
    tab_widget_->insertTab(ipos, tabs_[index].widget, tabs_[index].label);
    tabs_[index].hidden = false;
    tab_widget_->setUpdatesEnabled(true);
  }

  int effectiveIndex(int index)
  {
    int ipos = 0;
    for (int i = 0; i < index; ++i) {
      if (!tabs_[i].hidden) ++ipos;
    }
    return ipos;
  }
};


// --- MainWindow ------------------------------------------------------------

MainWindow::MainWindow( Tournament const& tournament )
  : ui_( new Ui_MainWindow )
  , tournament_( new Tournament( tournament ) )
  , player_model_( new PlayerModel( *tournament_ ) )
  , site_model_( new SiteModel( *tournament_ ) )
{
  if ( tournament_->playerList().isEmpty() ) {
    tournament_->loadTournament( currentTournamentName() );
  }
  ui_->setupUi( this );
  tab_widget_helper_.reset( new TabWidgetHelper( ui_->tabWidget ) );
  connect( ui_->actionBackup, &QAction::triggered, this, &MainWindow::exportTournament );
  connect( ui_->actionSettings, &QAction::triggered, this, &MainWindow::editSettings );
  connect( ui_->actionQuit, &QAction::triggered, this, &MainWindow::close );
  connect( ui_->actionPlayerLoad, &QAction::triggered, this, &MainWindow::loadPlayerList );
  connect( ui_->actionPlayerSave, &QAction::triggered, this, &MainWindow::savePlayerList );
  connect( ui_->actionPlayerAdd, &QAction::triggered, this, &MainWindow::addPlayer );
  connect( ui_->actionRoundCreate, &QAction::triggered, this, &MainWindow::newRound );
  connect( ui_->actionRoundFinish, &QAction::triggered, this, &MainWindow::finishRound );
  connect( ui_->actionCreateWindow, &QAction::triggered, this, &MainWindow::createWindow );
  connect( ui_->actionDeleteAllWindows, &QAction::triggered, this, &MainWindow::deleteAllWindows );
  connect( ui_->actionLockScreen, &QAction::triggered, this, &MainWindow::lockScreen );
  connect( ui_->actionAbout, &QAction::triggered, this, &MainWindow::about );
  connect( ui_->pbSetSiteCnt, &QPushButton::clicked, this, &MainWindow::updateSiteCount );
  connect( ui_->tvPlayerList, &QAbstractItemView::activated, this, &MainWindow::playerActivated );
  //connect( ui_->tvSiteList, &QAbstractItemView::activated, this, &MainWindow::siteActivated );
  connect( ui_->tvMatchList, &QAbstractItemView::activated, this, &MainWindow::matchActivated );
  connect( ui_->tvPlayerResultList, &QAbstractItemView::activated, this, &MainWindow::playerResultActivated );
  connect( ui_->tvTeamResultList, &QAbstractItemView::activated, this, &MainWindow::teamResultActivated );
  connect( ui_->cmbRound, SIGNAL(currentIndexChanged(int)), this, SLOT(roundChanged(int)) );
  connect( ui_->tvPlayerList, &QAbstractItemView::customContextMenuRequested, this, &MainWindow::playerListContextMenu );
  connect( ui_->tvSiteList, &QAbstractItemView::customContextMenuRequested, this, &MainWindow::siteListContextMenu );
  connect( ui_->tvMatchList, &QAbstractItemView::customContextMenuRequested, this, &MainWindow::matchListContextMenu );
  connect( ui_->tvPlayerResultList, &QAbstractItemView::customContextMenuRequested, this, &MainWindow::resultListContextMenu );
  connect( ui_->tvTeamResultList, &QAbstractItemView::customContextMenuRequested, this, &MainWindow::resultListContextMenu );
  connect( ui_->actionMarkAllPlayers, &QAction::triggered,
           this, [this]() { player_model_->setAllChecked( true ); } );
  connect( ui_->actionUnmarkAllPlayers, &QAction::triggered,
           this, [this]() { player_model_->setAllChecked( false ); } );
  connect( ui_->actionSimulatePlayers, &QAction::triggered,
           this, &MainWindow::simulateGeneratePlayers );
  connect( ui_->actionMarkAllSites, &QAction::triggered,
           this, [this]() { site_model_->setAllChecked( true ); } );
  connect( ui_->actionUnmarkAllSites, &QAction::triggered,
           this, [this]() { site_model_->setAllChecked( false ); } );
  connect( ui_->actionSimulateResults, &QAction::triggered,
           this, &MainWindow::simulateGenerateResults );
  connect( ui_->actionExportWindow, &QAction::triggered,
    this, &MainWindow::exportWindowToCsv );
  updateStyleSheet( *this );
  QFontMetrics fm( ui_->leSiteCnt->font() );
  ui_->leSiteCnt->setMaximumWidth( fm.horizontalAdvance( QLatin1String( "55555" ) ) );
  ui_->tvPlayerList->setModel( player_model_ );
  ui_->tvSiteList->setModel( site_model_ );
  ui_->leSiteCnt->setValidator( new SiteCntValidator( ui_->leSiteCnt ) );
  initModels();
  int const last_round_idx = tournament_->lastRoundIdx();
  if ( last_round_idx >= 0 ) {
    updateRoundSelect( last_round_idx );
  }
  ui_->tvPlayerList->installEventFilter( this );
  ui_->tvPlayerList->setContextMenuPolicy( Qt::CustomContextMenu );
  ui_->tabPlayer->installEventFilter( this );
  ui_->tvSiteList->installEventFilter( this );
  ui_->tvSiteList->setContextMenuPolicy( Qt::CustomContextMenu );
  ui_->tabSites->installEventFilter( this );
  ui_->leSiteCnt->installEventFilter( this );
  ui_->tvMatchList->installEventFilter( this );
  ui_->tvMatchList->setContextMenuPolicy( Qt::CustomContextMenu );
  ui_->tvPlayerResultList->setContextMenuPolicy( Qt::CustomContextMenu );
  ui_->tvTeamResultList->setContextMenuPolicy( Qt::CustomContextMenu );
  ui_->tabRound->installEventFilter( this );
  player_model_->triggerSort();
  updateView( TabMode::round );
  updateView( TabMode::result );
  activateTab( TabMode::player );
}

MainWindow::~MainWindow() = default;

void MainWindow::activateTab( TabMode mode )
{
  updateView( mode );
  switch ( mode ) {
  case TabMode::player:
    ui_->tabWidget->setCurrentIndex( TI_Player );
    ui_->tvPlayerList->setFocus();
    break;
  case TabMode::site:
    ui_->tabWidget->setCurrentIndex( TI_Site );
    ui_->leSiteCnt->setFocus();
    break;
  case TabMode::round:
    ui_->tabWidget->setCurrentIndex( TI_Round );
    ui_->tvMatchList->setFocus();
    break;
  case TabMode::result:
    if ( global().isTeamMode() ) {
      ui_->tabWidget->setCurrentIndex( TI_TeamResult );
      ui_->tvTeamResultList->setFocus();
    } else {
      ui_->tabWidget->setCurrentIndex( TI_PlayerResult );
      ui_->tvPlayerResultList->setFocus();
    }
    break;
  case TabMode::all:
    break;
  }
}

void MainWindow::exportTournament()
{
  if ( tournament_->playerList().isEmpty() ) return;
  QString filename = QFileDialog::getSaveFileName(
    this,
    tr( "Spieler-Daten speichern"),
    last_save_dir,
    tr( "JSON-Dateien (*.json)" ) );
  if ( filename.isEmpty() ) return;
  last_save_dir = QFileInfo( filename ).absoluteDir().absolutePath();
  saveTournament( filename );
}

void MainWindow::editSettings()
{
  bool const site_enabled = global().siteEnabled();
  int const font_size = global().fontSize();
  int const site_count = global().siteCount();
  bool const team_mode = global().isTeamMode();
  bool const formule_x = global().isFormuleX();
  bool const swiss_simple = global().isSwissSimple();
  if ( global().execDialog( this, tournament_->round( 0 ).isEmpty() ) ) {
    if ( global().siteEnabled() != site_enabled ) {
      round_model_->setRound( round_model_->currentRound() );
    }
    if (team_mode != global().isTeamMode()) {
      updateView(TabMode::player);
    }
    if ( global().siteCount() != site_count ) {
      if ( ! tournament_->isChanged() ) {
        tournament_->setSiteCount( global().siteCount() );
      }
    }
    updateView( TabMode::round );
    if ( global().fontSize() != font_size ) {
      updateStyleSheet( *this );
      updateView( TabMode::all );
    }
    if ( formule_x != global().isFormuleX()
      || swiss_simple != global().isSwissSimple() ) {
      team_result_model_->updateTeamList();
    }
  }
}

bool MainWindow::saveTournament( QString const& filename )
{
  QString error_string;
  if ( ! tournament_->saveTournament( filename, error_string ) ) {
    QMessageBox::warning(
      this,
      tr( "Daten speichern" ),
      tr( "Die Daten konnten nicht gespeichert werden:\n%1" ).arg( error_string ) );
    return false;
  }
  return true;
}

void MainWindow::loadPlayerList()
{
  if ( last_load_dir.isEmpty() ) last_load_dir = global().dataDir();
  QString filename = QFileDialog::getOpenFileName(
    this,
    tr( "Spieler-Daten laden"),
    last_load_dir,
    tr( "CSV-Dateien (*.csv *.txt);;Alle Dateien (*.*)" ) );
  if ( filename.isEmpty() ) return;
  last_load_dir = QFileInfo( filename ).absoluteDir().absolutePath();
  if ( tournament_->isChanged() ) {
    if ( ! saveTournament( currentTournamentName() ) ) {
      return;
    }
  }
  if ( tournament_->loadPlayerList( filename ) ) {
    player_model_->triggerSort();
  }
  initModels();
  activateTab( TabMode::player );
}

void MainWindow::savePlayerList()
{
  if ( last_save_dir.isEmpty() ) {
    last_save_dir = last_load_dir.isEmpty() ? global().dataDir() : last_load_dir;
  }
  if ( player_model_->isEmpty() ) return;
  QString filename = QFileDialog::getSaveFileName(
    this,
    tr( "Spieler-Daten speichern"),
    last_save_dir,
    tr( "CSV-Dateien (*.csv *.txt)" ) );
  if ( filename.isEmpty() ) return;
  last_save_dir = QFileInfo( filename ).absoluteDir().absolutePath();
  tournament_->savePlayerList( filename );
}

void MainWindow::addPlayer()
{
  activateTab( TabMode::player );
  EditPlayerDlg dlg( this );
  if ( dlg.exec( Player(), global().isTeamMode() ) ) {
    player_model_->addPlayer( dlg.player() );
  }
}

void MainWindow::newRound()
{
  QString title = tr( "Neue Runde erzeugen" );
  int const n_player = tournament_->selectedPlayerCount();
  int const min_player = global().isTeamMode() ? 4 : 8;
  if ( n_player < min_player ) {
    QMessageBox::warning(
      this,
      title,
      tr( "Es ist nur %n Spieler ausgewählt", "", n_player ) +
      tr( ", für eine neue Runde benötigt man %1." ).arg( min_player ) );
    activateTab( TabMode::player );
    return;
  }
  if ( global().siteEnabled() ) {
    int const n_sites = tournament_->selectedSiteCount();
    int const n_needed = tournament_->neededSites();
    if ( n_sites < n_needed ) {
      QMessageBox::warning(
        this,
        title,
        tr( "Es ist nur %n Platz ausgewählt", "", n_sites ) +
        tr( ", benötigt werden %1 Plätze." ).arg( n_needed ) );
      activateTab( TabMode::site );
      return;
    }
  }
  bool last_round_finished = true;
  int round_idx = tournament_->lastRoundIdx();
  if ( round_idx == INVALID_IDX ) {
    // erste Runde
    round_idx = 0;
  } else if ( round_idx == round_model_->currentRound() ) {
    // normalfall: letzte runde wird eingegeben
    switch ( round_model_->roundStatus() ) {
    case RoundStatus::defined: {
      QMessageBox::StandardButton button = QMessageBox::warning(
        this,
        title,
        tr( "Die aktuelle Runde hat noch nicht begonnen.\n" \
          "Soll diese Runde überschrieben werden?" ),
        QMessageBox::Yes | QMessageBox::No );
      if ( button != QMessageBox::Yes ) return;
      break; }
    case RoundStatus::started:
      last_round_finished = false;
      break;
    case RoundStatus::ready:
      finishRound();
      ++round_idx;
    }
  } else if ( tournament_->lastRoundFinished() ) {
    // andere runde wurde angezeigt/editiert, letzte runde ist bereits fertig
    finishRound();
    ++round_idx;
  } else {
    // andere runde wurde angezeigt/editiert, letzte runde ist nicht fertig
    finishRound();
    last_round_finished = false;
  }
  if ( !last_round_finished ) {
    QMessageBox::warning(
      this,
      title,
      tr( "Eine neue Runde kann nicht erzeugt werden, wenn die " \
        "aktuelle Runde noch nicht beendet ist." ) );
    return;
  } else if ( !global().isTeamMode() && round_idx >= 3 ) {
    QMessageBox::warning(
      this,
      title,
      tr( "Im Supermelee-Modus werden nur drei Runden unterstützt." ) );
    return;
  } else {
    RoundCalculator round_calculator( ui_->tabWidget, round_idx );
    if ( ! tournament_->createRound( round_calculator ) ) {
      QMessageBox::warning( this, title, round_calculator.lastError() );
      return;
    }
  }
  updateRoundSelect( round_idx );
  activateTab( TabMode::round );
}

void MainWindow::initModels()
{
  deleteAllWindows();
  round_model_ = new RoundModel( *tournament_ );
  ui_->tvMatchList->setModel( round_model_ );
  player_result_model_ = new PlayerResultModel( *tournament_ );
  ui_->tvPlayerResultList->setModel( player_result_model_ );
  team_result_model_ = new TeamResultModel( *tournament_ );
  ui_->tvTeamResultList->setModel( team_result_model_ );
  ui_->tvTeamResultList->header()->setMinimumSectionSize( 1 );
  ui_->cmbRound->clear();
  ui_->leSiteCnt->setText( QString::number( tournament_->siteCount() ) );
}

void MainWindow::updateView( TabMode tm )
{
  bool const team_mode = global().isTeamMode();
  tab_widget_helper_->setTabVisible( TI_Site, global().siteEnabled() );
  tab_widget_helper_->setTabVisible( TI_PlayerResult, ! team_mode );
  tab_widget_helper_->setTabVisible( TI_TeamResult, team_mode );

  if ( tm == TabMode::player || tm == TabMode::all ) {
    for ( int col = 0; col < player_model_->columnCount() - 1; ++col ) {
      ui_->tvPlayerList->resizeColumnToContents( col );
    }
  }
  if ( tm == TabMode::all ) {
    for ( int col = 0; col < site_model_->columnCount() - 1; ++col ) {
      ui_->tvSiteList->resizeColumnToContents( col );
    }
  }
  if ( tm == TabMode::round || tm == TabMode::all ) {
    ui_->tvMatchList->header()->setVisible( global().isTeamOnlyShown() );
    ui_->tvMatchList->setSortingEnabled( global().isTeamOnlyShown() );
    for ( int col = 0; col < round_model_->columnCount() - 1; ++col ) {
      ui_->tvMatchList->resizeColumnToContents( col );
    }
  }
  if ( tm == TabMode::result || tm == TabMode::all ) {
    if ( team_mode ) {
      for ( int col = 0; col < team_result_model_->columnCount() - 1; ++col ) {
        ui_->tvTeamResultList->resizeColumnToContents( col );
      }
    } else {
      for ( int col = 0; col < player_result_model_->columnCount() - 1; ++col ) {
        ui_->tvPlayerResultList->resizeColumnToContents( col );
      }
    }
  }
}

void MainWindow::updateRoundSelect( int round_idx )
{
  ui_->cmbRound->blockSignals( true );
  ui_->cmbRound->clear();
  for ( int ri = ui_->cmbRound->count(); ri <= round_idx; ++ri ) {
    ui_->cmbRound->addItem( tr( "Runde %1" ).arg( ri + 1 ) );
    ui_->cmbRound->setCurrentIndex( ri );
    round_model_->setRound( ri );
    if ( round_model_->roundStatus() == RoundStatus::ready ) {
      round_model_->finishRound();
    }
  }
  ui_->cmbRound->setCurrentIndex( round_idx );
  round_model_->setRound( round_idx );
  ui_->cmbRound->blockSignals( false );
  ui_->cmbRound->show();
  player_result_model_->updatePlayerList();
  team_result_model_->updateTeamList();
  foreach( QSharedPointer<ExternalWindow> p, window_register_ ) {
    p->updateRound( round_idx );
  }
}

void MainWindow::finishRound()
{
  round_model_->finishRound();
  if ( global().isTeamMode() ) {
    team_result_model_->updateTeamList();
    for ( int col = 0; col < team_result_model_->columnCount() - 1; ++col ) {
      ui_->tvTeamResultList->resizeColumnToContents( col );
    }
  } else {
    player_result_model_->updatePlayerList();
    for ( int col = 0; col < player_result_model_->columnCount() - 1; ++col ) {
      ui_->tvPlayerResultList->resizeColumnToContents( col );
    }
  }
}

void MainWindow::updateSiteCount()
{
  QString v = ui_->leSiteCnt->text();
  if ( site_model_->setSiteCount( v.toInt() ) ) {
    ui_->tvSiteList->show();
  } else {
    QMessageBox::warning(
      this,
      tr( "Platzanzahl aktualisieren" ),
      tr( "Die Platzanzahl konnten nicht übernommen werden" ) );
  }
}

void MainWindow::createWindow()
{
  SelectWindowDlg swd( this );
  if ( ! swd.exec() ) return;
  QAbstractItemModel* model = nullptr;
  if ( swd.isRoundWindow() ) {
    model = round_model_;
  }
  else if ( swd.isResultWindow() ) {
    if ( global().isTeamMode() ) {
      model = team_result_model_;
    }
    else {
      model = player_result_model_;
    }
  }
  else {
    model = player_model_;
  }
  QSharedPointer<ExternalWindow> external_window( new ExternalWindow( *model, swd.fontSize() ) );
  if ( swd.screen() > 0 ) {
    external_window->setScreen( swd.screen() );
  }
  external_window->show();
  window_register_.append( external_window );
}

void MainWindow::deleteAllWindows()
{
  foreach( QSharedPointer<ExternalWindow> p, window_register_ ) {
    p->close();
    p = nullptr;
  }
  window_register_.clear();
}

void MainWindow::lockScreen()
{
  LockScreen( this ).exec();
}

void MainWindow::about()
{
  AboutDlg( this ).exec();
}

void MainWindow::playerActivated( QModelIndex const& index )
{
  EditPlayerDlg dlg( this );
  Player p = player_model_->player( index );
  if ( dlg.exec( p, global().isTeamMode() ) ) {
    player_model_->setPlayer( index, dlg.player() );
  }
}

void MainWindow::matchActivated( QModelIndex const& index )
{
  EditMatchDlg dlg( this );
  Match m = round_model_->match( index );
  int m_idx = round_model_->matchIndex( index );
  if ( dlg.exec( m, m_idx + 1, *tournament_ ) ) {
    round_model_->setResult( index, dlg.pointsLeft(), dlg.pointsRight() );
  }
}

void MainWindow::playerResultActivated( QModelIndex const& mi )
{
  ShowResultDlg dlg( this );
  Player player( player_result_model_->getPlayer( mi ) );
  dlg.exec( *tournament_, player );
}

void MainWindow::teamResultActivated( QModelIndex const& mi )
{
  ShowResultDlg dlg( this );
  Team team( team_result_model_->getTeam( mi ) );
  dlg.exec( *tournament_, team );
}

void MainWindow::roundChanged( int round_idx )
{
  if ( round_model_->isRoundChanged() ) {
    switch ( QMessageBox::warning(
      this,
      tr( "Runde wechseln" ),
      tr( "In der Runde gibt es ungespeicherte Ergegbnisse.\n" \
          "Sie können gespeichert oder verworfen werden." ),
      QMessageBox::Save,
      QMessageBox::Discard ) )
    {
    case QMessageBox::Save:
      finishRound();
      break;
    case QMessageBox::Discard:
      break;
    default:
      ui_->cmbRound->blockSignals( true );
      ui_->cmbRound->setCurrentIndex( round_model_->currentRound() );
      ui_->cmbRound->blockSignals( false );
      return;
    }
  }
  round_model_->setRound( round_idx );
  foreach( QSharedPointer<ExternalWindow> p, window_register_ ) {
    p->updateRound( round_idx );
  }
  updateView( TabMode::round );
}

bool MainWindow::eventFilter( QObject* obj, QEvent *evt )
{
  if ( evt->type() == QEvent::KeyPress ) {
    if ( QKeyEvent* ke = dynamic_cast<QKeyEvent*>( evt ) ) {
      switch ( ui_->tabWidget->currentIndex() ) {
      case TI_Player:
        if ( ke->key() == Qt::Key_Space ) {
          player_model_->toggleChecked( ui_->tvPlayerList->currentIndex() );
          return true;
        } else if ( ke->key() == Qt::Key_A && ( ke->modifiers() & Qt::ControlModifier ) ) {
          bool checked = ! ( ke->modifiers() & Qt::ShiftModifier );
          player_model_->setAllChecked( checked );
          return true;
        } else if ( isSimulateKey( ke ) ) {
          simulateGeneratePlayers();
          return true;
        }
        break;
      case TI_Site:
        if ( ke->key() == Qt::Key_A && ( ke->modifiers() & Qt::ControlModifier ) ) {
          bool checked = ! ( ke->modifiers() & Qt::ShiftModifier );
          site_model_->setAllChecked( checked );
          return true;
        }
        break;
      case TI_Round:
        if ( isSimulateKey( ke ) ) {
          simulateGenerateResults();
          return true;
        }
        break;
      }
    }
  }
  return QMainWindow::eventFilter( obj, evt );
}

void MainWindow::closeEvent( QCloseEvent* event )
{
  event->ignore();
  if ( ! tournament_->isChanged() || saveTournament( currentTournamentName() ) ) {
    deleteAllWindows();
    event->accept();
  }
}

void MainWindow::playerListContextMenu( const QPoint& pos )
{
  context_menu_ = new QMenu( ui_->tvPlayerList );
  context_menu_->addAction( ui_->actionMarkAllPlayers );
  context_menu_->addAction( ui_->actionUnmarkAllPlayers );
  if ( global().simulationEnabled() ) {
    context_menu_->addAction( ui_->actionSimulatePlayers );
  }
  context_menu_->popup( ui_->tvPlayerList->mapToGlobal( pos ) );
}

void MainWindow::siteListContextMenu( const QPoint& pos )
{
  context_menu_ = new QMenu( ui_->tvPlayerList );
  context_menu_->addAction( ui_->actionMarkAllSites );
  context_menu_->addAction( ui_->actionUnmarkAllSites );
  context_menu_->popup( ui_->tvSiteList->mapToGlobal( pos ) );
}

void MainWindow::matchListContextMenu( const QPoint& pos )
{
  if ( !global().simulationEnabled() ) return;
  context_menu_ = new QMenu( ui_->tvMatchList );
  context_menu_->addAction( ui_->actionSimulateResults );
  context_menu_->popup( ui_->tvPlayerList->mapToGlobal( pos ) );
}

void MainWindow::resultListContextMenu( const QPoint& pos )
{
  QTreeView* tv = ui_->tabWidget->currentIndex() == TI_PlayerResult
    ? ui_->tvPlayerResultList
    : ui_->tvTeamResultList;
  context_menu_ = new QMenu( tv );
  context_menu_->addAction( ui_->actionExportWindow );
  context_menu_->popup( ui_->tvPlayerList->mapToGlobal( pos ) );
}

void MainWindow::exportWindowToCsv()
{
  if ( tournament_->lastRoundIdx() < 0 ) return;
  QAbstractItemModel* model = nullptr;
  switch ( ui_->tabWidget->currentIndex() ) {
  case TI_PlayerResult:
    model = player_result_model_;
    break;
  case TI_TeamResult:
    model = team_result_model_;
    break;
  }
  if ( !model || model->rowCount() < 1 ) return;
  if ( last_save_dir.isEmpty() ) {
    last_save_dir = last_load_dir.isEmpty() ? global().dataDir() : last_load_dir;
  }
  QString filename = QFileDialog::getSaveFileName(
    this,
    tr( "Ergebnisse exportieren"),
    last_save_dir,
    tr( "CSV-Dateien (*.csv *.txt)" ) );
  if ( filename.isEmpty() ) return;
  last_save_dir = QFileInfo( filename ).absoluteDir().absolutePath();
  ItemModelAdaptor( model ).exportToCsv( filename );
}

void MainWindow::simulateGeneratePlayers()
{
  QTimer::singleShot( 10, [this]()
  {
    PlayerList pl = TournamentSimulator( *tournament_ ).createRandomPlayers( this );
    if ( ! pl.isEmpty() ) {
      foreach( Player const& p, pl ) {
        player_model_->addPlayer( p );
      }
      updateView( TabMode::player );
    }
  } );
}

void MainWindow::simulateGenerateResults()
{
  QTimer::singleShot( 10, [this]()
  {
    int r_idx = round_model_->currentRound();
    Round round = TournamentSimulator::createRandomResults( tournament_->round( r_idx ) );
    tournament_->setRound( r_idx, round );
    round_model_->setRound( r_idx );
    finishRound();
    updateView( TabMode::round );
  } );
}

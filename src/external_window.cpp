#include "external_window.h"
#include "player_model.h"
#include "round_model.h"
#include "settings.h"
#include <qapplication.h>
#include <qdesktopwidget.h>
#include <qdialog.h>
#include <qlayout.h>
#include <qtreeview.h>
#include <qwindow.h>


ExternalWindow::ExternalWindow( QAbstractItemModel& model, int font_size )
  : dlg_( new QDialog )
  , tree_view_( new QTreeView )
{
  updateStyleSheet( *dlg_, font_size );
  tree_view_->setStyleSheet( QStringLiteral( 
    "QHeaderView::section { border: 0px; padding: 0 15px }\n"
    "QTreeView::item { border: 0px; padding: 0 15px }\n" )
    );
  QVBoxLayout* layout = new QVBoxLayout( dlg_ );
  if ( RoundModel* round_model = dynamic_cast<RoundModel *>( &model ) ) {
    dlg_->setWindowTitle( tr( "Runde %1" )
      .arg( round_model->currentRound() + 1 ) );
    tree_view_->setHeaderHidden( true );
  }
  else if ( dynamic_cast<PlayerModel *>( &model ) ) {
    dlg_->setWindowTitle( tr( "Spieler" ) );
  }
  else {
    dlg_->setWindowTitle( tr( "Ergebnisse" ) );
  }
  tree_view_->setModel( &model );
  tree_view_->setRootIsDecorated( false );
  layout->addWidget( tree_view_ );
}

ExternalWindow::~ExternalWindow()
{
  close();
}

void ExternalWindow::show()
{
  if ( screen_ > 0 ) {
    dlg_->show();
    dlg_->windowHandle()->setScreen( qApp->screens()[screen_] );
    dlg_->showFullScreen();
  }
  else {
    QRect rec = QApplication::desktop()->screenGeometry();
    QSize size( rec.width() / 2, rec.height() / 2 );
    dlg_->resize( size );
    dlg_->show();
  }
  for ( int col = 0; col < tree_view_->model()->columnCount() - 1; ++col ) {
    tree_view_->resizeColumnToContents( col );
  }
}

void ExternalWindow::close()
{
  if (dlg_) {
    dlg_->close();
    dlg_ = nullptr;
  }
}

void ExternalWindow::updateRound( int round_idx )
{
  if ( dlg_ && dlg_->windowTitle().startsWith( tr( "Runde" ) ) ) {
    dlg_->setWindowTitle( tr( "Runde %1" ).arg( round_idx + 1 ) );
  }
}

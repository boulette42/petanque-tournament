#include "external_window.h"
#include "round_model.h"
#include "settings.h"
#include <qapplication.h>
#include <qdesktopwidget.h>
#include <qdialog.h>
#include <qlayout.h>
#include <qtreeview.h>

namespace {

  QString roundTitle()
  {
    return tr( "Runde" );
  }
}


ExternalWindow::ExternalWindow( QAbstractItemModel& model, int font_size )
  : dlg_( new QDialog )
{
  updateStyleSheet( *dlg_, font_size );
  QVBoxLayout* layout = new QVBoxLayout( dlg_ );
  QTreeView* tree_view = new QTreeView;
  RoundModel* round_model = dynamic_cast<RoundModel *>( &model );
  if ( round_model ) {
    dlg_->setWindowTitle( QStringLiteral( "%1 %2" )
      .arg( roundTitle() )
      .arg( round_model->currentRound() + 1 ) );
    tree_view->setHeaderHidden( true );
  }
  else {
    dlg_->setWindowTitle( tr( "Ergebnisse" ) );
  }
  tree_view->setModel( &model );
  tree_view->setRootIsDecorated( false );
  layout->addWidget( tree_view );
  QRect rec = QApplication::desktop()->screenGeometry();
  QSize size( rec.width()/2, rec.height()/2 );
  dlg_->resize(size);
  dlg_->show();
  for ( int col = 0; col < model.columnCount() - 1; ++col ) {
    tree_view->resizeColumnToContents( col );
  }
}

ExternalWindow::~ExternalWindow()
{
  close();
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
  if ( dlg_ && dlg_->windowTitle().startsWith( roundTitle() ) ) {
    dlg_->setWindowTitle( QStringLiteral( "%1 %2" )
      .arg( roundTitle() )
      .arg( round_idx + 1 ) );
  }
}

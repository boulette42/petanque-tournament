#include "lock_screen.h"
#include "settings.h"
#include <qdialog.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qpushbutton.h>


LockScreen::LockScreen( QWidget* parent )
  : dlg_( new QDialog( parent ) )
  , le_password_( new QLineEdit( dlg_ ) )
{
  int const font_size = 25;
  updateStyleSheet( *dlg_, font_size );
  QHBoxLayout* h_layout = new QHBoxLayout( dlg_ );
  h_layout->addItem( new QSpacerItem( 200, 5, QSizePolicy::Fixed ) );
  h_layout->addWidget( new QLabel( tr( "Passwort:" ), dlg_ ) );
  le_password_->setEchoMode( QLineEdit::Password );
  QFontMetrics fm( le_password_->font() );
  int w = fm.horizontalAdvance( QLatin1String( "5" ) );
  le_password_->setMaximumWidth( 30*w );
  h_layout->addWidget( le_password_ );
  QPushButton* pb_open = new QPushButton( tr( "Entsperren" ), dlg_ );
  pb_open ->connect( pb_open , &QPushButton::pressed, dlg_, &QDialog::accept );
  h_layout->addWidget( pb_open );
  h_layout->addItem( new QSpacerItem( 5, 5, QSizePolicy::Expanding ) );
  QVBoxLayout* v_layout = new QVBoxLayout( dlg_ );
  h_layout->addItem( new QSpacerItem( 5, 200, QSizePolicy::Minimum, QSizePolicy::Fixed ) );
  v_layout->addLayout( h_layout );
  h_layout->addItem( new QSpacerItem( 5, 100, QSizePolicy::Minimum, QSizePolicy::Expanding ) );
}

LockScreen::~LockScreen() = default;

void LockScreen::exec()
{
  dlg_->showFullScreen();
  do {
    dlg_->exec();
  } while ( le_password_->text() != global().password() );
}

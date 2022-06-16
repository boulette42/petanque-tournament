#include "edit_player_dlg.h"
#include "ui_edit_player_dlg.h"
#include "match.h"
#include "player.h"
#include "player_result.h"
#include "settings.h"
#include <qdialog.h>
#include <qvalidator.h>


namespace {

class PointsValidator : public QValidator
{
public:
  PointsValidator( QObject* parent ) : QValidator( parent ) { }

  QValidator::State validate( QString& input, int& /*pos*/ ) const override
  {
    if ( ! input.isEmpty() ) {
      bool ok = false;
      int i = input.toInt( &ok );
      if ( ! ok ) return QValidator::Invalid;
      if ( i < 0 || i > 9999 ) return QValidator::Invalid;
    }
    return QValidator::Acceptable;
  }
};

}


EditPlayerDlg::EditPlayerDlg( QWidget* parent )
  : dlg_( new QDialog( parent ) )
  , ui_( new Ui_EditPlayerDialog() )
  , player_( new Player() )
{
  ui_->setupUi( dlg_ );
  QRegExp rx( QLatin1String( "\\S+.*" ) );
  ui_->leName->setValidator( new QRegExpValidator( rx ) );
  updateStyleSheet( *dlg_ );
  QFontMetrics fm( ui_->lePoints->font() );
  ui_->lePoints->setMaximumWidth( fm.horizontalAdvance( QLatin1String( "555555" ) ) );
  ui_->lePoints->setValidator( new PointsValidator( dlg_ ) );
}

EditPlayerDlg::~EditPlayerDlg() = default;

bool EditPlayerDlg::exec( Player const& player, bool team_mode )
{
  if ( player_->id() != player.id() ) {
    *player_ = player;
  }
  dlg_->setWindowTitle( dlg_->tr( "Spieler %1" ).arg( player.id() ) );
  ui_->tlAssoc->setText( team_mode ? tr( "Team" ) : tr( "Verein" ) );
  ui_->leName->setText( player.name() );
  ui_->leVorname->setText( player.vorname() );
  ui_->leAssoc->setText( player.verein() );
  ui_->lePoints->setText( QString::number( player.points() ) );
  bool selected = player.selected();
  if ( ! dlg_->exec() ) return false;
  *player_ = Player(
    player_->id(),
    ui_->leName->text(),
    ui_->leVorname->text(),
    ui_->leAssoc->text(),
    ui_->lePoints->text().toInt() );
  player_->setSelected( selected );
  return true;
}

Player const& EditPlayerDlg::player() const
{
  return *player_;
}


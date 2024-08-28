#include "edit_match_dlg.h"
#include "ui_edit_match_dlg.h"
#include "match.h"
#include "player.h"
#include "settings.h"
#include "tournament.h"
#include <qdialog.h>
#include <qvalidator.h>


namespace {

void setLabel( QLabel* tl, int id, Tournament const& tournament )
{
  tl->setText( tournament.playerName( id ) );
}

class ResultValidator : public QValidator
{
  QLineEdit* pair_;

public:
  ResultValidator( QObject* parent, QLineEdit* pair )
    : QValidator( parent )
    , pair_( pair )
  {
  }

  QValidator::State validate( QString& input, int& /*pos*/ ) const override
  {
    if ( ! input.isEmpty() ) {
      bool ok = false;
      int i = input.toInt( &ok );
      if ( ! ok ) return QValidator::Invalid;
      if ( i < 0 || i > 13 ) return QValidator::Invalid;
      if ( i == 13 ) {
        if ( pair_->text().toInt() > 12 ) return QValidator::Invalid;
      }
    }
    return QValidator::Acceptable;
  }
};

}


EditMatchDlg::EditMatchDlg( QWidget* parent )
  : dlg_( new QDialog( parent ) )
  , ui_( new Ui_EditMatchDialog() )
{
  ui_->setupUi( dlg_ );
  updateStyleSheet( *dlg_ );
  QFontMetrics fm( ui_->lePtsLt->font() );
  int w = fm.horizontalAdvance( QLatin1String( "5555" ) );
  ui_->lePtsLt->setMaximumWidth( w );
  ui_->lePtsRt->setMaximumWidth( w );
  ui_->lePtsLt->setValidator( new ResultValidator( dlg_, ui_->lePtsRt ) );
  ui_->lePtsRt->setValidator( new ResultValidator( dlg_, ui_->lePtsLt ) );
}

EditMatchDlg::~EditMatchDlg() = default;

bool EditMatchDlg::exec( Match const& match, int mno, Tournament const& tournament )
{
  QString title;
  if ( global().isTeamMode() ) {
    int p_id_lt = match.team_lt_.playerId( 0 );
    int p_id_rt = match.team_rt_.playerId( 0 );
    title = QStringLiteral( "%1 - %2" )
      .arg( tournament.player( p_id_lt ).team() )
      .arg( tournament.player( p_id_rt ).team() );
  } else {
    if ( global().siteEnabled() ) {
      title = tournament.siteName( match.site_id_ );
    } else {
      title = tr( "Match %1" ).arg( mno );
    }
  }
  dlg_->setWindowTitle( title );
  setLabel( ui_->tlPlayer1_1, match.team_lt_.playerId( 0 ), tournament );
  setLabel( ui_->tlPlayer1_2, match.team_lt_.playerId( 1 ), tournament );
  setLabel( ui_->tlPlayer1_3, match.team_lt_.playerId( 2 ), tournament );
  setLabel( ui_->tlPlayer2_1, match.team_rt_.playerId( 0 ), tournament );
  setLabel( ui_->tlPlayer2_2, match.team_rt_.playerId( 1 ), tournament );
  setLabel( ui_->tlPlayer2_3, match.team_rt_.playerId( 2 ), tournament );
  int pts_lt = match.result_.pointsLeft();
  int pts_rt = match.result_.pointsRight();
  ui_->lePtsLt->setText( pts_lt || pts_rt ? QString::number( pts_lt ) : QString() );
  ui_->lePtsRt->setText( pts_rt || pts_lt ? QString::number( pts_rt ) : QString() );
  if ( ! dlg_->exec() ) return false;
  return pointsLeft() != pts_lt || pointsRight() != pts_rt;
}

int EditMatchDlg::pointsLeft() const
{
  return ui_->lePtsLt->text().toInt();
}

int EditMatchDlg::pointsRight() const
{
  return ui_->lePtsRt->text().toInt();
}

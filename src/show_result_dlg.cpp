#include "show_result_dlg.h"
#include "ui_show_result_dlg.h"
#include "match.h"
#include "settings.h"
#include "player.h"
#include "player_result.h"
#include "round_model.h"
#include "tournament.h"
#include <qdialog.h>


ShowResultDlg::ShowResultDlg( QWidget* parent )
  : dlg_( new QDialog( parent ) )
  , ui_( new Ui_ShowPlayerResultDialog() )
{
  ui_->setupUi( dlg_ );
  updateStyleSheet( *dlg_ );
}

ShowResultDlg::~ShowResultDlg() = default;

void ShowResultDlg::exec( Tournament const& tournament, Player const& player )
{
  match_model_ = new PlayerMatchModel( tournament, player );
  ui_->tvMatches->setModel( match_model_ );
  dlg_->setWindowTitle( tournament.playerName( player.id() ) );
  ui_->tvMatches->show();
  dlg_->exec();
}

void ShowResultDlg::exec( Tournament const& tournament, Team const& team )
{
  match_model_ = new TeamMatchModel( tournament, team );
  ui_->tvMatches->setModel( match_model_ );
  QString team_name = tr( "unbekannt" );
  int points = 0;
  int opponent_points = 0;
  for ( int i = 0; i < team.size(); ++i ) {
    int p_id = team.playerId( i );
    Player const& player( tournament.player( p_id ) );
    if ( i == 0 ) {
      QSharedPointer<PlayerResult> result( player.result() );
      if ( result ) {
        points = result->resultPoints( true );
        opponent_points = tournament.getOpponentPoints( p_id ) / team.size();
      }
    }
    if ( ! player.team().isEmpty() ) {
      team_name = player.team();
      break;
    }
  }
  if ( points + opponent_points ) {
    team_name += opponent_points > 0
      ? QStringLiteral( " (%1/%2)" ).arg( points ).arg( opponent_points )
      : QStringLiteral( " (%1)" ).arg( points );
  }
  dlg_->setWindowTitle( team_name );
  ui_->tvMatches->show();
  dlg_->exec();
}

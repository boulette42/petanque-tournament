#include "tournament_simulator.h"
#include "match.h"
#include "player.h"
#include "player_result.h"
#include "tournament.h"
#include <qinputdialog.h>
#include <qrandom.h>


TournamentSimulator::TournamentSimulator( Tournament& tournament )
  : tournament_ ( tournament )
{
}

TournamentSimulator::~TournamentSimulator() = default;

PlayerList TournamentSimulator::createRandomPlayers( QWidget* parent ) const
{
  PlayerList ret;
  int cnt = QInputDialog::getInt( parent, tr( "Eingabe" ), tr( "Anzahl generierte Spieler" ), 10, 0, 50 );
  if ( cnt == 0 ) return ret;
  ret.reserve( cnt );
  int max_id = 0;
  foreach ( Player const& player, tournament_.playerList() ) {
    if ( player.id() > max_id ) max_id = player.id();
  }
  max_id += 1;
  for ( int i = 0; i < cnt; ++i ) {
    QString name = tr( "Spieler %1" ).arg( max_id + i, 4, 10, QLatin1Char( '0' ) );
    QString team = tr( "Team %1" ).arg( (max_id + i) / 2, 3, 10, QLatin1Char( '0' ) );;
    Player player( max_id + i, name, QString(), team );
    ret.append( player );
  }
  return ret;
}

Round TournamentSimulator::createRandomResults( Round const& round )
{
  Round ret( round );
  for ( Round::iterator it = ret.begin(); it != ret.end(); ++it ) {
    if ( it->result_.pointsLeft() == 0 && it->result_.pointsRight() == 0 ) {
      uint random = QRandomGenerator::global()->generate();
      it->result_.setPointsLeft( random % 2 ? random % 13 : 13 );
      it->result_.setPointsRight( random % 2 ? 13 : random % 13 );
    }
  }
  return ret;
}


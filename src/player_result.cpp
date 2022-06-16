#include "player_result.h"
#include "match.h"


// --- Player ----------------------------------------------------------------

PlayerResult::PlayerResult( int id )
  : id_( id )
{
  match_list_.reserve( 3 );
}

void PlayerResult::setMatch( int round_idx, Match const& match )
{
  if ( match_list_.size() <= round_idx ) match_list_.resize( round_idx + 1 );
  match_list_[round_idx] = match;
}

int PlayerResult::resultPoints() const
{
  int res = 0;
  for ( int ri = 0; ri < match_list_.size(); ++ri ) {
    res += resultPoints( ri );
  }
  return res;
}

int PlayerResult::wonRounds() const
{
  int res = 0;
  for ( int ri = 0; ri < match_list_.size(); ++ri ) {
    if ( resultPoints( ri ) > 100 ) ++res;
  }
  return res;
}

int PlayerResult::resultPoints( int round_idx ) const
{
  Match const& m( match( round_idx ) );
  if ( m.team_lt_.containsPlayer( id_ ) ) {
    return m.result_.resultPointsLeft();
  }
  if ( m.team_rt_.containsPlayer( id_ ) ) {
    return m.result_.resultPointsRight();
  }
  return 0;
}

Match const& PlayerResult::match( int round_idx ) const
{
  static Match empty;
  return 0 <= round_idx && round_idx < match_list_.size() ? match_list_[round_idx] : empty;
}


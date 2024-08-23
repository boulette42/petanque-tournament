#include "player_result.h"
#include "match.h"
#include "player.h"
#include "tournament.h"


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

int PlayerResult::wonRounds() const
{
  return wonRoundsUntil( match_list_.size() );
}

int PlayerResult::resultPoints( bool formule_x ) const
{
  int res = 0;
  for ( int ri = 0; ri < match_list_.size(); ++ri ) {
    res += resultPoints( ri, formule_x );
  }
  return res;
}

int PlayerResult::buchholzPoints( Tournament const& tournament ) const
{
  int res = 0;
  for ( int ri = 0; ri < match_list_.size(); ++ri ) {
    Match const& m( match( ri ) );
    int p_id = m.team_lt_.containsPlayer( id_ )
      ? m.team_rt_.playerId( 0 )
      : m.team_lt_.playerId( 0 );
    auto pr = tournament.player( p_id ).result();
    res += pr->wonRounds();
  }
  return res;
}

int PlayerResult::buchholzFeinwertung( Tournament const& tournament ) const
{
  int res = 0;
  for ( int ri = 0; ri < match_list_.size(); ++ri ) {
    Match const& m( match( ri ) );
    int p_id = m.team_lt_.containsPlayer( id_ )
      ? m.team_rt_.playerId( 0 )
      : m.team_lt_.playerId( 0 );
    auto pr = tournament.player( p_id ).result();
    res += pr->buchholzPoints( tournament );
  }
  return res;
}

int PlayerResult::wonRoundsUntil( int max_round ) const
{
  int res = 0;
  for ( int ri = 0; ri < max_round; ++ri ) {
    if ( resultPoints( ri, false ) > 0 ) ++res;
  }
  return res;
}

int PlayerResult::resultPoints( int round_idx, bool formule_x ) const
{
  Match const& m( match( round_idx ) );
  int res_mine = 0;
  int res_other = 0;
  if ( m.team_lt_.containsPlayer( id_ ) ) {
    res_mine = m.result_.resultPointsLeft();
    res_other = m.result_.resultPointsRight();
  }
  if ( m.team_rt_.containsPlayer( id_ ) ) {
    res_mine = m.result_.resultPointsRight();
    res_other = m.result_.resultPointsLeft();
  }
  if ( formule_x ) {
    if ( res_mine > res_other ) return 100 + res_mine + res_mine - res_other;
    return res_mine;
  }
  return res_mine - res_other;
}

Match const& PlayerResult::match( int round_idx ) const
{
  static Match empty;
  return 0 <= round_idx && round_idx < match_list_.size() ? match_list_[round_idx] : empty;
}


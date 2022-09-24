#include "round_calculator.h"
#include "match.h"
#include "player.h"
#include "player_result.h"
#include "site.h"
#include "tournament.h"
#include <qfile.h>
#include <qmap.h>
#include <qrandom.h>
#include <qset.h>

extern Round readRoundFile( QString const& filepath, QString& error_string );


namespace {

using FlagList = QVector<bool>;

// Strafpunkte
const int DIFF_LEVEL_1S = 3;      // Unterschied gewonnene Runden == 1 selbes Team
const int DIFF_LEVEL_2S = 8;      // Unterschied gewonnene Runden == 2 selbes Team
const int DIFF_LEVEL_FACTOR = 1;  // Unterschied gewonnene Runden zwischen Teams * Runden
const int DIFF_LEVEL_N = 10;      // Unterschied gewonnene Runden > 2  (kommt nicht vor)
const int DOUBLE_TRIPLET = 15;    // Spieler-Wiederholung in Triplette
const int DOUBLE_DOUBLET = 25;    // Spieler-Wiederholung in Doublette
const int OPPONENT_TRIPLET = 3;   // Gegner-Wiederholung in Triplette
const int OPPONENT_DOUBLET = 6;   // Gegner-Wiederholung in Doublette

const int MAX_TRY_PER_ITERATION = 10000;
const int MAX_TRY_ABSOLUTE = 1000000;
const int MAX_POINTS = 10;        // Ergebnisse oberhalb verwerfen

struct Randomizer
{
  int random_ = 0;
  Randomizer() : random_( QRandomGenerator::global()->generate() ) { }

protected:
  ~Randomizer() { }
};

}


struct PlayerCalc : public Randomizer
{
  int id_ = INVALID_ID;
  int won_rounds_ = 0;
  QSet<int> partners2_;    // Doublette
  QSet<int> partners3_;    // Triplette
  QSet<int> opponents2_;   // Doublette
  QSet<int> opponents3_;   // Triplette

  PlayerCalc() = default;

  void addPartners( Team const& team )
  {
    if ( team.containsPlayer( id_ ) ) {
      for ( int i = 0; i < team.size(); ++i ) {
        int x = team.playerId( i );
        if ( x != id_ ) {
          team.size() == 2 ? partners2_.insert( x ) : partners3_.insert( x );
        }
      }
    } else {
      for ( int i = 0; i < team.size(); ++i ) {
        int x = team.playerId( i );
        team.size() == 2 ? opponents2_.insert( x ) : opponents3_.insert( x );
      }
    }
  }
};


// --- TeamRoundCalculator ---------------------------------------------------

class TeamRoundCalculator
{
  struct TeamCalc : public Randomizer
  {
    int team_idx_ = INVALID_IDX;
    int won_rounds_ = 0;
    int points_ = 0;
    TeamCalc() = default;
  };

  PlayerList const player_list_;
  TeamList const team_list_;
  QVector<TeamCalc> sorted_teams_;
  FlagList team_used_;

public:
  TeamRoundCalculator( PlayerList const& player_list, TeamList const& team_list )
    : player_list_( player_list )
    , team_list_( team_list )
  {
  }

  Round exec()
  {
    sortTeams();
    int const n_teams = sorted_teams_.size();
    team_used_ = FlagList( n_teams, false );
    IdList status( n_teams, -1 );
    int t = 0;                          // aktuelle Team-Position
    do {
      if ( t % 2 ) {
        // rechtes Team
        int idx_rt = nextValidTeam( status[t-1], status[t] );
        if ( idx_rt == INVALID_IDX ) {
          if ( t < 2 ) {
            return Round();
          }
          --t;
          team_used_[status[t]] = false;
          --t;
          team_used_[status[t]] = false;
        } else {
          team_used_[idx_rt] = true;
          status[t] = idx_rt;
          ++t;
        }
      } else {
        // linkes team
        int idx_lt = nextUnusedIdx( 0 );
        if ( idx_lt == INVALID_ID ) {
          // es gibt keine gültige Runde mehr
          return Round();
        }
        team_used_[idx_lt] = true;
        status[t] = idx_lt;
        ++t;
        if ( t == n_teams ) {
          // ungerade Anzahl Teams !?
          break;
        }
        status[t] = idx_lt;  // start für rechtes Team
      }
    } while ( t < n_teams );
    int n_matches = n_teams / 2;
    Round ret( n_matches );
    for ( int m = 0; m < n_matches; ++m ) {
      Match& match( ret[m] );
      match.team_lt_ = currentTeam( status[2 * m] );
      match.team_rt_ = currentTeam( status[2 * m + 1] );
    }
    return ret;
  }

private:
  void sortTeams()
  {
    struct GreaterThanTeam
    {
      inline bool operator() ( TeamCalc const& lhs, TeamCalc const& rhs ) const
      {
        if ( lhs.won_rounds_ != rhs.won_rounds_ ) {
          return lhs.won_rounds_ < rhs.won_rounds_;
        } else {
          return lhs.random_ < rhs.random_;
        }
      }
    };

    QVector<TeamCalc> ret;
    int const n_teams = team_list_.size();
    ret.reserve( n_teams );
    for ( int i = 0; i < n_teams; ++i ) {
      int const id = team_list_[i].playerId( 0 );
      PlayerList::const_iterator it = getPlayerIterator( id );
      if ( it != player_list_.end() ) {
        TeamCalc tc;
        tc.team_idx_ = i;
        tc.points_ = it->result() ? it->result()->resultPoints() : 0;
        tc.won_rounds_ = it->result() ? it->result()->wonRounds() : 0;
        ret.append( tc );
      }
    }
    std::sort( ret.begin(), ret.end(), GreaterThanTeam() );
    sorted_teams_ = ret;
  }

  int nextValidTeam( int idx_lt, int idx_rt )
  {
    Team const& team_lt = currentTeam( idx_lt );
    for ( ;; ) {
      idx_rt = nextUnusedIdx( idx_rt + 1 );
      if ( idx_rt == INVALID_IDX ) break;
      Team const& team_rt = currentTeam( idx_rt );
      if ( ! alreadyCoupled( team_lt, team_rt ) ) break;
    }
    return idx_rt;
  }

  int nextUnusedIdx( int start_idx ) const
  {
    for ( int i = start_idx, n = team_used_.size(); i < n; ++i ) {
      if ( ! team_used_[i] ) return i;
    }
    return INVALID_IDX;
  }

  bool alreadyCoupled( Team const& team_lt, Team const& team_rt ) const
  {
    // Spieler, die das Team wchseln, werden hier nicht berücksichtigt
    int id_lt = team_lt.playerId( 0 );
    PlayerList::const_iterator it_lt = getPlayerIterator( id_lt );
    if ( it_lt == player_list_.constEnd() ) return false;
    QString const team_name = it_lt->team();
    int id_rt = team_rt.playerId( 0 );
    PlayerList::const_iterator it_rt = getPlayerIterator( id_rt );
    if ( it_rt == player_list_.constEnd() ) return false;
    QSharedPointer<PlayerResult> result = it_rt->result();
    if ( ! result ) return false;
    for ( int i = 0, n = result->rounds(); i < n; ++i ) {
      Match const& match( result->match( i ) );
      if ( alreadyCoupled( match.team_lt_, team_name ) ) return true;
      if ( alreadyCoupled( match.team_rt_, team_name ) ) return true;
    }
    return false;
  }

  bool alreadyCoupled( Team const& team_lt, QString const& team_name_rt ) const
  {
    int id_lt = team_lt.playerId( 0 );
    PlayerList::const_iterator it_lt = getPlayerIterator( id_lt );
    if ( it_lt == player_list_.constEnd() ) return false;
    return team_name_rt.compare( it_lt->team(), Qt::CaseInsensitive ) == 0;
  }

  Team const& currentTeam( int idx ) const
  {
    int const team_idx = sorted_teams_[idx].team_idx_;
    return team_list_[team_idx];
  }

  PlayerList::const_iterator getPlayerIterator( int id ) const
  {
    for ( PlayerList::const_iterator it = player_list_.constBegin();
          it != player_list_.constEnd();
          ++it )
    {
      if ( it->id() == id ) return it;
    }
    return player_list_.constEnd();
  }
};


// --- SuperMeleeRoundCalculator ---------------------------------------------

class SuperMeleeRoundCalculator
{
  using IdMap = QMap<int, int>;

  QVector<PlayerCalc> player_list_;   // aktuelle Spieler (sortiert)
  IdMap id_idx_map_;                  // Map Player-ID -> index
  FlagList player_used_;              // Spieler bereits verteilt?
  IdxList status_;                    // gewählter spieler-idx an position

public:
  Round exec( PlayerList const& players )
  {
    Round ret;
    player_list_ = initPlayers( players );
    id_idx_map_ = initIdMap();
    int const n_players = player_list_.size();
    player_used_ = FlagList( n_players, false );
    status_ = IdxList( n_players, INVALID_IDX );
    int last_eval = -1;
    Round rx = initMatches();
    IdxList player_to_team( initPlayerToTeam( rx ) );
    IdxList team_to_player( initTeamToPlayer( player_to_team ) );
    int p = 0;                          // position im status
    int first_doublette = 0;
    if ( n_players % 2 ) first_doublette += 1;
    if ( ( n_players % 4 ) / 2 ) first_doublette += 2;
    int loop_cnt = 1;
    for ( ;; ) {
      int t = player_to_team[p];
      Team& team = currentTeam( rx, t );
      int first_team_idx = team_to_player[t];
      int t_offs = p - first_team_idx;
      int idx = setTeamPlayer( p, team, t_offs );
      if ( idx == INVALID_IDX ) {
        if ( t == 0 ) {
          if ( t_offs == 0 ) break;
          if ( team.size() > 2 ) {
            setMinIdx( p, INVALID_IDX );
          }
        }
        --p;
        idx = status_[p];
        player_used_[idx] = false;
      } else {
        status_[p] = idx;
        if ( p + 1 == n_players ) {
          int s = evaluateRound( rx );
          if ( s == 0 ) return rx;      // optimal: nehmen
          if ( last_eval == -1 || s < last_eval ) {
            last_eval = s;
            ret = rx;
            ret.detach();
          }
          player_used_[idx] = false;
        } else {
          ++p;
          if ( t_offs == 0 && team.size() == 2 ) {
            setMinIdx( p, idx );
          } else if ( team.size() > 2 && t_offs < 2 ) {
            status_[p] = idx;
          }
        }
      }
      ++loop_cnt;
      if ( loop_cnt % MAX_TRY_PER_ITERATION == 0 ) {
        if ( last_eval <= MAX_POINTS ) break;
      }
      if ( loop_cnt >= MAX_TRY_ABSOLUTE ) break;
    }
    return ret;
  }

private:
  QVector<PlayerCalc> initPlayers( PlayerList const& players ) const
  {
    struct LessThanPlayer
    {
      inline bool operator() ( PlayerCalc const& lhs, PlayerCalc const& rhs ) const
      {
        if ( lhs.won_rounds_ != rhs.won_rounds_ ) {
          return lhs.won_rounds_ < rhs.won_rounds_;
        } else {
          return lhs.random_ < rhs.random_;
        }
      }
    };

    QVector<PlayerCalc> ret;
    ret.reserve( players.size() );
    foreach ( Player const& p, players ) {
      PlayerCalc ps;
      ps.id_ = p.id();
      QSharedPointer<PlayerResult> result = p.result();
      if ( result ) {
        ps.won_rounds_ = result->wonRounds();
        for ( int r = 0; r < result->rounds(); ++r ) {
          Match const& m = result->match( r );
          ps.addPartners( m.team_lt_ );
          ps.addPartners( m.team_rt_ );
        }
      }
      ret.append( ps );
    }
    std::sort( ret.begin(), ret.end(), LessThanPlayer() );
    return ret;
  }
  
  IdMap initIdMap()
  {
    IdMap ret;
    for ( int i = 0; i < player_list_.size(); ++i ) {
      PlayerCalc const& player = player_list_[i];
      ret[player.id_] = i;
    }
    return ret;
  }

  Round initMatches()
  {
    Round ret;
    int const n_players = playerCount();
    int const n_matches = n_players / 4;
    int const n_triplets = ( n_players % 4 ) / 2;
    int const n_semitriplets = n_players % 2;
    ret.reserve( n_matches );
    int m = 0;
    if ( n_triplets ) {
      ret.append( { Team::triplet(), Team::triplet(), Result() } );
      ++m;
    }
    if ( n_semitriplets ) {
      ret.append( { Team::triplet(), Team::doublet(), Result() } );
      ++m;
    }
    while ( m < n_matches ) {
      ret.append( { Team::doublet(), Team::doublet(), Result() } );
      ++m;
    }
    return ret;
  }

  IdxList initPlayerToTeam( Round const& rx )
  {
    IdxList ret;
    ret.reserve( rx.size() * 6 );
    int t = 0;
    foreach ( Match const& match, rx ) {
      for ( int i = 0; i < match.team_lt_.size(); ++i ) {
        ret.append( t );
      }
      ++t;
      for ( int i = 0; i < match.team_rt_.size(); ++i ) {
        ret.append( t );
      }
      ++t;
    }
    return ret;
  }

  IdxList initTeamToPlayer( IdxList const& player_to_team )
  {
    IdxList ret;
    int const n_players = player_to_team.size();
    ret.reserve( n_players / 4 );
    int last_idx = INVALID_IDX;
    for ( int i = 0; i < n_players; ++i ) {
      int const t_idx = player_to_team[i];
      if ( t_idx != last_idx ) {
        ret.append( i );
        last_idx = t_idx;
      }
    }
    return ret;
  }

  Team& currentTeam( Round& rx, int t )
  {
    Match& match( rx[t / 2] );
    return t % 2 ? match.team_rt_ : match.team_lt_;
  }

  int setTeamPlayer( int p, Team& team, int t_offs )
  {
    int idx = status_[p];
    idx = nextFreePlayerIdx( idx );
    if ( idx != INVALID_IDX ) {
      player_used_[idx] = true;
      PlayerCalc const& player = player_list_[idx];
      team.setPlayerId( t_offs, player.id_ );
    }
    return idx;
  }

  void setMinIdx( int p, int idx )
  {
    // minimum setzen
    for ( int i = p, n = status_.size(); i < n; ++i ) {
      status_[i] = idx;
    }
  }

  int nextFreePlayerIdx( int min_idx ) const
  {
    int const first_idx = min_idx == INVALID_IDX ? 0 : min_idx + 1;
    for ( int idx = first_idx; idx < playerCount(); ++idx ) {
      if ( ! player_used_[idx] ) return idx;
    }
    return INVALID_IDX;
  }

  int evaluateRound( Round const& round ) const
  {
    int ret = 0;
    foreach ( Match const& m, round ) {
      ret += evaluateTeamRounds( m.team_lt_ );
      ret += evaluateTeamRounds( m.team_rt_ );
      ret += evaluateTeamPartner( m.team_lt_ );
      ret += evaluateTeamPartner( m.team_rt_ );
      ret += evaluateTeamOpponent( m.team_lt_, m.team_rt_ );
      ret += evaluateTeamDiff( m.team_lt_, m.team_rt_ );
    }
    return ret;
  }

  // Anzahl der gewonnene Runden
  int evaluateTeamRounds( Team const& t ) const
  {
    int ret = 0;
    if ( t.size() == 0 ) return ret;
    PlayerCalc const& pc_0 = player( t.playerId( 0 ) );
    int min_w = pc_0.won_rounds_;
    int max_w = pc_0.won_rounds_;
    for ( int i = 1; i < t.size(); ++i ) {
      PlayerCalc const& pc_n = player( t.playerId( i ) );
      min_w = qMin( min_w, pc_n.won_rounds_ );
      max_w = qMin( max_w, pc_n.won_rounds_ );
    }
    switch ( max_w - min_w ) {
    case 0: break;
    case 1: ret += DIFF_LEVEL_1S; break;
    case 2: ret += DIFF_LEVEL_2S; break;
    default: ret += DIFF_LEVEL_N; break;
    }
    return ret;
  }

  // Spieler in derselben Mannschaft
  int evaluateTeamPartner( Team const& t ) const
  {
    int ret = 0;
    for ( int i = 0; i < t.size() - 1; ++i ) {
      PlayerCalc const& pc = player( t.playerId( i ) );
      for ( int j = i + 1; j < t.size(); ++j ) {
        if ( pc.partners2_.contains( t.playerId( j ) ) ) {
          ret += t.size() == 2 ? DOUBLE_DOUBLET : DOUBLE_TRIPLET;
        }
        if ( pc.partners3_.contains( t.playerId( j ) ) ) {
          ret += DOUBLE_TRIPLET;
        }
      }
    }
    return ret;
  }

  // Spieler in der gegnerischen Mannschaft
  int evaluateTeamOpponent( Team const& t_lhs, Team const& t_rhs ) const
  {
    int ret = 0;
    for ( int i = 0; i < t_lhs.size(); ++i ) {
      PlayerCalc const& pc = player( t_lhs.playerId( i ) );
      for ( int j = 0; j < t_rhs.size(); ++j ) {
        if ( pc.opponents2_.contains( t_rhs.playerId( j ) ) ) {
          ret += t_rhs.size() == 2 ? OPPONENT_DOUBLET : OPPONENT_TRIPLET;
        }
        if ( pc.opponents3_.contains( t_rhs.playerId( j ) ) ) {
          ret += OPPONENT_TRIPLET;
        }
      }
    }
    return ret;
  }

  int evaluateTeamDiff( Team const& lhs, Team const& rhs ) const
  {
    int s_lhs = getTeamWon( lhs );
    int s_rhs = getTeamWon( rhs );
    int ret = s_lhs - s_rhs;
    if ( ret < 0 ) ret *= -1;
    return ret * DIFF_LEVEL_FACTOR;
  }

  // auf 2 Spieler bezogen
  int getTeamWon( Team const& t ) const
  {
    int ret = 0;
    if ( t.size() > 0 ) {
      for ( int i = 0; i < t.size(); ++i ) {
        PlayerCalc const& pc = player( t.playerId( i ) );
        ret += pc.won_rounds_;
      }
      if ( t.size() == 3 ) {
        // auf 2 Spieler umrechnen
        ret *= 2;
        ret += 1;
        ret /= 3;
      }
    }
    return ret;
  }

  PlayerCalc const& player( int id ) const
  {
    int const idx = id_idx_map_[id];
    return player_list_[idx];
  }

  int playerCount() const
  {
    return player_list_.size();
  }

};


// --- SiteCalculator --------------------------------------------------------

class SiteCalculator
{
  struct SiteCalc : public Randomizer
  {
    int site_id_ = INVALID_ID;
    int malus_ = 0;

    SiteCalc() = default;
  };

  Round const round_;
  QVector<SiteCalc> sorted_sites_;
  using PlayerSiteMap = QMap<int, IdxList>;
  PlayerSiteMap player_used_sites_;
  FlagList site_used_;
  IdList site_idx_;
  IdList min_site_idx_;

public:
  SiteCalculator( Round const& round )
    : round_( round )
  {
  }

  Round exec( Tournament const& tournament )
  {
    initSiteList( tournament.selectedSiteList() );
    initUsedSites( tournament );
    int const n_matches = round_.size();
    int const n_sites = sorted_sites_.size();
    int min_result = -1;
    int s_idx = 0;
    do {
      int idx = nextUnusedIdx( site_idx_[s_idx] );
      if ( idx != INVALID_IDX ) {
        site_idx_[s_idx] = idx;
        if ( s_idx + 1 < n_sites ) {
          site_used_[idx] = true;
          ++s_idx;
          continue;
        }
        int result = calcSiteMalus();
        if ( result == 0 ) {
          return sortRound( site_idx_ );
        }
        if ( min_result < 0 || result < min_result ) {
          min_result = result;
          min_site_idx_ = site_idx_;
        }
      }
      --s_idx;
      if ( s_idx < 0 ) {
        // keiner mit '0': bisher besten nehmen
        return sortRound( min_site_idx_ );
      }
      idx = site_idx_[s_idx];
      site_used_[idx] = false;
    } while ( 1 );
  }

private:
  void initSiteList( QVector<Site> const& site_list )
  {
    struct LessThan {
      bool operator()( SiteCalc const& lhs, SiteCalc const& rhs ) {
        return lhs.random_ < rhs.random_;
      }
    };

    int const n_matches = round_.size();
    int const n_defined_sites = site_list.size();
    int const n_sites = qMax( n_matches, n_defined_sites );
    site_used_ = FlagList( n_sites, false );
    site_idx_ = IdList( n_sites, INVALID_IDX );
    int last_id = 0;
    for ( int i = 0; i < n_defined_sites; ++i ) {
      last_id = qMax( site_list[i].id(), last_id );
    }
    sorted_sites_.clear();
    int i = 0;
    for ( ; i < n_defined_sites; ++i ) {
      SiteCalc sc;
      int id = site_list[i].id();
      if ( id <= 0 ) {
        ++last_id;
        id = last_id;
      }
      sc.site_id_ = id;
      sorted_sites_.append( sc );
    }
    for ( ; i < n_matches; ++i ) {
      SiteCalc sc;
      ++last_id;
      sc.site_id_ = last_id;
      sorted_sites_.append( sc );
    }
    std::sort( sorted_sites_.begin(), sorted_sites_.end(), LessThan() );
  }

  void initUsedSites( Tournament const& tournament )
  {
    player_used_sites_.clear();    int n_matches = round_.size();
    for ( int i = 0; i < n_matches; ++i ) {
      Match const& match( round_[i] );
      addUsedSites( match.team_lt_, tournament );
      addUsedSites( match.team_rt_, tournament );
    }
  }

  void addUsedSites( Team const& team, Tournament const& tournament )
  {
    for ( int t = 0; t < team.size(); ++t ) {
      int p_id = team.playerId( t );
      PlayerSiteMap::iterator p_it = player_used_sites_.find( p_id );
      if ( p_it == player_used_sites_.end() ) {
        p_it = player_used_sites_.insert( p_id, IdxList() );
      }
      Player const& player = tournament.player( p_id );
      if ( player.result() ) {
        for ( int r = 0; r < player.result()->rounds(); ++r ) {
          p_it->append( player.result()->match( r ).site_id_ );
        }
      }
    }
  }

  int calcSiteMalus()
  {
    int res = 0;
    int n_matches = round_.size();
    for ( int i = 0; i < n_matches; ++i ) {
      int idx = site_idx_[i];
      int site_id = sorted_sites_[idx].site_id_;
      Match const& match( round_[i] );
      res += calcSiteMalus( site_id, match.team_lt_ );
      res += calcSiteMalus( site_id, match.team_rt_ );
    }
    return res;
  }

  int calcSiteMalus( int site_id, Team const& team )
  {
    int res = 0;
    for ( int t = 0; t < team.size(); ++t ) {
      int p_id = team.playerId( t );
      PlayerSiteMap::iterator p_it = player_used_sites_.find( p_id );
      if ( p_it != player_used_sites_.end() ) {
        res += p_it->contains( site_id );
      }
    }
    return res;
  }

  Round sortRound( IdList const& site_idx )
  {
    struct LessThan {
      bool operator()( Match const& lhs, Match const& rhs ) {
        return lhs.site_id_ < rhs.site_id_;
      }
    };

    Round ret = round_;
    for ( int i = 0; i < ret.size(); ++i ) {
      int idx = site_idx[i];
      ret[i].site_id_ = sorted_sites_[idx].site_id_;
    }
    std::sort( ret.begin(), ret.end(), LessThan() );
    return ret;
  }

  int nextUnusedIdx( int start_idx )
  {
    start_idx = start_idx == INVALID_IDX ? 0 : start_idx + 1;
    for ( int i = start_idx; i < site_used_.size(); ++i ) {
      if ( ! site_used_[i] ) return i;
    }
    return INVALID_IDX;
  }
};


// --- interface -------------------------------------------------------------

Round calcRound( PlayerList const& player_list )
{
  return SuperMeleeRoundCalculator().exec( player_list );
}

Round calcRound( PlayerList const& player_list, TeamList const& team_list )
{
  return TeamRoundCalculator( player_list, team_list ).exec();
}

Round calcSites( Round const& round, Tournament const& tournament )
{
  return SiteCalculator( round ).exec( tournament );
}

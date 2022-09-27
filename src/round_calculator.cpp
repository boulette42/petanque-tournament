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


namespace {

using FlagList = QVector<bool>;

// Supermelee Strafpunkte
const int DIFF_LEVEL_1S = 30;     // Unterschied gewonnene Runden == 1 selbes Team
const int DIFF_LEVEL_2S = 80;     // Unterschied gewonnene Runden == 2 selbes Team
const int DIFF_LEVEL_FACTOR = 1;  // Unterschied gewonnene Runden zwischen Teams * Runden
const int DIFF_LEVEL_N = 100;     // Unterschied gewonnene Runden > 2  (kommt nicht vor)
const int DOUBLE_TRIPLET = 150;   // Spieler-Wiederholung in Triplette
const int DOUBLE_DOUBLET = 250;   // Spieler-Wiederholung in Doublette
const int OPPONENT_TRIPLET = 30;  // Gegner-Wiederholung in Triplette
const int OPPONENT_DOUBLET = 60;  // Gegner-Wiederholung in Doublette

// Team-Strafpunkte
const int T_SAME_TEAM = 10000;      // erneut gegen selbes team spielen
const int T_OTHER_POINTS = 100;     // gegen Team mit anderer Punktzahl
const int T_SAME_ASSOC = 1;         // Gegner im selben Verein

const int MAX_TRY_PER_ITERATION = 10000;
const int MAX_TRY_ABSOLUTE = 1000000;
const int MAX_POINTS = 100;       // Ergebnisse oberhalb verwerfen


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
  using CrcList = QVector<quint16>;

  struct TeamCalc : public Randomizer
  {
    int team_idx_ = INVALID_IDX;
    int won_rounds_ = 0;
    IdList opponents_;
    CrcList assocs_;

    TeamCalc() = default;

    void appendAssoc( QString const& association )
    {
      QByteArray ba( association.toUtf8() );
      assocs_.append( qChecksum( ba, ba.size() ) );
    }
  };

  PlayerList const player_list_;
  TeamList const team_list_;
  int round_idx_ = -1;
  QVector<TeamCalc> sorted_teams_;
  FlagList team_used_;

public:
  TeamRoundCalculator( PlayerList const& player_list, TeamList const& team_list, int round_idx )
    : player_list_( player_list )
    , team_list_( team_list )
    , round_idx_( round_idx )
  {
  }

  Round exec()
  {
    initSortedTeams();
    int const n_teams = sorted_teams_.size();
    int abort_values[] = { T_OTHER_POINTS * ( round_idx_ + 1 ), T_SAME_TEAM, INT_MAX };
    for (int i_try = 0; i_try < 3; ++i_try) {
      int points = 0;
      int min_points = abort_values[i_try];
      team_used_ = FlagList( n_teams, false );
      IdxList status( n_teams, -1 );
      IdxList best_status;
      QVector<int> point_status( n_teams, 0 );
      int t = 0;                          // aktuelle Team-Position
      do {
        if ( t % 2 ) {
          // rechtes Team
          int malus = 0;
          int idx_rt = calcNextValidTeam( status[t-1], status[t], malus );
          if ( idx_rt == INVALID_IDX ) {
            do {
              if ( t < 2 ) {
                if ( !best_status.isEmpty() ) return toRound( best_status );
                break;
              }
              --t;
              team_used_[status[t]] = false;
              --t;
              team_used_[status[t]] = false;
              points = point_status[t];
            } while ( points >= min_points );
            if ( t < 2 ) break;   // neuer versuch mit geringeren anforderungen
          } else {
            status[t] = idx_rt;
            if ( points + malus < min_points ) {
              team_used_[idx_rt] = true;
              points += malus;
              point_status[t] = points;
              ++t;
              if ( t == n_teams ) {
                if ( points == 0 ) return toRound( status );
                if ( min_points > points ) {
                  min_points = points;
                  best_status = status;
                }
                --t;
                team_used_[status[t]] = false;
              }
            }
          }
        } else {
          // linkes team
          int idx_lt = nextUnusedIdx( 0 );
          if ( idx_lt == INVALID_ID ) break;  // <?> es gibt keine gültige Runde mehr
          team_used_[idx_lt] = true;
          status[t] = idx_lt;
          ++t;
          status[t] = idx_lt;  // start für rechtes Team
        }
      } while ( t < n_teams );
      if ( !best_status.isEmpty() ) return toRound( best_status );
    }
    return Round();
  }

  Round toRound( IdxList const& status )
  {
    int const n_matches = status.size() / 2;
    Round ret( n_matches );
    for ( int m = 0; m < n_matches; ++m ) {
      Match& match( ret[m] );
      match.team_lt_ = currentTeam( status[2 * m] );
      match.team_rt_ = currentTeam( status[2 * m + 1] );
    }
    return ret;
  }

private:
  void initSortedTeams()
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

    QVector<TeamCalc> tc_list;
    QMap<int,int> player_to_team = getPlayerToTeam( team_list_ );  // player-id -> team-idx
    int const n_teams = team_list_.size();
    tc_list.reserve( n_teams );
    for ( int i = 0; i < n_teams; ++i ) {
      bool first = true;
      Team const& team( team_list_[i] );
      TeamCalc tc;
      tc.team_idx_ = i;
      for ( int p = 0; p < team.size(); ++ p ) {
        int const id = team.playerId( p );
        PlayerList::const_iterator it = getPlayerIterator( id );
        if ( it != player_list_.end() ) {
          if ( first ) {
            first = false;
            QSharedPointer<PlayerResult> result( it->result() );
            if ( result ) {
              tc.won_rounds_ = result->wonRounds();
              for ( int ri = 0; ri < result->rounds(); ++ri ) {
                Match const& m( result->match( ri ) );
                Team const& t_opp( m.team_lt_.containsPlayer( id ) ? m.team_rt_ : m.team_lt_ );
                tc.opponents_.append( player_to_team[t_opp.playerId( 0 )] );
              }
            }
          }
          tc.appendAssoc(it->association());
        }
      }
      tc_list.append( tc );
    }
    std::sort( tc_list.begin(), tc_list.end(), GreaterThanTeam() );
    sorted_teams_ = tc_list;
  }

  static QMap<int,int> getPlayerToTeam( TeamList const& team_list )
  {
    QMap<int,int> ret;
    for ( int ti = 0, tn = team_list.size(); ti < tn; ++ti ) {
      Team const& team( team_list[ti] );
      for ( int pi = 0; pi < team.size(); ++pi ) {
        ret[team.playerId( pi )] = ti;
      }
    }
    return ret;
  }

  int calcNextValidTeam( int idx_lt, int idx_rt, int& malus )
  {
    malus = 0;
    TeamCalc const& tc_lt( sorted_teams_[idx_lt] );
    idx_rt = nextUnusedIdx( idx_rt + 1 );
    if ( idx_rt == INVALID_IDX ) return INVALID_IDX;
    TeamCalc const& tc_rt( sorted_teams_[idx_rt] );
    if ( tc_lt.opponents_.contains( tc_rt.team_idx_ ) ) malus += T_SAME_TEAM;
    int won_diff = tc_lt.won_rounds_ - tc_rt.won_rounds_;
    if ( won_diff >= 0 ) malus += won_diff * T_OTHER_POINTS;
    else                 malus -= won_diff * T_OTHER_POINTS;
    malus += sameAssoc( tc_lt.assocs_, tc_rt.assocs_) * T_SAME_ASSOC;
    return idx_rt;
  }

  int nextUnusedIdx( int start_idx ) const
  {
    for ( int i = start_idx, n = team_used_.size(); i < n; ++i ) {
      if ( ! team_used_[i] ) return i;
    }
    return INVALID_IDX;
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

  int sameAssoc( CrcList const& a_lt, CrcList const& a_rt )
  {
    int ret = 0;
    QVector<int> used( a_rt.size(), -1 );
    for ( int i = 0; i < a_lt.size(); ++i ) {
      int p = -1;
      for (;;) {
        p = a_rt.indexOf( a_lt[i], p+1 );
        if ( p < 0 ) break;
        if ( !used.contains( p ) ) {
          ++ret;
          used.append( p );
          break;
        }
      }
    }
    return ret;
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
  using MatchSites = QVector<IdList>;
  MatchSites pre_used_sites_;
  FlagList site_used_;
  IdxList site_idx_;

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
    int pos_max = n_matches;	// keine Überschneidung
    int pos = 0;        // match-position
    do {
      int idx = nextUnusedIdx( site_idx_[pos] );
      if ( idx != INVALID_IDX ) {
        site_idx_[pos] = idx;
        if ( !pre_used_sites_[pos].contains( sorted_sites_[idx].site_id_ ) ) {
          ++pos;
          if ( pos < pos_max ) {
            site_used_[idx] = true;
            continue;
          }
          // Ziel erreicht
          if ( pos < n_matches ) fillRemaining( pos );
          return sortRound();
        }
        continue;
      }
      site_idx_[pos] = INVALID_IDX;
      if ( pos == 0 ) {
        // kein optimaler gefunden: Anspruch senken
        --pos_max;
        if ( pos_max < 3 ) {
          // weiteres suchen nicht mehr sinnvoll: Zufalls-Lösung nehmen
          for ( idx = 0; idx < site_idx_.size(); ++idx ) {
            site_idx_[idx] = idx;
          }
          return sortRound();
        }
      }
      else {
        --pos;
        idx = site_idx_[pos];
        site_used_[idx] = false;
      }
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
    site_idx_ = IdList( n_matches, INVALID_IDX );
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
    pre_used_sites_ = MatchSites( round_.size() );
    int n_matches = round_.size();
    for ( int i = 0; i < n_matches; ++i ) {
      Match const& match( round_[i] );
      pre_used_sites_[i] += usedTeamSites( match.team_lt_, tournament );
      pre_used_sites_[i] += usedTeamSites( match.team_rt_, tournament );
    }
  }

  IdList usedTeamSites( Team const& team, Tournament const& tournament )
  {
    IdList ret;
    for ( int t = 0; t < team.size(); ++t ) {
      int p_id = team.playerId( t );
      Player const& player = tournament.player( p_id );
      if ( player.result() ) {
        for ( int r = 0; r < player.result()->rounds(); ++r ) {
          ret.append( player.result()->match( r ).site_id_ );
        }
      }
    }
    return ret;
  }

  void fillRemaining( int pos )
  {
     while ( pos < site_idx_.size() ) {
       // kein optimales Ergebnis, Rest aufsteigend zuordnen
       int idx = nextUnusedIdx( INVALID_IDX );
       site_idx_[pos] = idx;
       site_used_[idx] = true;
       ++pos;
     }
  }

  Round sortRound()
  {
    struct LessThan {
      bool operator()( Match const& lhs, Match const& rhs ) {
        return lhs.site_id_ < rhs.site_id_;
      }
    };

    Round ret = round_;
    for ( int i = 0; i < ret.size(); ++i ) {
      int idx = site_idx_[i];
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


// --- class RoundCalculator -------------------------------------------------

RoundCalculator::RoundCalculator( QWidget* parent, int round_idx )
  : parent_( parent )
  , round_idx_( round_idx )
{
}

Round RoundCalculator::calcRound( PlayerList const& player_list )
{
  return SuperMeleeRoundCalculator().exec( player_list );
}

Round RoundCalculator::calcRound( PlayerList const& player_list, TeamList const& team_list )
{
  return TeamRoundCalculator( player_list, team_list, round_idx_ ).exec();
}

Round RoundCalculator::calcSites( Round const& round, Tournament const& tournament )
{
  return SiteCalculator( round ).exec( tournament );
}

void RoundCalculator::setError( QString const& error_string )
{
  error_string_ = error_string;
}

QString RoundCalculator::lastError() const
{
  return error_string_;
}

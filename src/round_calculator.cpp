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
#include <qdebug.h>

#define TRACE(x)  // qDebug() << x;

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

// Team-Strafpunkte (bis 999 Teams disjunkt
const int T_SAME_TEAM = 1000000;    // erneut gegen selbes team spielen
const int T_OTHER_POINTS = 1000;    // gegen Team mit anderer Punktzahl
const int T_SAME_ASSOC = 1;         // Gegner im selben Verein
const int MAX_TRY_SAME_ASSOC = 10;  // Abbruch bei ausschließlich selben Verein


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
    int points = 0;
    int min_points = INT_MAX;
    int minimum_malus = calcMinimumMalus();
    int max_try = MAX_TRY_SAME_ASSOC;
    team_used_ = FlagList( n_teams, false );
    IdxList status( n_teams, -1 );
    IdxList best_status;
    QVector<int> base_points( n_teams/2, 0 );
    int t = 0;                          // aktuelle Team-Position
    for (;;) {
      if ( t % 2 ) {
        // rechtes Team
        int malus = 0;
        int idx_rt = calcNextValidTeam( status[t-1], status[t], malus );
        if ( idx_rt == INVALID_IDX ) {
          do {
            if ( t < 2 ) {
              if ( !best_status.isEmpty() ) return toRound( best_status );
              return Round();   // kein Ergebnis
            }
            --t;
            team_used_[status[t]] = false;
            --t;
            team_used_[status[t]] = false;
            points = base_points[t/2];
          } while ( points >= min_points );
        } else {
          status[t] = idx_rt;
          if ( points + malus <= min_points ) {
            points += malus;
            if ( t == n_teams-1 ) {
              if ( points == 0 ) return toRound( status );  // optimal
              if ( min_points > points ) {
                min_points = points;
                best_status = status;
                if ( points < minimum_malus ) {
                  minimum_malus = points;
                  max_try = MAX_TRY_SAME_ASSOC;
                }
              }
              if ( points <= minimum_malus ) {
                // dies kann dazu führen, dass Spieler doch gegen andere Spieler
                // des selben Vereins spielen. Nämlich wenn die gleichen Vereine
                // das einzige Kriterium zum Ausschluss bilden und mehr als
                // MAX_TRY_SAME_ASSOC Lösungen hintereinander gleich gut aber
                // nicht die besten sind...
                --max_try;
                if ( max_try < 1 ) return toRound( status );
              }
            } else {
              team_used_[idx_rt] = true;
              ++t;
              base_points[t/2] = points;
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

  int calcMinimumMalus() const
  {
    QVector<int> wr_cnt;
    for ( TeamCalc const& tc : sorted_teams_ ) {
      int wr = tc.won_rounds_;
      if ( wr >= wr_cnt.size() ) {
        wr_cnt.resize( wr + 1 );
      }
      wr_cnt[wr] += 1;
    }
    // Ungerade Anzahl von Teams mit derselben Punktzahl führt *immer*
    // zu Malus-Punkten
    int is_uneven = wr_cnt[0] % 2;
    int different_wr_malus = is_uneven;
    for ( int i = 1; i < wr_cnt.size(); ++i ) {
      if ( ( wr_cnt[i] % 2 ) != is_uneven ) {
        ++different_wr_malus;
        is_uneven = 1 - is_uneven;
      }
    }
    different_wr_malus *= T_OTHER_POINTS;
    // ausschließlich Malus-Punkte wegen gleicher Vereine sollten irgendwann
    // ignoriert werden können, insbesondere bei Vereins-internen Turnieren
    int same_assoc_malus = player_list_.size() / 2;
    same_assoc_malus *= T_SAME_ASSOC;
    return different_wr_malus + same_assoc_malus;
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
  PlayerList players_;
  QMap<int, int> idx_map_;
  QVector<int> team_id_;  // index pos - Team-Id
  QVector<int> match_id_; // index pos - Match-Id

public:
  SuperMeleeRoundCalculator( PlayerList const& players )
    : players_( players )
  {
    init();
  }

  Round exec( int round_idx )
  {
    PlayerList players = getScrambledPlayerList();
    TeamList team_list;
    Round ret;
    switch ( round_idx ) {
    case 0:
      // round 0: random sorted matches
      // minimze partners/opponents in same association
      team_list = getTeamList0( players );
      break;
    case 1:
      // round 1: combine winner/loser in one team
      // minimize same partners/opponents as in round 0
      team_list = getTeamList1( players );
      break;
    case 2:
      // round 2: combine double winner/double loser in one team
      // and scramble the rest
      // minimize same partners/opponents as in round 0
      team_list = getTeamList2( players );
      break;
    }
    for ( int t = 0, tn = teamCount(); t < tn; t += 2 ) {
      Match m;
      m.team_lt_ = team_list[t];
      m.team_rt_ = team_list[t + 1];
      ret.push_back( m );
    }
    return ret;
  }

  void init()
  {
    // init maps: player-idx -> team-id
    //            player-idx -> match-id
    int const n = playerCount();
    team_id_.clear();
    match_id_.clear();
    int const i_trpl = n - ( n % 4 ) * 3;
    int t_id = 0;
    int m_id = 0;
    for ( int i = 0; i < n; ++i ) {
      team_id_.push_back( t_id );
      match_id_.push_back( m_id );
      if ( i < i_trpl && ( i % 2 ) == 1
        || i >= i_trpl && ( i % 3 ) == 2 )
      {
        ++t_id;
        if ( t_id % 2 == 0 ) {
          ++m_id;
        }
      }
    }
    // init map: player-id -> player-idx
    for ( int i = 0; i < n; ++i ) {
      Player const& player = players_[i];
      idx_map_[player.id()] = i;
    }
  }

  PlayerList getScrambledPlayerList()
  {
    QVector<int> i_rand;
    int const n = players_.size();
    for ( int i = 0; i < n; ++i ) {
      i_rand.push_back( QRandomGenerator::global()->generate() );
    }
    QMap<int, int> const& idx_map( idx_map_ );
    auto isLess = [i_rand, idx_map]( Player const& lhs, Player const& rhs ) {
      int l_idx = idx_map[lhs.id()];
      int r_idx = idx_map[rhs.id()];
      return i_rand[l_idx] < i_rand[r_idx];
    };
    PlayerList sorted = players_;
    std::sort( sorted.begin(), sorted.end(), isLess );
    return sorted;
  }

  TeamList getTeamList0( PlayerList const& players ) const
  {
    // group players by association
    QMap <QString, IdList> a_map;
    for ( Player const& p : players ) {
      QString assoc = p.association();
      int id = p.id();
      a_map[assoc].push_back( id );
    }
    // sort associations by member count
    QList<QString> keys = a_map.keys();
    auto countGreater = [a_map]( QString const& lhs, QString const& rhs ) {
      return a_map[lhs].size() > a_map[rhs].size();
    };
    std::sort( keys.begin(), keys.end(), countGreater );
    // fill teams in order of association size
    //   first player left teams
    //   first player right team
    //   second player left teams
    //   second player right team
    //   all other
    TeamList ret = initTeams();
    int const tc = ret.size();
    int first_t_id = 0;
    int t_id = first_t_id;
    int pos = 0;
    for ( QString const& key : keys ) {
      for ( int id : a_map[key] ) {
        ret[t_id].setPlayerId( pos, id );
        ++t_id;
        if ( pos < 2 ) ++t_id; 
        if ( t_id >= tc ) {
          first_t_id = 1 - first_t_id;    // switch left / right
          if ( first_t_id == 0 ) ++pos;   // left: next position
          t_id = first_t_id;
        }
      }
    }
    return ret;
  }


  enum class Result { Loss = 0, Win = 1, DblWin = 2 };
  // array: player_ids grouped by wincount
  struct WinLossArr
  {
    QMap<int, IdList> id_map_;

  public:
    WinLossArr( PlayerList const& players )
    {
      for ( Player const& p : players ) {
        int won = p.result()->wonRounds();
        id_map_[won].push_back( p.id() );
      }
    }

    // returns next unused winner- or loserid
    int nextUnusedId( Result wl )
    {
      IdList& id_list( id_map_[(int)wl] );
      int id = INVALID_ID;
      for ( int i = 0; i < id_list.size(); ++i ) {
        id = id_list[i];
        if ( id != INVALID_ID ) {
          id_list[i] = INVALID_ID;
          break;
        }
      }
      return id;
    }

    // stores unused winner/loser ids
    void insertIdList( IdList const& diff_list, Result wl )
    {
      IdList& id_list( id_map_[(int)wl] );
      int i = 0;
      for ( int id : diff_list ) {
        while ( id_list[i] != INVALID_ID ) ++i;
        id_list[i] = id;
      }
    }

    Result currentResult() const
    {
      IdList const& id_list_loss( id_map_[0] );
      int loss_cnt = 0;
      for ( int id : id_list_loss ) {
        if ( id != INVALID_ID ) ++loss_cnt;
      }
      int w_idx = id_map_.size() - 1;
      IdList const& id_list_win( id_map_[w_idx] );
      int win_cnt = 0;
      for ( int id : id_list_win ) {
        if ( id != INVALID_ID ) ++win_cnt;
      }
      return win_cnt > loss_cnt ? Result::Win : Result::Loss;
    }

    QString status() const
    {
      QStringList err;
      int ix = 0;
      for ( IdList const& id_list : id_map_ ) {
        int cnt = 0;
        for ( int id : id_list ) {
          if ( id != INVALID_ID ) ++cnt;
        }
        err << QStringLiteral( "id_map[%1]: %2 items" ).arg( ix ).arg( cnt );
        ++ix;
      }
      return err.join( QStringLiteral( "; " ) );
    }
  };

  struct IdChecker
  {
    SuperMeleeRoundCalculator const* smc_;
    IdList const& opponents_;

  public:
    IdChecker( SuperMeleeRoundCalculator const* smc, IdList const& opponents )
      : smc_( smc )
      , opponents_( opponents )
    { }

    int getNextId( WinLossArr& wla, Result res = Result::Win ) const
    {
      return getNextId( wla, IdList(), res );
    }

    int getNextId( WinLossArr& wla, IdList const& exclude_list, Result res = Result::Win ) const
    {
      int id = INVALID_ID;
      IdList diff_list;
      do {
        // check auf partner
        id = smc_->getNextResId( wla, exclude_list, res );
        if ( !opponents_.contains( id ) ) break;
        diff_list.push_back( id );
      } while ( id != INVALID_ID );
      wla.insertIdList( diff_list, res );
      if ( id == INVALID_ID ) id = wla.nextUnusedId( res );
      return id;
    }
  };


  TeamList getTeamList1( PlayerList const& players )
  {
    WinLossArr wla( players );
    TeamList ret = initTeams();
    for ( int t = 0; t < ret.size(); t += 2 ) {
      Team& team_lt = ret[t];
      int win_id = wla.nextUnusedId( Result::Win );
      team_lt.setPlayerId( 0, win_id );
      int loss_id = wla.nextUnusedId( Result::Loss );
      team_lt.setPlayerId( 1, loss_id );
      if ( team_lt.size() == 3 ) {
        Result res = wla.currentResult();
        IdList exclude_list = getPartners( res == Result::Win ? win_id : loss_id );
        int id = getNextValidId( wla, exclude_list, res );
        if ( id == INVALID_ID ) {
          // internal error
        }
        team_lt.setPlayerId( 2, id );
      }

      IdList opponents = getTeamOpponents( team_lt );
      IdChecker ichk( this, opponents );
      Team& team_rt = ret[t + 1];
      win_id = ichk.getNextId( wla, Result::Win );
      team_rt.setPlayerId( 0, win_id );
      loss_id = ichk.getNextId( wla, Result::Loss );
      team_rt.setPlayerId( 1, loss_id );
      if ( team_rt.size() == 3 ) {
        Result res = wla.currentResult();
        IdList diff_list;
        int id = INVALID_ID;
        IdList exclude_list = getPartners( res == Result::Win ? win_id : loss_id );
        do {
          id = getNextResId( wla, exclude_list, res );
          if ( !opponents.contains( id ) ) break;
          diff_list.push_back( id );
        } while ( id != INVALID_ID );
        wla.insertIdList( diff_list, res );
        if ( id == INVALID_ID ) {
          id = getNextValidId( wla, exclude_list, res );
        }
        team_rt.setPlayerId( 2, id );
      }
    }
    return ret;
  }

  TeamList getTeamList2( PlayerList const& players )
  {
    WinLossArr wla( players );
    TeamList ret = initTeams();
    // state I - double winners ans losers
    // start with doublette
    TRACE(wla.status())
    for ( int t = ret.size() - 2; t > 0; t -= 2 ) {
      Team& team_lt = ret[t+1];
      int win_id = wla.nextUnusedId( Result::DblWin );
      int loss_id = wla.nextUnusedId( Result::Loss );
      if ( win_id == INVALID_ID ) {
        if ( loss_id == INVALID_ID ) break;
        win_id = getNextValidId( wla, getPartners( loss_id ), Result::Win );
      }
      else if ( loss_id == INVALID_ID ) {
        loss_id = getNextValidId( wla, getPartners( win_id) , Result::Win );
      }
      team_lt.setPlayerId( 0, win_id );
      team_lt.setPlayerId( 1, loss_id );

      IdList opponents = getTeamOpponents( team_lt );
      IdChecker ichk( this, opponents );
      Team& team_rt = ret[t];
      win_id = ichk.getNextId( wla, Result::DblWin );
      loss_id = ichk.getNextId( wla, Result::Loss );
      if ( win_id == INVALID_ID ) {
        if ( loss_id == INVALID_ID ) {
          win_id = ichk.getNextId( wla );
          IdList exclude_list = getPartners( win_id );
          loss_id = ichk.getNextId( wla, exclude_list );
        }
        else {
          IdList exclude_list = getPartners( loss_id );
          win_id = ichk.getNextId( wla, exclude_list );
        }
      }
      else if ( loss_id == INVALID_ID ) {
        IdList exclude_list = getPartners( win_id );
        loss_id = ichk.getNextId( wla, exclude_list );
      }
      team_rt.setPlayerId( 0, win_id );
      team_rt.setPlayerId( 1, loss_id );
      if ( team_rt.size() == 3 ) {
        IdList exclude_list = getPartners( win_id ) + getPartners( loss_id );
        int id = getNextValidId( wla, exclude_list, Result::Win );
        team_rt.setPlayerId( 2, id );
      }
      TRACE(wla.status())
    }

    // state II - rest of the pack
    // start with triplette
    for ( int t = 0; t < ret.size(); t += 2 ) {
      Team& team_lt = ret[t];
      int id = wla.nextUnusedId( Result::Win );
      if ( id == INVALID_ID ) break;  // end is detected here
      team_lt.setPlayerId( 0, id );
      IdList exclude_list = getPartners( id );
      id = getNextValidId( wla, exclude_list, Result::Win );
      team_lt.setPlayerId( 1, id );
      if ( team_lt.size() == 3 ) {
        exclude_list += getPartners( id );
        // mit anderem spieler vergleichen?
        id = getNextValidId( wla, exclude_list, Result::Win );
        team_lt.setPlayerId( 2, id );
      }

      IdList opponents = getTeamOpponents( team_lt );
      Team& team_rt = ret[t+1];
      IdChecker ichk( this, opponents );
      id = ichk.getNextId( wla );
      team_rt.setPlayerId( 0, id );
      exclude_list = getPartners( id );
      id = ichk.getNextId( wla, exclude_list );
      team_rt.setPlayerId( 1, id );
      if ( team_rt.size() == 3 ) {
        exclude_list += getPartners( id );
        id = getNextValidId( wla, exclude_list, Result::Win );
        team_rt.setPlayerId( 2, id );
      }
      TRACE(wla.status())
    }
    return ret;
  }

  int getNextValidId( WinLossArr& wla, IdList const& exclude_list, Result res ) const
  {
    int id = getNextResId( wla, exclude_list, res );
    if ( id == INVALID_ID ) {
      // update round result? 
      id = wla.nextUnusedId( res );
    }
    return id;
  }

  int getNextResId( WinLossArr& wla, IdList const& exclude_list, Result res ) const
  {
    if ( exclude_list.isEmpty() ) {
      return wla.nextUnusedId( res );
    }
    int id = INVALID_ID;
    IdList diff_list;
    do {
      id = wla.nextUnusedId( res );
      if ( !exclude_list.contains( id ) ) break;
      diff_list.push_back( id );
    } while ( id != INVALID_ID );
    wla.insertIdList( diff_list, res );
    return id;
  }

  TeamList initTeams() const
  {
    TeamList ret;
    int const tc = teamCount();
    int const triplet_cnt = playerCount() % 4;
    ret.reserve( tc );
    for ( int t = 0; t < triplet_cnt; ++t ) ret.push_back( Team::triplet() );
    for ( int t = triplet_cnt; t < tc; ++t ) ret.push_back( Team::doublet() );
    return ret;
  }

  IdList getPartners( int id_self ) const
  {
    return getTeamPlayers( id_self, true );
  }

  IdList getTeamOpponents( Team const& team )
  {
    IdList ret;
    for ( int i = 0; i < team.size(); ++i ) {
      int id = team.playerId( i );
      ret += getOpponents( id );
    }
    return ret;
  }

  IdList getOpponents( int id_self ) const
  {
    return getTeamPlayers( id_self, false );
  }

  IdList getTeamPlayers( int id_self, bool own_team ) const
  {
    IdList ret;
    int idx = idx_map_[id_self];
    Player const& player = players_[idx];
    QSharedPointer<PlayerResult> result = player.result();
    for ( int m = 0; m < result->rounds(); ++m ) {
      Match const& match = result->match( m );
      ret += getTeamPlayers( match.team_lt_, id_self, own_team );
      ret += getTeamPlayers( match.team_rt_, id_self, own_team );
    }
    return ret;
  }

  IdList getTeamPlayers( Team const& team, int id_self, bool own_team ) const
  {
    IdList ret;
    bool test_team = own_team;
    for ( int i = 0; i < team.size(); ++i ) {
      int const id = team.playerId( i );
      if ( id == id_self ) {
        test_team = !own_team;
      }
      else {
        ret.push_back( id );
      }
    }
    return test_team != own_team ? ret : IdList();
  }

  int playerCount() const
  {
    return players_.size();
  }

  int teamCount() const
  {
    return ( playerCount() / 4 ) * 2;
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
  void initSiteList( SiteList const& site_list )
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
  return SuperMeleeRoundCalculator( player_list ).exec( round_idx_ );
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

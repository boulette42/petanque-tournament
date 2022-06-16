#include "round_model.h"
#include "tournament.h"
#include "match.h"
#include "player.h"
#include "player_result.h"
#include "settings.h"


namespace {

int const COLUMN_CNT_TEAMS = 3;
int const COLUMN_CNT_SUPERMELEE = 2;

Match empty_match;

}


RoundModelBase::RoundModelBase( Tournament& tournament )
  : tournament_( tournament )
{
}

RoundModelBase::~RoundModelBase() = default;

void RoundModelBase::initRound( Round const& round )
{
  beginResetModel();
  round_ = round;
  int const n_matches = round.size();
  row_to_match_.clear();
  row_to_match_.reserve( n_matches * matchRowCount() );
  for ( int m = 0; m < n_matches; ++m ) {
    int row = m * matchRowCount();
    row_to_match_.append( row++ );
    if ( global().site_enabled_ ) {
      if ( tournament_.isTeamMode() ) {
        row_to_match_.append( row );
      }
      ++row;
    }
    Match const& match = round_[m];
    for ( int i = 0; i < match.team_lt_.size(); ++i ) {
      row_to_match_.append( row++ );
    }
    row_to_match_.append( row );
  }
  endResetModel();
}

int RoundModelBase::matchRowCount() const
{
  return global().site_enabled_
    ? MATCH_ROW_CNT + 1
    : MATCH_ROW_CNT;
}


QModelIndex RoundModelBase::index( int row, int column, QModelIndex const& /*parent*/ ) const
{
  return createIndex( row, column, const_cast<RoundModelBase*>( this ) );
}

QModelIndex RoundModelBase::parent( QModelIndex const& /*mi*/ ) const
{
  return QModelIndex();
}

int RoundModelBase::rowCount( QModelIndex const& parent ) const
{
  if ( ! parent.isValid() ) return row_to_match_.size();
  else return 0;
}
  
int RoundModelBase::columnCount( QModelIndex const& parent ) const
{
  if ( parent.isValid() ) return 0;
  return tournament_.isTeamMode()
    ? COLUMN_CNT_TEAMS
    : COLUMN_CNT_SUPERMELEE;
}
  
QVariant RoundModelBase::data( QModelIndex const& mi, int role ) const
{
  enum DisplayFlags {
    DF_SITE = 1,
    DF_SM = 2,
    DF_TM = 4
  };

  int const row = mi.row();
  if ( 0 <= row && row < row_to_match_.size() ) {
    int const col = mi.column();
    switch ( role ) {
    case Qt::DisplayRole: {
      int const m = row_to_match_[row] / matchRowCount();
      int const k = ( row_to_match_[row] % matchRowCount() ) - global().site_enabled_;
      Match const& match = round_[m];
      if ( k == -1 ) {
        switch ( col ) {
        case 0:
          return tournament_.siteName( match.site_id_ );
        case 1:
          if ( ! tournament_.isTeamMode() ) {
            return match.result_.toString();
          }
          break;
        }
      } else if ( k == 0 ) {
        switch ( col ) {
        case 0:
          return tournament_.isTeamMode()
            ? tournament_.player( match.team_lt_.playerId( 0 ) ).verein()
            : tr( "Match %1" ).arg( m + 1 );
        case 1:
          if ( tournament_.isTeamMode() ) {
            return tournament_.player( match.team_rt_.playerId( 0 ) ).verein();
          }
          [[fallthrough]];
        case 2:
          return match.result_.toString();
        }
      } else if ( k <= 3 ) {
        switch ( col ) {
        case 0:
          return tournament_.playerName( match.team_lt_.playerId( k-1 ) );
        case 1:
          if ( match.team_rt_.size() >= k ) {
            return tournament_.playerName( match.team_rt_.playerId( k-1 ) );
          }
          break;
        }
      //} else {
      //  if ( col == 0 ) {
      //    return tr( "--------" );
      //  }
      }
      break; }
    }
  }
  return QVariant();
}

QVariant RoundModelBase::headerData( int section, Qt::Orientation orientation, int role ) const
{
  if ( orientation == Qt::Horizontal ) {
    switch ( role ) {
    case Qt::DisplayRole:
      if ( section == 0 ) {
        return tr( "Runde" );
      }
      break;
    }
  }
  return QVariant();
}


// --- RoundModel ------------------------------------------------------------

RoundModel::RoundModel( Tournament& tournament )
  : RoundModelBase( tournament )
{
}

bool RoundModel::setRound( int round_idx )
{
  if ( round_idx < 0 || tournament_.lastRoundIdx() < round_idx ) {
    return false;
  }
  Round round = tournament_.round( round_idx );
  if ( round.size() < 2 ) return false;
  initRound( round );
  round_idx_ = round_idx;
  return true;
}

RoundStatus RoundModel::roundStatus() const
{
  if ( round_idx_ == tournament_.lastRoundIdx() ) {
    int set_cnt = 0;
    int n_matches = round_.size();
    foreach ( Match const& match, round_ ) {
      if ( ! match.result_.isEmpty() ) ++set_cnt;
    }
    if ( set_cnt == 0 ) return RoundStatus::defined;
    if ( set_cnt == n_matches ) return RoundStatus::ready;
  }
  return RoundStatus::started;
}

bool RoundModel::isRoundChanged() const
{
  if ( round_idx_ < 0 ) return false;
  return round_ != tournament_.round( round_idx_);
}

Match const& RoundModel::match( QModelIndex const& mi ) const
{
  int m_idx = matchIndex( mi );
  return m_idx == INVALID_IDX ? empty_match : round_[m_idx];
}

int RoundModel::matchIndex( QModelIndex const& mi ) const
{
  if ( round_idx_ < 0 ) return INVALID_IDX;
  int const row = mi.row();
  if ( 0 <= row && row < row_to_match_.size() ) {
    return row_to_match_[row] / matchRowCount();
  }
  return INVALID_IDX;
}

void RoundModel::setResult( QModelIndex const& mi, int pts_lt, int pts_rt )
{
  if ( round_idx_ < 0 ) return;
  int const row = mi.row();
  if ( 0 <= row && row < row_to_match_.size() ) {
    int const m = row_to_match_[row] / matchRowCount();
    Match& match = round_[m];
    match.result_.setPointsLeft( pts_lt );
    match.result_.setPointsRight( pts_rt );
    QModelIndex mi_first = index( m * matchRowCount(), 0 );
    QModelIndex mi_last = index( m * matchRowCount(), 1 );
    if ( round_idx_ == tournament_.lastRoundIdx() ) {
      if ( isRoundChanged() ) {
        tournament_.setLastRoundFinished( false );
      }
    }
    emit dataChanged( mi_first, mi_last );
  }
}

void RoundModel::finishRound()
{
  tournament_.setRound( round_idx_, round_ );
  if ( round_idx_ == tournament_.lastRoundIdx() ) {
    tournament_.setLastRoundFinished( true );
  }
}

int RoundModel::columnCount() const
{
  return RoundModelBase::columnCount( QModelIndex() );
}


// --- PlayerMatchModel ------------------------------------------------------

PlayerMatchModel::PlayerMatchModel( Tournament const& tournament, Player const& player )
  : RoundModelBase( const_cast<Tournament&>( tournament ) )
{
  QSharedPointer<PlayerResult> result = player.result();
  if ( ! result ) return;
  Round round;
  for ( int i = 0; i < result->rounds(); ++i ) {
    Match match = result->match( i );
    if ( match.team_rt_.containsPlayer( player.id() ) ) {
      qSwap( match.team_lt_, match.team_rt_ );
      int const pts_lt = match.result_.pointsLeft();
      match.result_.setPointsLeft( match.result_.pointsRight() );
      match.result_.setPointsRight( pts_lt );
    }
    round.append( match );
  }
  initRound( round );
}


// --- TeamMatchModel --------------------------------------------------------

namespace {

Player matchPlayer( Tournament const& tournament, Team const& team, int round )
{
  for ( int i = 0; i < team.size(); ++i ) {
    Player const& player( tournament.player( team.playerId( i ) ) );
    if ( player.result() && player.result()->rounds() > round ) {
      Match const& match( player.result()->match( round ) );
      if ( match.team_lt_.size() ) return player;
    }
  }
  return Player();
}

}

TeamMatchModel::TeamMatchModel( Tournament const& tournament, Team const& team )
  : RoundModelBase( const_cast<Tournament&>( tournament ) )
{
  Player player;
  for ( int r = 0; r <= tournament.lastRoundIdx(); ++r ) {
    player = matchPlayer( tournament, team, r );
    if ( player.result() ) break;
  }
  QSharedPointer<PlayerResult> result = player.result();
  if ( ! result ) return;
  Round round;
  for ( int i = 0; i < result->rounds(); ++i ) {
    Player player = matchPlayer( tournament, team, i );
    if ( player.result() ) {
      Match match = player.result()->match( i );
      if ( match.team_rt_.containsPlayer( player.id() ) ) {
        qSwap( match.team_lt_, match.team_rt_ );
        int const pts_lt = match.result_.pointsLeft();
        match.result_.setPointsLeft( match.result_.pointsRight() );
        match.result_.setPointsRight( pts_lt );
      }
      round.append( match );
    }
  }
  initRound( round );
}

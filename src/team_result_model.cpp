#include "team_result_model.h"
#include "tournament.h"
#include "match.h"
#include "player.h"
#include "player_result.h"


namespace {

int playerIndex( PlayerList const& player_list, int id )
{
  if ( id != INVALID_ID ) {
    for ( int i = 0, n = player_list.size(); i < n; ++i ) {
      Player const& p = player_list[i];
      if ( p.id() == id ) {
        return i;
      }
    }
  }
  return INVALID_IDX;
}

}


TeamResultModel::TeamResultModel( Tournament const& tournament )
  : tournament_( tournament )
{
}

int TeamResultModel::columnCount() const
{
  return columnCount( QModelIndex() );
}

void TeamResultModel::updateTeamList()
{
  sorted_.clear();
  PlayerList const& player_list( tournament_.playerList() );
  QString error_string;
  TeamList team_list = tournament_.setTeams( player_list, error_string );
  int const n_teams = team_list.size();
  sorted_.reserve( n_teams );
  for ( int i = 0; i < n_teams; ++i ) {
    Team const& team( team_list[i] );
    for ( int p = 0; p < team.size(); ++p ) {
      int idx = playerIndex( player_list, team.playerId( p ) );
      if ( idx != INVALID_IDX ) {
        Player const& player( player_list[idx] );
        if ( player.selected() || player.result() ) {
          sorted_.append( idx );
          break;
        }
      }
    }
  }
  beginResetModel();
  sort( 0, Qt::DescendingOrder );
  endResetModel();
}

Team TeamResultModel::getTeam( QModelIndex const& mi ) const
{
  int const row = mi.row();
  if ( 0 <= row && row < sorted_.size() ) {
    int idx = sorted_[row];
    Player const& player = tournament_.playerList()[idx];
    if ( player.result() ) {
      for ( int i = 0; i < player.result()->rounds(); ++i ) {
        Match const& match( player.result()->match( i ) );
        if ( match.team_lt_.containsPlayer( player.id() ) ) {
          return match.team_lt_;
        }
        if ( match.team_rt_.containsPlayer( player.id() ) ) {
          return match.team_rt_;
        }
      }
    }
  }
  return Team();
}

QModelIndex TeamResultModel::index( int row, int column, QModelIndex const& /*parent*/ ) const
{
  return createIndex( row, column, const_cast<TeamResultModel*>( this ) );
}

QModelIndex TeamResultModel::parent( QModelIndex const& /*mi*/ ) const
{
  return QModelIndex();
}

int TeamResultModel::rowCount( QModelIndex const& parent ) const
{
  if ( ! parent.isValid() ) return sorted_.size();
  else return 0;
}

int TeamResultModel::columnCount( QModelIndex const& parent ) const
{
  if ( ! parent.isValid() ) return COLUMN_OFFSET + tournament_.lastRoundIdx() + 1;
  else return 0;
}

QVariant TeamResultModel::data( QModelIndex const& mi, int role ) const
{
  int const row = mi.row();
  if ( 0 <= row && row < sorted_.size() ) {
    int const idx = sorted_[row];
    int const col = mi.column();
    if ( role == Qt::DisplayRole ) {
      Player const& player = tournament_.playerList()[idx];
      switch ( col ) {
      case C_TEAM:   return player.verein();
      case C_POINTS: return player.result()
                                ? player.result()->resultPoints()
                                : 0;
      default:        return player.result()
                                ? player.result()->resultPoints( col-COLUMN_OFFSET )
                                : 0;

      }
    }
  }
  return QVariant();
}

QVariant TeamResultModel::headerData( int section, Qt::Orientation orientation, int role ) const
{
  if ( orientation == Qt::Horizontal ) {
    if ( role == Qt::DisplayRole ) {
      switch ( section ) {
      case C_TEAM:   return tr( "Team" );
      case C_POINTS: return tr( "Punkte" );
      default:       return tr( "Runde %1" ).arg( section-COLUMN_OFFSET+1 );
      }
    }
  }
  return QVariant();
}

void TeamResultModel::sort( int /*column*/, Qt::SortOrder /*order*/ )
{
  struct GreaterThan
  {
    PlayerList const& player_list_;

    GreaterThan( PlayerList const& player_list )
      : player_list_( player_list )
    { }

    bool operator()( int i_lhs, int i_rhs ) {
      Player const& lhs( player_list_[i_lhs] );
      Player const& rhs( player_list_[i_rhs] );
      int res_lhs = lhs.result() ? lhs.result()->resultPoints() : 0;
      int res_rhs = rhs.result() ? rhs.result()->resultPoints() : 0;
      return res_lhs > res_rhs;
    }
  };
  beginResetModel();
  std::sort( sorted_.begin(), sorted_.end(), GreaterThan( tournament_.playerList() ) );
  endResetModel();
}

#include "player_model.h"
#include "tournament.h"
#include "player.h"
#include "player_result.h"

enum Columns {
  C_ID,
  C_VORNAME,
  C_NAME,
  C_VEREIN,
  C_POINTS,
  COLUMN_CNT
};

static Player empty_player;

PlayerModel::PlayerModel( Tournament& tournament )
  : tournament_( tournament )
{
}

bool PlayerModel::isEmpty() const
{
  return tournament_.player_list_.isEmpty();
}

int PlayerModel::columnCount() const
{
  return columnCount( QModelIndex() );
}

QModelIndex PlayerModel::firstIndex() const
{
  return index( 0, 0 );
}

void PlayerModel::triggerSort()
{
  sort( sort_column_, ascending_ ? Qt::AscendingOrder : Qt::DescendingOrder );
}

void PlayerModel::toggleChecked( QModelIndex const& row_mi )
{
  QModelIndex mi( index( row_mi.row(), 0 ) );
  int cs = data( mi, Qt::CheckStateRole ).toInt();
  setData( mi, cs == Qt::Checked ? Qt::Unchecked : Qt::Checked, Qt::CheckStateRole );
  emit dataChanged( mi, mi );
}

void PlayerModel::setAllChecked( bool checked )
{
  beginResetModel();
  int const n_players = tournament_.player_list_.size();
  for ( int i = 0; i < n_players; ++i ) {
    Player& player( tournament_.player_list_[i] );
    player.setSelected( checked );
  }
  endResetModel();
}

Player const& PlayerModel::player( QModelIndex const& mi ) const
{
  int const row = mi.row();
  return 0 <= row && row < tournament_.player_list_.size()
    ? tournament_.player_list_[sorted_[row]]
    : empty_player;
}

void PlayerModel::setPlayer( QModelIndex const& mi, Player const& player )
{
  int const row = mi.row();
  if ( 0 <= row && row < tournament_.player_list_.size() ) {
    int idx = sorted_[row];
    if ( tournament_.player_list_[idx] != player ) {
      tournament_.player_list_[idx] = player;
      QModelIndex mi_first = index( mi.row(), 0 );
      QModelIndex mi_last = index( mi.row(), COLUMN_CNT - 1 );
      emit dataChanged( mi_first, mi_last );
    }
  }
}

void PlayerModel::addPlayer( Player player )
{
  player.setSelected( true );
  tournament_.addPlayer( player );
  triggerSort();
}

QModelIndex PlayerModel::index( int row, int column, QModelIndex const& /*parent*/ ) const
{
  return createIndex( row, column, const_cast<PlayerModel*>( this ) );
}

QModelIndex PlayerModel::parent( QModelIndex const& /*mi*/ ) const
{
  return QModelIndex();
}

int PlayerModel::rowCount( QModelIndex const& parent ) const
{
  if ( ! parent.isValid() ) return tournament_.player_list_.size();
  else return 0;
}

int PlayerModel::columnCount( QModelIndex const& parent ) const
{
  if ( ! parent.isValid() ) return COLUMN_CNT;
  else return 0;
}

QVariant PlayerModel::data( QModelIndex const& mi, int role ) const
{
  int const row = mi.row();
  if ( 0 <= row && row < tournament_.player_list_.size() ) {
    int const idx = sorted_[row];
    int const col = mi.column();
    switch ( role ) {
    case Qt::DisplayRole: {
      Player const& player = tournament_.player_list_[idx];
      switch ( col ) {
      case C_ID:      return player.id();
      case C_VORNAME: return player.vorname();
      case C_NAME:    return player.name();
      case C_VEREIN:  return player.verein();
      case C_POINTS:  return player.points();
      }
      break; }
    case Qt::CheckStateRole:
      if ( col == 0 ) {
        Player const& player = tournament_.player_list_[idx];
        return player.selected() ? Qt::Checked : Qt::Unchecked;
      }
      break;
    }
  }
  return QVariant();
}

bool PlayerModel::setData( QModelIndex const& mi, QVariant const& value, int role )
{
  bool result = false;
  int const row = mi.row();
  if ( 0 <= row && row < tournament_.player_list_.size() ) {
    int const idx = sorted_[row];
    switch ( role ) {
    case Qt::CheckStateRole: {
      Player& player = tournament_.player_list_[idx];
      player.setSelected( ! player.selected() );
      result = true;
      break; }
    case Qt::EditRole:
      result = QAbstractItemModel::setData( mi, value, role );
      break;
    }
    if ( result ) emit dataChanged( mi, mi );
  }
  return result;
}

Qt::ItemFlags PlayerModel::flags( QModelIndex const& mi ) const
{
  Qt::ItemFlags f = QAbstractItemModel::flags( mi );
  if ( mi.column() == 0 ) f |= Qt::ItemIsUserCheckable;
  return f;
}

QVariant PlayerModel::headerData( int section, Qt::Orientation orientation, int role ) const
{
  if ( orientation == Qt::Horizontal ) {
    switch ( role ) {
    case Qt::DisplayRole:
      switch ( section ) {
      case C_ID:      return tr( "      ID" );
      case C_VORNAME: return tr( "Vorname" );
      case C_NAME:    return tr( "Name" );
      case C_VEREIN:  return tournament_.isTeamMode() ? tr( "Team" ) : tr( "Verein" );
      case C_POINTS:  return tr( "Punkte" );
      }
      break;
    }
  }
  return QVariant();
}

void PlayerModel::sort( int column, Qt::SortOrder order )
{
  struct LowerThan {
    PlayerList const player_list_;
    int const col_;
    bool const ascending_;
    bool const team_mode_;

    LowerThan( Tournament const& tournament, int col, bool ascending )
      : player_list_( tournament.playerList() )
      , col_( col )
      , ascending_( ascending )
      , team_mode_( tournament.isTeamMode() )
    { }

    bool operator()( int i_lhs, int i_rhs )
    {
      int eff_lhs = ascending_ ? i_lhs : i_rhs;
      int eff_rhs = ascending_ ? i_rhs : i_lhs;
      Player const& lhs( player_list_[eff_lhs] );
      Player const& rhs( player_list_[eff_rhs] );
      if ( lhs.selected() != rhs.selected()
        && ( col_ == C_ID 
          || col_ == C_VEREIN /*&& team_mode_*/ ) )
      {
        return lhs.selected() > rhs.selected();
      }
      switch ( col_ ) {
      case C_ID:      return lhs.id() < rhs.id();
      case C_VORNAME: return lhs.vorname().compare( rhs.vorname(), Qt::CaseInsensitive ) < 0;
      case C_NAME:    return lhs.name().compare( rhs.name().toLower(), Qt::CaseInsensitive ) < 0;
      case C_VEREIN:  return lhs.verein().compare( rhs.verein().toLower(), Qt::CaseInsensitive ) < 0;
      case C_POINTS:  return lhs.points() > rhs.points();   // umgekehrte Sortierung
      }
      return false;
    }
  };

  int const n_players = tournament_.player_list_.size();
  if ( sorted_.size() != n_players ) {
    sorted_.resize( n_players );
    for ( int i = 0; i < n_players; ++i ) {
      sorted_[i] = i;
    }
  }
  beginResetModel();
  if ( column > -1 ) {
    LowerThan lt( tournament_, column, order == Qt::AscendingOrder );
    std::sort( sorted_.begin(), sorted_.end(), lt );
  }
  endResetModel();
}


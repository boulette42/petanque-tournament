#include "player_result_model.h"
#include "tournament.h"
#include "player.h"
#include "player_result.h"
#include "settings.h"


PlayerResultModel::PlayerResultModel( Tournament const& tournament )
  : tournament_( tournament )
{
}

int PlayerResultModel::columnCount() const
{
  return columnCount( QModelIndex() );
}

void PlayerResultModel::updatePlayerList()
{
  sorted_.clear();
  PlayerList player_list( tournament_.playerList() );
  int const n_players = player_list.size();
  sorted_.reserve( n_players );
  for ( int i = 0; i < n_players; ++i ) {
    Player const& player( player_list[i] );
    if ( player.selected() || player.result() ) {
      sorted_.append( i );
    }
  }
  sort( 0, Qt::DescendingOrder );
}

Player const& PlayerResultModel::getPlayer( QModelIndex const& mi ) const
{
  static Player empty;
  int const row = mi.row();
  if ( 0 <= row && row < sorted_.size() ) {
    int idx = sorted_[row];
    return tournament_.playerList()[idx];
  }
  return empty;
}

QModelIndex PlayerResultModel::index( int row, int column, QModelIndex const& /*parent*/ ) const
{
  return createIndex( row, column, const_cast<PlayerResultModel*>( this ) );
}

QModelIndex PlayerResultModel::parent( QModelIndex const& /*mi*/ ) const
{
  return QModelIndex();
}

int PlayerResultModel::rowCount( QModelIndex const& parent ) const
{
  if ( ! parent.isValid() ) return sorted_.size();
  else return 0;
}

int PlayerResultModel::columnCount( QModelIndex const& parent ) const
{
  if ( ! parent.isValid() ) return COLUMN_OFFSET + tournament_.lastRoundIdx() + 2;
  else return 0;
}

QVariant PlayerResultModel::data( QModelIndex const& mi, int role ) const
{
  int const row = mi.row();
  if ( 0 <= row && row < sorted_.size() ) {
    int const idx = sorted_[row];
    int const col = mi.column();
    if ( role == Qt::DisplayRole ) {
      Player const& player = tournament_.playerList()[idx];
      switch ( col ) {
      case C_ID:
        return player.id();
      case C_NAME:
        return QStringLiteral("%1, %2" ).arg( player.lastName(), player.firstName() );
      case C_ROUNDS:
        return player.result()
          ? player.result()->wonRounds()
          : 0;
      case C_POINTS:
        return player.result()
          ? player.result()->resultPoints( global().isFormuleX() )
          : 0;
      default:
        if ( col-COLUMN_OFFSET <= tournament_.lastRoundIdx() ) {
          return player.result()
            ? player.result()->resultPoints( col-COLUMN_OFFSET, global().isFormuleX() )
            : 0;
        }
      }
    } else if ( role == Qt::TextAlignmentRole ) {
      return col <= C_NAME ? Qt::AlignLeft : Qt::AlignRight;
    }
  }
  return QVariant();
}

QVariant PlayerResultModel::headerData( int section, Qt::Orientation orientation, int role ) const
{
  if ( orientation == Qt::Horizontal ) {
    if ( role == Qt::DisplayRole ) {
      switch ( section ) {
      case C_ID:
        return Tournament::tr( "ID" );
      case C_NAME:
        return Tournament::tr( "Name" );
      case C_ROUNDS:
        return Tournament::tr( "Siege" );
      case C_POINTS:
        return Tournament::tr( "Punkte" );
      default:
        if ( section-COLUMN_OFFSET <= tournament_.lastRoundIdx() ) {
          return Tournament::tr( "Runde %1" ).arg( section-COLUMN_OFFSET+1 );
        }
      }
    }
  }
  return QVariant();
}

void PlayerResultModel::sort( int /*column*/, Qt::SortOrder /*order*/ )
{
  struct GreaterThan
  {
    Tournament const& tournament_;

    GreaterThan( Tournament const& tournament )
      : tournament_( tournament )
    { }

    bool operator()( int i_lhs, int i_rhs ) {
      Player const& lhs( tournament_.playerList()[i_lhs] );
      Player const& rhs( tournament_.playerList()[i_rhs] );
      if ( !lhs.result() ) return false;
      if ( !rhs.result() ) return true;
      bool const formule_x = global().isFormuleX();
      int const res_lhs = lhs.result()->resultPoints( formule_x );
      int const res_rhs = rhs.result()->resultPoints( formule_x );
      if ( formule_x ) return res_lhs > res_rhs;
      int const wr_lhs = lhs.result()->wonRounds();
      int const wr_rhs = rhs.result()->wonRounds();
      if ( wr_lhs != wr_rhs ) return wr_lhs > wr_rhs;
      int const bh_lhs = lhs.result()->buchholzPoints( tournament_ );
      int const bh_rhs = rhs.result()->buchholzPoints( tournament_ );
      if ( bh_lhs != bh_rhs ) return bh_lhs > bh_rhs;
      int const fw_lhs = lhs.result()->buchholzFeinwertung( tournament_ );
      int const fw_rhs = rhs.result()->buchholzFeinwertung( tournament_ );
      return fw_lhs > fw_rhs;
    }
  };
  beginResetModel();
  std::sort( sorted_.begin(), sorted_.end(), GreaterThan( tournament_ ) );
  endResetModel();
}

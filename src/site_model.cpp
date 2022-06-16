#include "site_model.h"
#include "site.h"
#include "tournament.h"


namespace {

enum Columns {
  C_NAME = 0,
  COLUMN_CNT
};

}


SiteModel::SiteModel( Tournament& tournament )
  : tournament_( tournament )
{
}

SiteModel::~SiteModel() = default;

int SiteModel::columnCount() const
{
  return columnCount( QModelIndex() );
}

bool SiteModel::setSiteCount( int site_cnt )
{
  beginResetModel();
  bool res = tournament_.setSiteCount( site_cnt );
  endResetModel();
  return res;
}

void SiteModel::setAllChecked( bool checked )
{
  beginResetModel();
  int const n_sites = tournament_.siteCount();
  for ( int i = 0; i < n_sites; ++i ) {
    Site& site( tournament_.site_list_[i] );
    site.setSelected( checked );
  }
  endResetModel();
}

QModelIndex SiteModel::index( int row, int column, QModelIndex const& /*parent*/ ) const
{
  return createIndex( row, column, const_cast<SiteModel*>( this ) );
}

QModelIndex SiteModel::parent( QModelIndex const& /*mi*/ ) const
{
  return QModelIndex();
}

int SiteModel::rowCount( QModelIndex const& parent ) const
{
  if ( ! parent.isValid() ) return tournament_.site_cnt_;
  else return 0;
}

int SiteModel::columnCount( QModelIndex const& parent ) const
{
  if ( ! parent.isValid() ) return COLUMN_CNT;
  else return 0;
}

QVariant SiteModel::data( QModelIndex const& mi, int role ) const
{
  int const row = mi.row();
  if ( 0 <= row && row < tournament_.siteCount() ) {
    int const idx = row;
    int const col = mi.column();
    switch ( role ) {
    case Qt::DisplayRole: {
      Site const& site = tournament_.site_list_[idx];
      if ( col == C_NAME ) {
        return site.name();
      }
      break; }
    case Qt::CheckStateRole:
      if ( col == C_NAME ) {
        Site const& site( tournament_.site_list_[idx] );
        return site.selected() ? Qt::Checked : Qt::Unchecked;
      }
      break;
    }
  }
  return QVariant();
}

bool SiteModel::setData( QModelIndex const& mi, QVariant const& value, int role )
{
  bool result = false;
  int const row = mi.row();
  if ( 0 <= row && row < tournament_.siteCount() ) {
    int const idx = row;
    switch ( role ) {
    case Qt::CheckStateRole: {
      Site& site( tournament_.site_list_[idx] );
      site.setSelected( ! site.selected() );
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

Qt::ItemFlags SiteModel::flags( QModelIndex const& mi ) const
{
  Qt::ItemFlags f = QAbstractItemModel::flags( mi );
  if ( mi.column() == 0 ) f |= Qt::ItemIsUserCheckable;
  return f;
}

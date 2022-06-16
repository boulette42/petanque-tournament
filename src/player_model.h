#ifndef PLAYER_MODEL_H
#define PLAYER_MODEL_H

#include "config.h"
#include <qabstractitemmodel.h>


class PlayerModel : public QAbstractItemModel
{
  Tournament& tournament_;
  IdxList sorted_;
  int sort_column_ = 0;
  int ascending_ = true;

public:
  explicit PlayerModel( Tournament& tournament );

  bool isEmpty() const;
  int columnCount() const;
  QModelIndex firstIndex() const;
  Player const& player( QModelIndex const& index ) const;
  void setPlayer( QModelIndex const& index, Player const& player );
  void addPlayer( Player player );
  void triggerSort();
  void toggleChecked( QModelIndex const& mi );
  void setAllChecked( bool checked );

protected:
  QModelIndex index( int row, int column, QModelIndex const& parent = QModelIndex() ) const override;
  QModelIndex parent( QModelIndex const& /*mi*/ ) const override;
  int rowCount( QModelIndex const& parent ) const override;
  int columnCount( QModelIndex const& parent ) const override;
  QVariant data( QModelIndex const& mi, int role ) const override;
  bool setData( QModelIndex const& mi, QVariant const& value, int role ) override;
  Qt::ItemFlags flags( QModelIndex const& mi ) const;
  QVariant headerData( int section, Qt::Orientation orientation, int role ) const override;
  void sort( int column, Qt::SortOrder order ) override;

private:
  PlayerModel( PlayerModel const& );
  PlayerModel& operator=( PlayerModel const& );
};

#endif  // PLAYER_MODEL_H

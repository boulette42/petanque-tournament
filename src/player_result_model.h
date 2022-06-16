#ifndef PLAYER_RESULT_MODEL_H
#define PLAYER_RESULT_MODEL_H

#include "config.h"
#include <qabstractitemmodel.h>

enum Columns {
  C_ID,
  C_NAME,
  C_POINTS,
  COLUMN_OFFSET
};

class PlayerResultModel : public QAbstractItemModel
{
  Tournament const& tournament_;
  IdList sorted_;

public:
  explicit PlayerResultModel( Tournament const& tournament );
  int columnCount() const;
  void updatePlayerList();
  Player const& getPlayer( QModelIndex const& mi ) const;

protected:
  QModelIndex index( int row, int column, QModelIndex const& parent = QModelIndex() ) const override;
  QModelIndex parent( QModelIndex const& /*mi*/ ) const override;
  int rowCount( QModelIndex const& parent ) const override;
  int columnCount( QModelIndex const& parent ) const override;
  QVariant data( QModelIndex const& mi, int role ) const override;
  QVariant headerData( int section, Qt::Orientation orientation, int role ) const override;
  void sort( int column, Qt::SortOrder order ) override;
};

#endif  // PLAYER_RESULT_MODEL_H

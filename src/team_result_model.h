#ifndef TEAM_RESULT_MODEL_H
#define TEAM_RESULT_MODEL_H

#include "config.h"
#include <qabstractitemmodel.h>

class TeamResultModel : public QAbstractItemModel
{
  enum Columns {
    C_TEAM,
    C_POINTS,
    COLUMN_OFFSET
  };

  Tournament const& tournament_;
  IdList sorted_;

public:
  explicit TeamResultModel( Tournament const& tournament );
  int columnCount() const;
  void updateTeamList();
  Team getTeam( QModelIndex const& mi ) const;

protected:
  QModelIndex index( int row, int column, QModelIndex const& parent = QModelIndex() ) const override;
  QModelIndex parent( QModelIndex const& /*mi*/ ) const override;
  int rowCount( QModelIndex const& parent ) const override;
  int columnCount( QModelIndex const& parent ) const override;
  QVariant data( QModelIndex const& mi, int role ) const override;
  QVariant headerData( int section, Qt::Orientation orientation, int role ) const override;
  void sort( int column, Qt::SortOrder order ) override;
};

#endif  // TEAM_RESULT_MODEL_H

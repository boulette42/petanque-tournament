#ifndef ROUND_MODEL_H
#define ROUND_MODEL_H

#include "config.h"
#include <qabstractitemmodel.h>


enum class RoundStatus {
  defined,
  started,
  ready,
};


class RoundModelBase : public QAbstractItemModel
{
  int const MATCH_ROW_CNT = 5;

protected:
  Tournament& tournament_;
  Round round_;
  int round_idx_;
  QVector<int> row_to_match_;
  QVector<int> sorted_;

public:
  explicit RoundModelBase( Tournament& tournament );
  ~RoundModelBase();

protected:
  void initRound( Round const& round );
  void initRound( Round const& round, int round_idx );
  int matchRowCount() const;
  bool isTeamOnlyShown() const;

  QModelIndex index( int row, int column, QModelIndex const& parent = QModelIndex() ) const override;
  QModelIndex parent( QModelIndex const& /*mi*/ ) const override;
  int rowCount( QModelIndex const& parent ) const override;
  int columnCount( QModelIndex const& parent ) const override;
  QVariant data( QModelIndex const& mi, int role ) const override;
  QVariant RoundModelBase::teamOnlyData( QModelIndex const& mi, int role ) const;
  QVariant headerData( int section, Qt::Orientation orientation, int role ) const override;
  void sort( int column, Qt::SortOrder order ) override;
};


class RoundModel : public RoundModelBase
{
  int round_idx_ = INVALID_IDX;

public:
  RoundModel( Tournament& tournament );

  int currentRound() const { return round_idx_; }
  bool isRoundChanged() const;
  RoundStatus roundStatus() const;
  bool setRound( int round_idx );
  void finishRound();
  Match const& match( QModelIndex const& mi ) const;
  int matchIndex( QModelIndex const& mi ) const;
  void setResult( QModelIndex const& mi, int pts_lt, int pts_rt );
  int columnCount() const;
};


class PlayerMatchModel : public RoundModelBase
{
public:
  PlayerMatchModel( Tournament const& tournament, Player const& player );
};


class TeamMatchModel : public RoundModelBase
{
public:
  TeamMatchModel( Tournament const& tournament, Team const& team );
};

#endif  // ROUND_MODEL_H

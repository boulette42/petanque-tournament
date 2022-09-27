#ifndef ROUND_CALCULATOR_H
#define ROUND_CALCULATOR_H

#include "config.h"

class QWidget;


class RoundCalculator
{
  QWidget* parent_;
  QString error_string_;
  int round_idx_ = -1;

public:
  explicit RoundCalculator( QWidget* parent, int round_idx );
  int roundIndex() const { return round_idx_; }

  Round calcRound( PlayerList const& player_list );
  Round calcRound( PlayerList const& player_list, TeamList const& team_list );
  Round calcSites( Round const& round, Tournament const& tournament );
  void setError( QString const& error_string );
  QString lastError() const;
};

#endif  // ROUND_CALCULATOR_H

#ifndef PLAYER_RESULT_H
#define PLAYER_RESULT_H

#include "config.h"

class PlayerResult
{
  int id_ = INVALID_ID;
  QVector<Match> match_list_;

public:
  explicit PlayerResult( int id );
  void setMatch( int round_idx, Match const& match );
  Match const& match( int round_idx ) const;
  int rounds() const { return match_list_.size(); }
  int resultPoints() const;
  int resultPoints( int round_idx ) const;
  int wonRounds() const;
  int wonRoundsUntil( int max_round ) const;
};

#endif  // PLAYER_RESULT_H

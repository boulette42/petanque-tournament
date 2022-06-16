#ifndef RESULT_H
#define RESULT_H

#include "config.h"

class Result
{
  unsigned short points_lt_ = 0;
  unsigned short points_rt_ = 0;

public:
  Result() = default;
  bool isEmpty() const { return points_lt_ == 0 && points_rt_ == 0; }
  int pointsLeft() const { return points_lt_; }
  void setPointsLeft( unsigned short points ) { points_lt_ = points; }
  int pointsRight() const { return points_rt_; }
  void setPointsRight( unsigned short points ) { points_rt_ = points; }
  int resultPointsLeft() const;
  int resultPointsRight() const;
  QString toString() const;
  bool operator==( Result const& rhs ) const;
};

#endif  // RESULT_H

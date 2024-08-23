#include "result.h"

QString Result::toString() const
{
  return isEmpty()
    ? QString()
    : QStringLiteral( "%1 : %2" ).arg( points_lt_ ).arg( points_rt_ );
}

bool Result::operator==( Result const& rhs ) const
{
  return points_lt_ == rhs.points_lt_
      && points_rt_ == rhs.points_rt_;
}

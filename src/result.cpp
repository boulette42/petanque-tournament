#include "result.h"
#include "config.h"
#include <qstring.h>

namespace {

int calcPoints( int lt, int rt )
{
  int res = lt;
  if ( lt > rt ) {
    res += 100;
    res += lt - rt;
  }
  return res;
}

}


int Result::resultPointsLeft() const
{
  return calcPoints( points_lt_, points_rt_ );
}

int Result::resultPointsRight() const
{
  return calcPoints( points_rt_, points_lt_ );
}

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

#ifndef MATCH_H
#define MATCH_H

#include "team.h"
#include "result.h"

struct Match
{
  Team team_lt_;
  Team team_rt_;
  Result result_;
  int site_id_ = INVALID_ID;

  int playerCount() const { return team_lt_.size() + team_rt_.size(); }

  static Match readFromJson( QJsonObject const& json, QString& error_string );
  void writeToJson( QJsonObject& json ) const;
};

inline bool operator==( Match const& lhs, Match const& rhs )
{
  return lhs.team_lt_ == rhs.team_lt_
      && lhs.team_rt_ == rhs.team_rt_
      && lhs.result_ == rhs.result_;
}

#endif  // MATCH_H

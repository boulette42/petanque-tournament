#ifndef TEAM_H
#define TEAM_H

#include "config.h"

class QJsonObject;

class Team
{
  Q_DECLARE_TR_FUNCTIONS(Team)

  IdList player_list_;

public:
  Team() = default;

  explicit Team( IdList const& player_list );

  static Team doublet()
  {
    return Team( 2 );
  }

  static Team triplet()
  {
    return Team( 3 );
  }

  int size() const
  {
    return player_list_.size();
  }

  int playerId( int pos ) const
  {
    return pos < player_list_.size() ? player_list_[pos] : INVALID_ID;
  }

  void setPlayerId( int pos, int id )
  {
    if ( pos < player_list_.size() ) player_list_[pos] = id;
  }

  bool containsPlayer( int id ) const
  {
    return player_list_.contains( id );
  }

  bool operator==( Team const& rhs ) const
  {
    return player_list_ == rhs.player_list_;
  }

  static Team readFromJson( QJsonObject const& json, QString& error_string );
  void writeToJson( QJsonObject& t_arr ) const;

private:
  explicit Team( int size )
    : player_list_( size, INVALID_ID )
  {
  }
};

#endif  // TEAM_H

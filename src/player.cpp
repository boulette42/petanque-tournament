#include "player.h"
#include "json_common.h"
#include "match.h"
#include "player_result.h"
#include <qjsonobject.h>
#include <qstring.h>
#include <qstringlist.h>


// --- Player ----------------------------------------------------------------

Player::Player() = default;

Player::Player(
  int id,
  QString const& last_name,
  QString const& first_name,
  QString const& association,
  QString const& team,
  int points )
  : id_( id )
  , last_name_( last_name )
  , first_name_( first_name )
  , association_( association )
  , team_( team )
  , points_( points )
{
}

Player::Player( int id, Player const& other )
  : id_( id )
  , last_name_( other.last_name_ )
  , first_name_( other.first_name_ )
  , association_( other.association_ )
  , team_( other.team_ )
  , points_( other.points_ )
  , selected_( other.selected_ )
{
}

Player::~Player() = default;

bool Player::equals( Player const& rhs ) const
{
  return id_ == rhs.id_
    && first_name_ == rhs.first_name_
    && last_name_ == rhs.last_name_
    && association_ == rhs.association_
    && team_ == rhs.team_
    && points_ == rhs.points_;
  // selected_ und result_ irrelevant
}

void Player::setMatch( int round_idx, Match const& match )
{
  if ( ! result_ ) {
    result_ = QSharedPointer<PlayerResult>( new PlayerResult( id_ ) );
  }
  result_->setMatch( round_idx, match );
}

void Player::updatePoints()
{
  points_ = result_ ? result_->resultPoints() : 0;
}

Player Player::fromCsvLine( QString const& line, QString& error_string, char delimiter )
{
  Player player;
  QStringList fields = line.split( QLatin1Char( delimiter ) );
  int const n = fields.size();
  for ( int i = 0; i < n; ++i ) {
    QString fld = fields[i].trimmed();
    switch ( i ) {
    case 0:
      if ( !fld.isEmpty() ) {
        bool ok = false;
        player.id_ = fld.toInt( &ok );
        if ( ! ok ) {
          error_string = tr( "ID nicht nummerisch" );
        }
      }
      break;
    case 1:
      player.first_name_ = fld;
      break;
    case 2:
      player.last_name_ = fld;
      break;
    case 3:
      player.points_ = fld.toInt();
      break;
    case 4:
      player.association_ = fld;
      break;
    case 5:
      player.team_ = fld;
      break;
    }
  }
  if ( player.firstName().isEmpty() && player.lastName().isEmpty() ) {
    error_string = tr( "Spielername nicht angegeben" );
  }
  return player;
}

QString Player::toCsvLine( char delimiter ) const
{
  int points = points_;
  if ( result_ ) points += result_->resultPoints();
  return QStringLiteral( "%1%2%3%2%4%2%5%2%6%2%7" )
    .arg( id_ )
    .arg( QLatin1Char( delimiter ) )
    .arg( first_name_ )
    .arg( last_name_ )
    .arg( points )
    .arg( association_ )
    .arg( team_ );
}

bool operator==( Player const& lhs, Player const& rhs )
{
  return lhs.equals( rhs );
}

bool operator!=( Player const& lhs, Player const& rhs )
{
  return ! lhs.equals( rhs );
}

Player Player::readFromJson( QJsonObject const& json, QString& error_string )
{
  int id = INVALID_ID;
  QString first_name;
  QString last_name;
  QString association;
  QString team;
  int points = 0;
  bool selected = false;
  if ( ! json.contains( J_ID ) ) {
    error_string = tr( "Spieler-ID nicht definiert" );
  } else {
    id = readInt( json[J_ID] );
  }
  if ( id != INVALID_ID ) {
    bool ok = false;
    if ( json.contains( J_FIRST_NAME ) ) {
      first_name = json[J_FIRST_NAME].toString();
      ok = ! first_name.isEmpty();
    }
    if ( json.contains( J_LAST_NAME ) ) {
      last_name = json[J_LAST_NAME].toString();
      ok |= ! last_name.isEmpty();
    }
    if ( json.contains( J_ASSOCIATION ) ) {
      association = json[J_ASSOCIATION].toString();
    }
    if ( json.contains( J_TEAM ) ) {
      team = json[J_TEAM].toString();
    }
    if ( json.contains( J_POINTS ) ) {
      points = readInt( json[J_POINTS], INVALID_IDX );
      if ( points < 0 || points > 10000 ) {
        points = 0;
      }
    }
    if ( json.contains( J_SELECTED ) ) {
      selected = json[J_SELECTED].toBool();
    }
    if ( ! ok ) {
      error_string = tr( "Name des Spielers nicht enthalten" );
      id = INVALID_ID;
    }
  } else {
    error_string = tr( "ungültige Spieler-ID" );
  }
  Player ret( id, last_name, first_name, association, team, points );
  ret.setSelected( selected );
  return ret;
}

void Player::writeToJson( QJsonObject& json ) const
{
  json[J_ID] = id_;
  if ( ! first_name_.isEmpty() ) json[J_FIRST_NAME] = first_name_;
  if ( ! last_name_.isEmpty() ) json[J_LAST_NAME] = last_name_;
  if ( ! association_.isEmpty() ) json[J_ASSOCIATION] = association_;
  if ( ! team_.isEmpty() ) json[J_TEAM] = team_;
  if ( points_ > 0 ) json[J_POINTS] = points_;
  if ( selected_ ) json[J_SELECTED] = selected_;
}


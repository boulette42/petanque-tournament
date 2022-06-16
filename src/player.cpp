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
  QString const& name,
  QString const& vorname,
  QString const& verein,
  int points )
  : id_( id )
  , name_( name )
  , vorname_( vorname )
  , verein_( verein )
  , points_( points )
{
}

Player::Player( int id, Player const& other )
  : id_( id )
  , name_( other.name_ )
  , vorname_( other.vorname_ )
  , verein_( other.verein_ )
  , points_( other.points_ )
  , selected_( other.selected_ )
{
}

Player::~Player() = default;

bool Player::equals( Player const& rhs ) const
{
  return id_ == rhs.id_
    && vorname_ == rhs.vorname_
    && name_ == rhs.name_
    && verein_ == rhs.verein_
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

Player Player::fromCsvLine( QString const& line, QString& error_string, char delimiter )
{
  Player player;
  QStringList fields = line.split( QLatin1Char( delimiter ) );
  int const n = fields.size();
  if ( n < 2 ) {
    error_string = QStringLiteral( "Name oder ID nicht angegeben" );
  } else for ( int i = 0; i < n; ++i ) {
    QString fld = fields[i].trimmed();
    switch ( i ) {
    case 0: {
      bool ok = false;
      player.id_ = fld.toInt( &ok );
      if ( ! ok ) {
        error_string = QStringLiteral( "ID nicht nummerisch" );
      }
      break; }
    case 1:
      player.vorname_ = fld;
      break;
    case 2:
      player.name_ = fld;
      break;
    case 3:
      player.points_ = fld.toInt();
      break;
    case 4:
      player.verein_ = fld;
      break;
    }
  }
  return player;
}

QString Player::toCsvLine( char delimiter ) const
{
  int points = points_;
  if ( result_ ) points += result_->resultPoints();
  return QStringLiteral( "%1%2%3%2%4%2%5%2%6" )
    .arg( id_ )
    .arg( QLatin1Char( delimiter ) )
    .arg( vorname_ )
    .arg( name_ )
    .arg( points )
    .arg( verein_ );
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
  QString vorname;
  QString name;
  QString verein;
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
      vorname = json[J_FIRST_NAME].toString();
      ok = ! vorname.isEmpty();
    }
    if ( json.contains( J_LAST_NAME ) ) {
      name = json[J_LAST_NAME].toString();
      ok |= ! name.isEmpty();
    }
    if ( json.contains( J_ASSOCIATION ) ) {
      verein = json[J_ASSOCIATION].toString();
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
  Player ret( id, name, vorname, verein, points );
  ret.setSelected( selected );
  return ret;
}

void Player::writeToJson( QJsonObject& json ) const
{
  json[J_ID] = id_;
  if ( ! vorname_.isEmpty() ) json[J_FIRST_NAME] = vorname_;
  if ( ! name_.isEmpty() ) json[J_LAST_NAME] = name_;
  if ( ! verein_.isEmpty() ) json[J_ASSOCIATION] = verein_;
  if ( points_ > 0 ) json[J_POINTS] = points_;
  if ( selected_ ) json[J_SELECTED] = selected_;
}


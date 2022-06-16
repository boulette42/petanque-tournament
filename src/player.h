#ifndef PLAYER_H
#define PLAYER_H

#include "config.h"

class QJsonObject;

class Player
{
  int id_ = INVALID_ID;
  QString name_;
  QString vorname_;
  QString verein_;
  int points_ = 0;
  bool selected_ = false;
  QSharedPointer<PlayerResult> result_;

public:
  Player();
  Player(
    int id,
    QString const& name,
    QString const& vorname = QString(),
    QString const& verein = QString(),
    int points = 0 );
  Player( int id, Player const& copy_from );
  ~Player();

  bool equals( Player const& player ) const;

  int id() const { return id_; }
  QString const& vorname() const { return vorname_; }
  QString const& name() const { return name_; }
  QString const& verein() const { return verein_; }
  QSharedPointer<PlayerResult> result() const { return result_; }
  int points() const { return points_; }
  void setPoints( int points ) { points_ = points; }
  bool selected() const { return selected_; }
  void setSelected( bool selected ) { selected_ = selected; }

  void setMatch( int round_idx, Match const& match );

  static Player fromCsvLine( QString const& line, QString& error_string, char delimiter = ';' );
  QString toCsvLine( char delimiter = ';' ) const;

  static Player readFromJson( QJsonObject const& json, QString& error_string );
  void writeToJson( QJsonObject& json ) const;
};

bool operator==( Player const& lhs, Player const& rhs );
bool operator!=( Player const& lhs, Player const& rhs );

#endif  // PLAYER_H

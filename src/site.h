#ifndef SITE_H
#define SITE_H

#include "config.h"

class QJsonObject;

class Site
{
  int id_ = INVALID_ID;
  QString name_;
  bool selected_ = true;

public:
  explicit Site( int id, QString const& name = QString() );
  ~Site();

  int id() const { return id_; }
  bool selected() const { return selected_; }
  void setSelected( bool selected ) { selected_ = selected; }
  QString name() const;

  static Site readFromJson( QJsonObject const& json, QString& error_string );
  void writeToJson( QJsonObject& json ) const;
};

#endif  //SITE_H


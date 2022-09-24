#include "site.h"
#include "json_common.h"
#include <qjsonobject.h>

Site::Site()
  : id_( INVALID_ID )
  , name_()
  , selected_( false )
{
}

Site::Site( int id, QString const& name )
  : id_( id )
  , name_( name )
  , selected_( true )
{
}

Site::~Site() = default;

QString Site::name() const
{
  if ( !name_.isEmpty() ) return name_;
  return id_ != INVALID_ID ? tr( "Platz %1" ).arg( id_ ) : QString();
}

Site Site::readFromJson( QJsonObject const& json, QString& error_string )
{
  Site ret( INVALID_ID, QString() );
  ret.selected_ = false;
  if ( ! json.contains( J_ID ) ) {
    error_string = tr( "Platz-ID nicht definiert" );
  } else {
    ret.id_ = readInt( json[J_ID] );
    if ( json.contains( J_SITE_NAME ) ) {
      ret.name_ = json[J_SITE_NAME].toString();
    }
    if ( json.contains( J_SELECTED ) ) {
      ret.selected_ = json[J_SELECTED].toBool();
    }
  }
  return ret;
}

void Site::writeToJson( QJsonObject& json ) const
{
  json[J_ID] = id_;
  if ( ! name_.isEmpty() ) json[J_SITE_NAME] = name_;
  if ( selected_ ) json[J_SELECTED] = selected_;
}

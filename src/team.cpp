#include "team.h"
#include "json_common.h"
#include <qjsonarray.h>
#include <qjsonobject.h>

Team Team::readFromJson( QJsonObject const& json, QString& error_string )
{
  Team ret;
  if ( json.contains( J_TEAM ) && json[J_TEAM].isArray() ) {
    QJsonArray id_arr = json[J_TEAM].toArray();
    int const n_id = id_arr.size();
    switch ( n_id ) {
    case 2:
      ret = Team::doublet();
      break;
    case 3:
      ret = Team::triplet();
      break;
    default:
      error_string = tr( "Falsche Spieler-Anzahl im Team" );
      return ret;
    }
    for ( int i = 0; i < n_id; ++i ) {
      QJsonObject id_obj = id_arr[i].toObject();
      if ( ! id_obj.contains( J_ID ) ) {
        error_string = tr( "Spieler-ID in Team nicht angegeben" );
        return ret;
      }
      int id = readInt( id_obj[J_ID] );
      if ( id == INVALID_ID ) {
        error_string = tr( "Team: ungültige ID angegeben" );
        return ret;
      }
      ret.setPlayerId( i, id );
    }
  } else {
    error_string = tr( "Keine Spieler-ID in Team enthalten" );
  }
  return ret;
}

void Team::writeToJson( QJsonObject& t_obj ) const
{
  QJsonArray id_arr;
  for ( int i = 0; i < player_list_.size(); ++i ) {
    QJsonObject id_obj;
    id_obj[J_ID] = player_list_[i];
    id_arr.append( id_obj );
  }
  t_obj[J_TEAM] = id_arr;
}

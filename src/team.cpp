#include "team.h"
#include "json_common.h"
#include <qjsonarray.h>
#include <qjsonobject.h>

Team::Team( IdList const& player_list )
  : player_list_( player_list )
{
  int const n_id = player_list.size();
  if ( n_id < 1 || 3 < n_id ) {
    player_list_.resize(0);
  }
}

Team Team::readFromJson( QJsonObject const& json, QString& error_string )
{
  Team ret;
  if ( !json.contains( J_TEAM ) || !json[J_TEAM].isArray() ) {
    return ret;
  }
  QJsonArray id_arr = json[J_TEAM].toArray();
  int const n_id = id_arr.size();
  if ( n_id < 1 || n_id > 3 ) {
    error_string = tr( "Falsche Spieler-Anzahl im Team" );
    return ret;
  }
  IdList id_list;
  for ( int i = 0; i < n_id; ++i ) {
    QJsonObject id_obj = id_arr[i].toObject();
    if ( !id_obj.contains( J_ID ) ) {
      error_string = tr( "Spieler-ID in Team nicht angegeben" );
      return ret;
    }
    int id = readInt( id_obj[J_ID] );
    if ( id == INVALID_ID ) {
      error_string = tr( "Team: ungültige ID angegeben" );
      return ret;
    }
    id_list.push_back( id );
  }
  return Team( id_list );
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

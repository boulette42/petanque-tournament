#include "match.h"
#include "json_common.h"
#include <qjsonarray.h>
#include <qjsonobject.h>


Match Match::readFromJson( QJsonObject const& json, QString& error_string )
{
  Match ret;
  if ( ! json.contains( J_TEAMS ) || ! json[J_TEAMS].isArray() ) {
    error_string = tr( "Teamliste nicht vorhanden" );
    return ret;
  }
  QJsonArray team_arr = json[J_TEAMS].toArray();
  int const n_teams = team_arr.size();
  if ( n_teams != 2 ) {
    error_string = tr( "Falsche Anzahl Teams" );
    return ret;
  }
  QJsonObject team_obj = team_arr[0].toObject();
  ret.team_lt_ = Team::readFromJson( team_obj, error_string );
  if ( ! error_string.isEmpty() ) return Match();
  team_obj = team_arr[1].toObject();
  ret.team_rt_ = Team::readFromJson( team_obj, error_string );
  if ( ! error_string.isEmpty() ) return Match();
  
  if ( json.contains( J_RESULTS ) ) {
    if ( ! json[J_RESULTS].isArray() ) {
      error_string = tr( "Falsches Format der Ergebnisse" );
      return Match();
    }
    QJsonArray result_arr = json[J_RESULTS].toArray();
    int const n_results = result_arr.size();
    if ( n_results != 0 ) {
      if ( n_results != 2 ) {
        error_string = tr( "Falsche Anzahl Ergebnisse" );
        return Match();
      }
      QJsonObject result_obj = result_arr[0].toObject();
      int result_lt = readInt( result_obj[J_POINTS], INVALID_IDX );
      result_obj = result_arr[1].toObject();
      int result_rt = readInt( result_obj[J_POINTS], INVALID_IDX );
      if ( result_lt < 0 || result_lt > 13
        || result_rt < 0 || result_rt > 13
        || result_lt + result_rt > 25 )
      {
        error_string = tr( "Falsches Ergebnis" );
        return Match();
      }
      ret.result_.setPointsLeft( result_lt );
      ret.result_.setPointsRight( result_rt );
    }
  }
  if ( json.contains( J_SITE ) ) {
    int id = readInt( json[J_SITE] );
    if ( id > 0 ) {
      ret.site_id_ = id;
    }
  }
  return ret;
}

void Match::writeToJson( QJsonObject& match_obj ) const
{
  QJsonArray team_arr;
  QJsonObject team_lt_obj;
  team_lt_.writeToJson( team_lt_obj );
  team_arr.append( team_lt_obj );
  QJsonObject team_rt_obj;
  team_rt_.writeToJson( team_rt_obj );
  team_arr.append( team_rt_obj );
  match_obj[J_TEAMS] = team_arr;
  if ( ! result_.isEmpty() ) {
    QJsonArray result_arr;
    QJsonObject pts_lt_obj;
    pts_lt_obj[J_POINTS] = result_.pointsLeft();
    result_arr.append( pts_lt_obj );
    QJsonObject pts_rt_obj;
    pts_rt_obj[J_POINTS] = result_.pointsRight();
    result_arr.append( pts_rt_obj );
    match_obj[J_RESULTS] = result_arr;
  }
  if ( site_id_ != INVALID_ID ) {
    match_obj[J_SITE] = site_id_;
  }
}

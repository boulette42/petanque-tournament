#include "config.h"
#include "json_common.h"
#include <qjsonobject.h>
#include <qstring.h>
#include <qvariant.h>

QString const J_ASSOCIATION = QLatin1String( "association" );
QString const J_DATE = QLatin1String( "date" );
QString const J_FIRST_NAME = QLatin1String( "firstName" );
QString const J_ID = QLatin1String( "id" );
QString const J_LAST_NAME = QLatin1String( "lastName" );
QString const J_MATCHES = QLatin1String( "matches" );
QString const J_MODE = QLatin1String( "mode" );
QString const J_PLAYERS = QLatin1String( "players" );
QString const J_POINT_MODE = QLatin1String( "pointMode" );
QString const J_POINTS = QLatin1String( "points" );
QString const J_RESULTS = QLatin1String( "results" );
QString const J_ROUNDS = QLatin1String( "rounds" );
QString const J_SELECTED = QLatin1String( "selected" );
QString const J_SITE_NAME = QLatin1String( "siteName" );
QString const J_SITE = QLatin1String( "site" );
QString const J_SITES = QLatin1String( "sites" );
QString const J_TEAM = QLatin1String( "team" );
QString const J_TEAM_NAME = QLatin1String( "teamName" );
QString const J_TEAMS = QLatin1String( "teams" );

int readInt( QJsonValue const& vj, int default_int )
{
  QVariant v_id = vj.toVariant();
  if ( v_id.isValid() ) {
    int id = v_id.toInt();
    if ( id >= 0 ) return id;
  }
  return default_int;
}

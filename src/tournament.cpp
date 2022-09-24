#include "tournament.h"
#include "json_common.h"
#include "match.h"
#include "player.h"
#include "player_result.h"
#include "round_calculator.h"
#include "site.h"
#include "settings.h"
#include <qdatetime.h>
#include <qdebug.h>
#include <qdir.h>
#include <qfile.h>
#include <qfileinfo.h>
#include <qjsonarray.h>
#include <qjsondocument.h>
#include <qjsonobject.h>
#include <qmap.h>
#include <qregexp.h>


namespace {

QString const DATE_PATTERN = QLatin1String( "yyyy-MM-dd" );


PlayerList::iterator getPlayerIterator( PlayerList& player_list, int id )
{
  for ( PlayerList::iterator it = player_list.begin(); it != player_list.end(); ++it ) {
    if ( it->id() == id ) return it;
  }
  return player_list.end();
}

Round readRound( QJsonObject const& json, QString& error_string )
{
  Round ret;
  if ( json.contains( J_MATCHES ) ) {
    if ( ! json[J_MATCHES].isArray() ) {
      error_string = Tournament::tr( "Keine Matches definiert" );
      return ret;
    }
    QJsonArray m_arr = json[J_MATCHES].toArray();
    ret.reserve( m_arr.size() );
    for ( int m_i = 0; m_i < m_arr.size(); ++m_i ) {
      QJsonObject m_obj = m_arr[m_i].toObject();
      Match match = Match::readFromJson( m_obj, error_string );
      if ( ! error_string.isEmpty() ) break;
      ret.append( match );
    }
  }
  return ret;
}

QString globalTournamentName( QString const& tournament_name, bool to_write = false )
{
  QString filepath = tournament_name;
  QFileInfo fi( filepath );
  if ( fi.fileName() == tournament_name ) {
    QString dir = global().dataDir( to_write );
    if ( ! dir.isEmpty() && QDir( dir ).exists() ) {
      dir.replace( QLatin1Char( '\\' ), QLatin1Char( '/' ) );
      if ( ! dir.endsWith( QLatin1Char( '/' ) ) ) {
        dir += QLatin1Char( '/' );
      }
      filepath = dir + tournament_name;
    }
  }
  return filepath;
}

bool createBackup( QString const filename, QString& error_string )
{
  QFile f( filename );
  if ( f.exists() ) {
    QString backup_filename = filename + QLatin1String( ".bak" );
    if ( QFile::exists( backup_filename ) ) {
      if ( ! QFile::remove( backup_filename ) ) {
        error_string = Tournament::tr( "Datei '%1' lässt sich nicht löschen" ).arg( backup_filename );
        return false;
      }
    }
    if ( ! f.rename( backup_filename ) ) {
      error_string = Tournament::tr( "Backup von Datei '%1' lässt sich nicht erstellen (%2)" )
        .arg( filename, f.errorString() );
      return false;
    }
  }
  return true;
}

int lastPlayerId( PlayerList const& player_list )
{
  int last_player_id = INVALID_ID;
  foreach ( Player const& player, player_list ) {
    if ( player.id() > last_player_id ) {
      last_player_id = player.id();
    }
  }
  return last_player_id;
}

}


// --- Tournament ------------------------------------------------------------------

Tournament::Tournament()
  : site_cnt_( INITIAL_SITE_COUNT )
{
  site_list_.reserve( site_cnt_ );
  for ( int i = 0; i < site_cnt_; ++i ) {
    site_list_.append( Site( i + 1 ) );
  }
}

Tournament::~Tournament() = default;

int Tournament::lastRoundIdx() const
{
  return round_list_.size() - 1;
}

Round const& Tournament::round( int round_idx ) const
{
  static Round empty_round;
  return 0 <= round_idx && round_idx < round_list_.size()
    ? round_list_[round_idx]
    : empty_round;
}

bool Tournament::createRound( int round_idx, QString& error_string )
{
  int n_rounds = round_list_.size();
  if ( round_idx < n_rounds - 1 ) {
    error_string = tr( "Runden-Parameter unplausibel" );
    // Programmfehler: nicht neue Runde
    return false;
  }
  Round round;
  PlayerList player_list( selectedPlayerList() );
  if ( isTeamMode() ) {
    TeamList team_list = setTeams( player_list, error_string );
    if ( ! error_string.isEmpty() ) return false;
    if ( team_list.size() % 2 ) {
      error_string = tr( "Ungerade Anzahl von Teams" );
      return false;
    }
    round = calcRound( player_list, team_list );
  } else {
    round = calcRound( player_list );
  }
  if ( round.isEmpty() ) {
    error_string = tr( "Eine neue Runde konnte nicht erzeugt werden" );
    return false;
  }
  if ( global().siteEnabled() ) {
    round = calcSites( round, *this );
  }
  if ( round_idx == n_rounds ) {
    round_list_.append( round );
  } else {
    round_list_[round_idx] = round;
  }
  changed_ = true;
  last_round_finished_ = false;
  return true;
}

TeamList Tournament::setTeams( PlayerList const& player_list, QString& error_string )
{
  using TeamMap = QMap<QString, IdList>;
  TeamMap team_map;
  for ( int i = 0, n = player_list.size(); i < n; i++ ) {
    Player const& player( player_list[i] );
    QString team_name = player.team().toLower();
    TeamMap::iterator t_it = team_map.find( team_name );
    if ( t_it == team_map.end() ) {
      t_it = team_map.insert( team_name, IdList() );
    }
    t_it->append( i );
  }

  auto setTeam = [player_list]( IdList const& id_list ) {
    int const n = id_list.size();
    if ( n < 2 || 3 < n ) return Team();
    Team t( n == 2 ? Team::doublet() : Team::triplet() );
    for ( int i = 0; i < n; ++i ) {
      int const idx = id_list[i];
      t.setPlayerId( i, player_list[idx].id() );
    }
    return t;
  };

  TeamList ret;
  for ( TeamMap::const_iterator it = team_map.constBegin();
    it != team_map.constEnd();
    ++it )
  {
    Team t( setTeam( *it ) );
    if ( t.size() > 0 ) {
      ret.append( t );
    } else {
      if ( error_string.isEmpty() ) {
        error_string = tr( "Die Anzahl der Team-Spieler stimmt nicht:" );
      }
      error_string += tr( "\nTeam '%1' hat %2 Spieler" )
        .arg( it.key() )
        .arg( it->size() );
    }
  }
  return ret;
}

void Tournament::setRound( int round_idx, Round const& round )
{
  if ( round_idx >= round_list_.size() ) return;
  round_list_[round_idx] = round;
  foreach ( Match const& match, round ) {
    for ( int i = 0; i < match.team_lt_.size(); ++i ) {
      int id = match.team_lt_.playerId( i );
      PlayerList::iterator it = getPlayerIterator( player_list_, id );
      if ( it != player_list_.end() ) {
        it->setMatch( round_idx, match );
      }
    }
    for ( int i = 0; i < match.team_rt_.size(); ++i ) {
      int id = match.team_rt_.playerId( i );
      PlayerList::iterator it = getPlayerIterator( player_list_, id );
      if ( it != player_list_.end() ) {
        it->setMatch( round_idx, match );
      }
    }
  }
  changed_ = true;
}

bool Tournament::isTeamMode() const
{
  switch ( mode_ ) {
  case ProgMode::teams:
    return true;
  case ProgMode::undefined:
    return global().isTeamMode();
  //case ProgMode::SUPER_MELEE:
  }
  return false;
}

bool Tournament::isUndefinedMode() const
{
  return mode_ == ProgMode::undefined;
}

Player const& Tournament::player( int id ) const
{
  static Player empty;
  if ( id != INVALID_ID ) {
    for ( int i = 0, n = player_list_.size(); i < n; ++i ) {
      Player const& p = player_list_[i];
      if ( p.id() == id ) {
        return p;
      }
    }
  }
  return empty;
}

QString Tournament::playerName( int id ) const
{
  Player const& p = player( id );
  return id != INVALID_ID
    ? QStringLiteral( "%1 %2" ).arg( p.firstName(), p.lastName() )
    : QString();
}

QString Tournament::siteName( int id ) const
{
  if ( id == INVALID_ID ) return QString();
  for ( int i = 0, n = site_list_.size(); i < n; ++i ) {
    Site const& s = site_list_[i];
    if ( s.id() == id ) {
      return s.name();
    }
  }
  return tr( "Platz %1" ).arg( id );
}

bool Tournament::isChanged() const
{
  return changed_;
}

bool Tournament::addPlayer( Player const& p )
{
  int id = p.id();
  if ( id == INVALID_ID ) {
    int new_player_id = lastPlayerId( player_list_ )+1;
    player_list_.append( Player( new_player_id, p ) );
  } else {
    if ( getPlayerIterator( player_list_, id ) != player_list_.end() ) return false;
    player_list_.append( p );
  }
  changed_ = true;
  return true;
}

bool Tournament::deletePlayer( int id )
{
  for ( int i = 0; i < player_list_.size(); ++i ) {
    Player const& p = player_list_[i];
    if ( p.id() == id ) {
      player_list_.remove( i );
      return true;
    }
  }
  return false;
}

PlayerList Tournament::selectedPlayerList() const
{
  struct GreaterThanPoints {
    inline bool operator() ( Player const& lhs, Player const& rhs )
    {
      int pts_lt = lhs.result() ? lhs.result()->resultPoints() : 0;
      int pts_rt = rhs.result() ? rhs.result()->resultPoints() : 0;
      return pts_lt > pts_rt;
    }
  };

  PlayerList ret;
  int const n = player_list_.size();
  ret.reserve( n );
  for ( int i = 0; i < n; ++i ) {
    Player const& player( player_list_[i] );
    if ( player.selected() ) ret.append( player );
  }
  std::sort( ret.begin(), ret.end(), GreaterThanPoints() );
  return ret;
}

int Tournament::selectedPlayerCount() const
{
  int cnt = 0;
  foreach ( Player const& p, player_list_ ) {
    if ( p.selected() ) ++cnt;
  }
  return cnt;
}

QVector<Site> Tournament::selectedSiteList() const
{
  QVector<Site> ret;
  int const n = site_list_.size();
  ret.reserve( n );
  for ( int i = 0; i < n; ++i ) {
    Site const& site( site_list_[i] );
    if ( site.selected() ) ret.append( site );
  }
  return ret;
}

bool Tournament::setSiteCount( int site_cnt )
{
  if ( 1 > site_cnt || site_cnt > 999 ) return false;
  if ( site_cnt != site_cnt_ ) {
    int max_id = site_list_.size();
    if ( max_id < site_cnt ) {
      for ( int i = 0; i < site_cnt - max_id; ++i ) {
        site_list_.append( Site( i + 1 + max_id ) );
      }
    }
  }
  site_cnt_ = site_cnt;
  return true;
}

int Tournament::selectedSiteCount() const
{
  int cnt = 0;
  foreach ( Site const& s, site_list_ ) {
    if ( s.selected() ) ++cnt;
  }
  return cnt;
}

bool Tournament::savePlayerList( QString const& csv_name ) const
{
  struct GreaterThanPoints {
    inline bool operator() ( Player const& lhs, Player const& rhs )
    {
      int res_lt = lhs.result() ? lhs.result()->resultPoints() : 0;
      int res_rt = rhs.result() ? rhs.result()->resultPoints() : 0;
      return lhs.points() + res_lt > rhs.points() + res_rt;
    }
  };

  QFile fo( csv_name );
  if ( ! fo.open( QIODevice::WriteOnly ) ) {
    return false;
  }
  PlayerList player_list = player_list_;
  std::sort( player_list.begin(), player_list.end(), GreaterThanPoints() );
  foreach ( Player const& player, player_list ) {
    fo.write( player.toCsvLine().toLatin1() );
    fo.write( "\n" );
  }
  return true;
}

bool Tournament::loadPlayerList( QString const& csv_name )
{
  uchar const delimiter = ';';
  QFile fi( csv_name );
  if ( ! fi.open( QIODevice::ReadOnly ) ) {
   //p_error->fatal( QStringLiteral( "Datei '%1' kann nicht geladen werden" ).arg( file_path ) );
    return false;
  }
  PlayerList player_list;
  while ( ! fi.atEnd() ) {
    QByteArray line = fi.readLine();
    int len = line.length();
    while ( len > 0 ) {
      uchar uc = line[len - 1];
      if ( uc == delimiter || uc >= 32 ) break;
      --len;
    }
    if ( len > 0 ) {
      QString s_line = QString::fromLatin1( line.left( len ) );
      QString error_string;
      Player player = Player::fromCsvLine( s_line, error_string, delimiter );
      if ( error_string.isEmpty() ) {
        player_list.append( player );
      } else {
        //p_error->warning( QStringLiteral( "Zeile '%1' nicht gelesen: %2" )
        //  .arg( s_line, error_string ) );
      }
    }
  }
  if ( player_list.isEmpty() ) return false;
  int last_player_id = lastPlayerId( player_list );
  player_list_.clear();
  foreach ( Player const& player, player_list ) {
    if ( getPlayerIterator( player_list_, player.id() ) == player_list_.end() ) {
      player_list_.append( player );
    } else {
      ++last_player_id;
      player_list_.append( Player( last_player_id, player ) );
    }
  }
  round_list_.clear();
  setSiteCount( INITIAL_SITE_COUNT );
  mode_ = ProgMode::undefined;
  return true;
}

bool Tournament::loadTournament( QString const& tournament_name )
{
  QString filepath = globalTournamentName( tournament_name );
  QFile fi( filepath );
  if ( ! fi.open( QIODevice::ReadOnly ) ) {
    return false;
  }
  QByteArray tournament_data = fi.readAll();
  QJsonDocument tournament_doc( QJsonDocument::fromJson( tournament_data ) );
  QString error_string;
  if ( ! readFromJson( tournament_doc.object(), error_string ) ) {
    qWarning() << tr( "Lesen fehlgeschlagen: %1" ).arg( error_string );
    return false;
  }
  for ( int ri = 0; ri < round_list_.size(); ++ri ) {
    setRound( ri, round_list_[ri] );
  }
  return true;
}

bool Tournament::saveTournament( QString const& tournament_name, QString& error_string ) const
{
  QString file_path = globalTournamentName( tournament_name, true );
  if ( ! createBackup( file_path, error_string ) ) return false;
  QFile fo( file_path );
  if ( ! fo.open( QIODevice::WriteOnly ) ) {
    error_string = tr( "Datei '%1' lässt sich nicht speichern (%2)\n"
                       "Bítte anderes Verzeichnis in den Einstellungen wählen." )
      .arg( tournament_name, fo.errorString() );
    return false;
  }
  QJsonObject tournament_obj;
  writeToJson( tournament_obj );
  fo.write( QJsonDocument( tournament_obj ).toJson() );
  return true;
}

bool Tournament::readFromJson( QJsonObject const& json, QString& error_string )
{
  error_string.clear();
  ProgMode mode = ProgMode::undefined;
  if ( json.contains( J_MODE ) && json[J_MODE].isString() ) {
    QString s_mode = json[J_MODE].toString();
    mode = s_mode == QLatin1String( "teams" ) ? ProgMode::teams : ProgMode::super_melee;
  }
  if ( ! json.contains( J_PLAYERS ) || ! json[J_PLAYERS].isArray() ) {
    error_string = tr( "Array '%1' nicht definiert" ).arg( J_PLAYERS );
    return false;
  }
  QJsonArray p_arr = json[J_PLAYERS].toArray();
  player_list_.clear();
  player_list_.reserve( p_arr.size() );
  for ( int p_i = 0; p_i < p_arr.size(); ++p_i ) {
    QJsonObject p_obj = p_arr[p_i].toObject();
    Player player = Player::readFromJson( p_obj, error_string );
    if ( player.id() == INVALID_ID ) return false;
    player_list_.append( player );
  }
  round_list_.clear();
  if ( json.contains( J_ROUNDS ) && json[J_ROUNDS].isArray() ) {
    QJsonArray r_arr = json[J_ROUNDS].toArray();
    round_list_.reserve( r_arr.size() );
    for ( int r_i = 0; r_i < r_arr.size(); ++r_i ) {
      QJsonObject r_obj = r_arr[r_i].toObject();
      Round round = readRound( r_obj, error_string );
      if ( ! error_string.isEmpty() ) return false;
      round_list_.append( round );
    }
    if ( ! round_list_.isEmpty() || mode == ProgMode::undefined ) {
      mode_ = mode;
    }
  }
  site_list_.clear();
  if ( json.contains( J_SITES ) && json[J_SITES].isArray() ) {
    QJsonArray s_arr = json[J_SITES].toArray();
    site_list_.reserve( s_arr.size() );
    for ( int s_i = 0; s_i < s_arr.size(); ++s_i ) {
      QJsonObject s_obj = s_arr[s_i].toObject();
      Site site = Site::readFromJson( s_obj, error_string );
      if ( ! error_string.isEmpty() ) return false;
      int const id = site.id();
      int const sz = site_list_.size();
      if ( sz <= id ) {
        for ( int i = sz; i < id - 1; ++i ) {
          site_list_.append( Site( i ) );
        }
        site_list_.append( site );
      } else {
        site_list_[id - 1] = site;
      }
    }
    site_cnt_ = site_list_.size();
  }
  return true;
}

void Tournament::writeToJson( QJsonObject& json ) const
{
  json[J_DATE] = QDate::currentDate().toString( DATE_PATTERN );
  json[J_MODE] = QLatin1String( isTeamMode() ? "teams" : "supermelee" );
  QJsonArray p_arr;
  foreach ( Player const& player, player_list_ ) {
    QJsonObject p_obj;
    player.writeToJson( p_obj );
    p_arr.append( p_obj );
  }
  json[J_PLAYERS] = p_arr;

  QJsonArray r_arr;
  foreach ( Round const& round, round_list_ ) {
    QJsonArray m_arr;
    foreach ( Match const& match, round ) {
      QJsonObject match_obj;
      match.writeToJson( match_obj );
      m_arr.append( match_obj );
    }
    QJsonObject r_obj;
    r_obj[J_MATCHES] = m_arr;
    r_arr.append( r_obj );
  }
  json[J_ROUNDS] = r_arr;

  QJsonArray s_arr;
  for ( int i = 0; i < site_cnt_; ++i ) {
    Site const& site( site_list_[i] );
    QJsonObject s_obj;
    site.writeToJson( s_obj );
    s_arr.append( s_obj );
  }
  json[J_SITES] = s_arr;
}

Round readRoundFile( QString const& filepath, QString& error_string )
{
  QFile fi( filepath );
  if ( ! fi.open( QIODevice::ReadOnly ) ) {
    error_string = Tournament::tr( "Kann Datei '%1' nicht lesen" ).arg( filepath );
    return Round();
  }
  QByteArray round_data = fi.readAll();
  QJsonDocument round_doc( QJsonDocument::fromJson( round_data ) );
  Round round = readRound( round_doc.object(), error_string );
  return round;
}

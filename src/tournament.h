#ifndef TOURNAMENT_H
#define TOURNAMENT_H

#include "config.h"

class PlayerModel;
class RoundCalculator;
class SiteModel;
class QJsonObject;

class Tournament
{
  Q_DECLARE_TR_FUNCTIONS(Tournament)

  friend PlayerModel;
  friend SiteModel;

  int const INITIAL_SITE_COUNT = 9;

  PlayerList player_list_;
  QVector<Round> round_list_;
  QVector<Site> site_list_;
  int site_cnt_ = 0;
  bool last_round_finished_ = false;
  bool changed_ = false;
  ProgMode mode_ = ProgMode::undefined;

public:
  Tournament();
  ~Tournament();

  bool isChanged() const;

  int lastRoundIdx() const;
  Round const& round( int round_idx ) const;
  bool createRound( RoundCalculator& round_calculator );
  void setRound( int round_idx, Round const& round );
  bool lastRoundFinished() const { return last_round_finished_; }
  void setLastRoundFinished( bool last_round_finished ) { last_round_finished_ = last_round_finished; }

  bool isTeamMode() const;
  bool isUndefinedMode() const;
  Player const& player( int id ) const;
  QString playerName( int id ) const;
  bool addPlayer( Player const& player );
  bool deletePlayer( int id );
  PlayerList const& playerList() const { return player_list_; }
  PlayerList selectedPlayerList() const;
  int selectedPlayerCount() const;

  QString siteName( int id ) const;
  QVector<Site> const& siteList() const { return site_list_; }
  QVector<Site> selectedSiteList() const;
  int siteCount() const { return site_cnt_; }
  bool setSiteCount( int site_cnt );
  int selectedSiteCount() const;
  int neededSites() const;

  static TeamList setTeams( PlayerList const& player_list, QString& error_string );

  bool loadTournament( QString const& tournament_name );
  bool saveTournament( QString const& tournament_name, QString& error_string ) const;
  bool loadPlayerList( QString const& csv_name );
  bool savePlayerList( QString const& csv_name ) const;

private:
  bool readFromJson( QJsonObject const& json, QString& error_string );
  void writeToJson( QJsonObject& json ) const;
};

#endif  //#define TOURNAMENT_H


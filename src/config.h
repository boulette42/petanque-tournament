#ifndef CONFIG_H
#define CONFIG_H

#include <qcoreapplication.h>
#include <qmap.h>
#include <qpointer.h>
#include <qstring.h>
#include <qvector.h>

struct Match;
class Player;
class PlayerResult;
class Site;
class Team;
class Tournament;

using IdList = QVector<int>;
using IdxList = QVector<int>;
using Round = QVector<Match>;
using PlayerList = QVector<Player>;
using SiteList = QVector<Site>;
using TeamList = QVector<Team>;
using TeamMap = QMap<QString, IdList>;

int const INVALID_ID = -1;
int const INVALID_IDX = -1;


enum class ProgMode {
  super_melee,
  teams,
  tete
};

ProgMode toProgMode( QString const& s );
QString toString( ProgMode point_mode );

enum class PointMode {
  formule_x,
  swiss_simple,
  swiss_buchholz
};

PointMode toPointMode( QString const& s );
QString toString( PointMode point_mode );

#endif  // CONFIG_H

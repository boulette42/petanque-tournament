#ifndef CONFIG_H
#define CONFIG_H

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
using TeamList = QVector<Team>;

int const INVALID_ID = -1;
int const INVALID_IDX = -1;


enum class ProgMode {
  undefined,
  super_melee,
  teams
};

extern QString tr( char const* );

#endif  // CONFIG_H

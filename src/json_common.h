#ifndef JSON_COMMON_H
#define JSON_COMMON_H

#include "config.h"

class QJsonValue;

extern QString const J_ASSOCIATION;
extern QString const J_DATE;
extern QString const J_FIRST_NAME;
extern QString const J_ID;
extern QString const J_LAST_NAME;
extern QString const J_MATCHES;
extern QString const J_MODE;
extern QString const J_PLAYERS;
extern QString const J_POINTS;
extern QString const J_POINT_MODE;
extern QString const J_RESULTS;
extern QString const J_ROUNDS;
extern QString const J_SELECTED;
extern QString const J_SITE_NAME;
extern QString const J_SITE;
extern QString const J_SITES;
extern QString const J_TEAM;
extern QString const J_TEAM_NAME;
extern QString const J_TEAMS;


int readInt( QJsonValue const& vj, int default_int = INVALID_ID );

#endif  // JSON_COMMON_H

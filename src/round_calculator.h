#ifndef ROUND_CALCULATOR_H
#define ROUND_CALCULATOR_H

#include "config.h"

Round calcRound( PlayerList const& player_list );
Round calcRound( PlayerList const& player_list, TeamList const& team_list );
Round calcSites( Round const& round, Tournament const& tournament );

#endif  // ROUND_CALCULATOR_H

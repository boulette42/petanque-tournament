#ifndef TOURNAMENT_SIMULATOR_H
#define TOURNAMENT_SIMULATOR_H

#include "config.h"
class QWidget;

class TournamentSimulator
{
  Q_DECLARE_TR_FUNCTIONS(TournamentSimulator)

  Tournament& tournament_;

public:
  explicit TournamentSimulator( Tournament& tournament );
  ~TournamentSimulator();

  PlayerList createRandomPlayers( QWidget* parent ) const;
  static Round createRandomResults( Round const& round );
};

#endif  // TOURNAMENT_SIMULATOR_H

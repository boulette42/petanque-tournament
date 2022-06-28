#ifndef SETTINGS_H
#define SETTINGS_H

#include "config.h"

struct SettingsCore;
class QWidget;

class Settings
{
  QSharedPointer<SettingsCore> m_;

public:
  Settings();
  ~Settings();

  bool execDialog( QWidget* parent, bool enable_mode );
  QString dataDir( bool for_write = false ) const;
  int fontSize() const;
  bool isTeamMode() const;
  bool siteEnabled() const;
  bool simulationEnabled() const;
};


Settings& global();

void updateStyleSheet( QWidget& dlg );
void updateStyleSheet( QWidget& dlg, int font_size );

#endif  // SETTINGS_H

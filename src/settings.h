#ifndef SETTINGS_H
#define SETTINGS_H

#include "config.h"

class QWidget;

struct Settings
{
  QString data_dir_;
  ProgMode mode_ = ProgMode::super_melee;
  int font_size_ = 0;
  bool site_enabled_ = true;
  bool simulation_enabled_ = false;
};


Settings const& global();
bool settingsDialog( QWidget* parent, bool enable_mode );
void updateStyleSheet( QWidget& dlg );

#endif  // SETTINGS_H

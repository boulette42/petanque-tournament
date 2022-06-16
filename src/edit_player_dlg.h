#ifndef EDIT_PLAYER_DLG_H
#define EDIT_PLAYER_DLG_H

#include "config.h"

class QDialog;
class QWidget;
class Ui_EditPlayerDialog;

class EditPlayerDlg
{
  QPointer<QDialog> dlg_;
  QSharedPointer<Ui_EditPlayerDialog> ui_;
  QSharedPointer<Player> player_;

public:
  explicit EditPlayerDlg( QWidget* parent );
  ~EditPlayerDlg();

  bool exec( Player const& player, bool team_mode );
  Player const& player() const;
};

#endif  // EDIT_PLAYER_DLG_H

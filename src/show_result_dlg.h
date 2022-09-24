#ifndef SHOW_RESULT_DLG_H
#define SHOW_RESULT_DLG_H

#include "config.h"

class QAbstractItemModel;
class QDialog;
class QWidget;
class Ui_ShowPlayerResultDialog;


class ShowResultDlg
{
  Q_DECLARE_TR_FUNCTIONS(Tournament)

  QPointer<QDialog> dlg_;
  QSharedPointer<Ui_ShowPlayerResultDialog> ui_;
  QPointer<QAbstractItemModel> match_model_;

public:
  explicit ShowResultDlg( QWidget* parent );
  ~ShowResultDlg();

  void exec( Tournament const& tournament, Player const& player );
  void exec( Tournament const& tournament, Team const& team );
};

#endif  // SHOW_PLAYER_RESULT_DLG_H

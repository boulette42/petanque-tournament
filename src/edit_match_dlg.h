#ifndef EDIT_MATCH_DLG_H
#define EDIT_MATCH_DLG_H

#include "config.h"

class QDialog;
class QWidget;
class Ui_EditMatchDialog;

class EditMatchDlg
{
  QPointer<QDialog> dlg_;
  QSharedPointer<Ui_EditMatchDialog> ui_;

public:
  explicit EditMatchDlg( QWidget* parent );
  ~EditMatchDlg();

  bool exec( Match const& match, int mno, Tournament const& tournament );
  int pointsLeft() const;
  int pointsRight() const;
};

#endif  // EDIT_MATCH_DLG_H

#ifndef SELECT_WINDOW_DLG_H
#define SELECT_WINDOW_DLG_H

#include "config.h"

class QDialog;
class QWidget;
class Ui_SelectWindowDialog;

class SelectWindowDlg
{
  QPointer<QDialog> dlg_;
  QSharedPointer<Ui_SelectWindowDialog> ui_;

public:
  explicit SelectWindowDlg( QWidget* parent );
  ~SelectWindowDlg();

  bool exec();
  int screen() const;
  int fontSize() const;
  bool isRoundWindow() const;
  bool isResultWindow() const;
  bool isPlayerWindow() const;
};

#endif  // SELECT_WINDOW_DLG_H

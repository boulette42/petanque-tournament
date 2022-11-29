#ifndef ABOUT_DLG_H
#define ABOUT_DLG_H

#include "config.h"

class QDialog;
class QWidget;
class Ui_AboutDialog;

class AboutDlg
{
  Q_DECLARE_TR_FUNCTIONS(AboutDlg)

  QPointer<QDialog> dlg_;
  QSharedPointer<Ui_AboutDialog> ui_;

public:
  explicit AboutDlg( QWidget* parent );
  ~AboutDlg();

  void exec();

private:
  void showLicenses();
};

#endif  // ABOUT_DLG_H

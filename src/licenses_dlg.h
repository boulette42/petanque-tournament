#ifndef LICENSES_DLG_H
#define LICENSES_DLG_H

#include "config.h"

class QDialog;
class QWidget;
class Ui_LicensesDialog;

class LicensesDlg
{
  Q_DECLARE_TR_FUNCTIONS(LicensesDlg)

  QPointer<QDialog> dlg_;
  QSharedPointer<Ui_LicensesDialog> ui_;

public:
  explicit LicensesDlg( QWidget* parent );
  ~LicensesDlg();

  void exec();
  static bool licenseFileExists();
  static QString loadLicenses();
  static QString licenseFileName();
};

#endif

#include "about_dlg.h"
#include <ui_about_dlg.h>
#include "licenses_dlg.h"
#include "version.h"
#include <qdialog.h>


AboutDlg::AboutDlg( QWidget* parent )
  : dlg_( new QDialog( parent ) )
  , ui_( new Ui_AboutDialog() )
{
  ui_->setupUi( dlg_ );
  ui_->txVersion->setText( QStringLiteral( MY_VERSION_STRING ) );
  ui_->txAuthor->setText( QStringLiteral( MY_COMPANY_NAME ) );
  ui_->txYear->setText( QStringLiteral( MY_YEAR ) );
  QObject::connect( ui_->pbOk, &QPushButton::clicked,
    dlg_, &QDialog::accept );
  if ( !LicensesDlg::licenseFileExists() ) {
    ui_->pbLicenses->hide();
  } else {
    QObject::connect( ui_->pbLicenses, &QToolButton::clicked,
      [this]() { showLicenses(); } );
  }
}

AboutDlg::~AboutDlg() = default;

void AboutDlg::exec()
{
  dlg_->exec();
}

void AboutDlg::showLicenses()
{
  LicensesDlg( dlg_ ).exec();
}

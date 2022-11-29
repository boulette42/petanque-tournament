#include "licenses_dlg.h"
#include <ui_licenses_dlg.h>
#include <qdialog.h>
#include <qfile.h>
#include <qtextcodec.h>


LicensesDlg::LicensesDlg( QWidget* parent )
  : dlg_( new QDialog( parent ) )
  , ui_( new Ui_LicensesDialog() )
{
  ui_->setupUi( dlg_ );
  ui_->teLicenses->viewport()->setBackgroundRole( QPalette::Window );
  ui_->teLicenses->viewport()->setForegroundRole( QPalette::WindowText );
  dlg_->setModal( true );
}

LicensesDlg::~LicensesDlg() = default;

void LicensesDlg::exec()
{
  QString html_text = loadLicenses();
  if ( html_text.isEmpty() ) return;
  ui_->teLicenses->setHtml( html_text );
  QObject::connect( ui_->pbOk, &QPushButton::clicked,
    dlg_, &QDialog::accept );
  dlg_->exec();
}

bool LicensesDlg::licenseFileExists()
{
  return ! licenseFileName().isEmpty();
}

QString LicensesDlg::loadLicenses()
{
  QFile f( licenseFileName() );
  if ( !f.open( QIODevice::ReadOnly ) ) return QString();
  QByteArray ba = f.readAll();
  QTextCodec* c = QTextCodec::codecForHtml( ba );
  return c->toUnicode( ba );
}

QString LicensesDlg::licenseFileName()
{
  QString licence_path = QCoreApplication::applicationDirPath();
  QString fn = licence_path + QStringLiteral( "/lizenzen.html" );
  return QFile::exists( fn ) ? fn : QString();
}

#include "settings.h"
#include "ui_settings_dlg.h"
#include <qfiledialog.h>
#include <qmessagebox.h>
#include <qsettings.h>
#include <qvalidator.h>


namespace {

QString const company( QStringLiteral( "Boulette 42" ) );
QString const app_name( QStringLiteral( "Supermelée" ) );
QString const v_mode( QStringLiteral( "Modus" ) );
QString const v_mode_s( QStringLiteral( "Supermelee" ) );
QString const v_mode_t( QStringLiteral( "Teams" ) );
QString const v_data_dir( QStringLiteral( "DataDir" ) );
QString const v_font_size( QStringLiteral( "FontSize" ) );
QString const v_site_enabled( QStringLiteral( "SiteEnabled" ) );

const int MIN_FONT_SIZE = 7;
const int MAX_FONT_SIZE = 24;


class ZoomValidator : public QValidator
{
public:
  ZoomValidator( QObject* parent ) : QValidator( parent ) { }

  QValidator::State validate( QString& input, int& /*pos*/ ) const override
  {
    if ( ! input.isEmpty() ) {
      bool ok = false;
      int i = input.toInt( &ok );
      if ( ! ok ) return QValidator::Invalid;
      if ( i != 0 && i > MAX_FONT_SIZE ) return QValidator::Invalid;
    }
    return QValidator::Acceptable;
  }
};


Settings loadSettings()
{
  Settings settings;
  QSettings s( company, app_name );
  QString mode = s.value( v_mode ).toString();
  settings.mode_ = mode.compare( v_mode_t, Qt::CaseInsensitive ) == 0
    ? ProgMode::teams
    : ProgMode::super_melee;
  settings.data_dir_ = s.value( v_data_dir ).toString();
  settings.site_enabled_ = s.value( v_site_enabled ).toBool();
  settings.font_size_ = s.value( v_font_size ).toInt();
  return settings;
}

void saveSettings( Settings const& settings )
{
  QSettings s( company, app_name );
  s.setValue( v_mode, settings.mode_ == ProgMode::teams ? v_mode_t : v_mode_s );
  s.setValue( v_data_dir, settings.data_dir_ );
  s.setValue( v_site_enabled, settings.site_enabled_ );
  s.setValue( v_font_size,settings. font_size_ );
}

Settings& settings()
{
  static Settings s( loadSettings() );
  return s;
}


class SettingsDlg : public QObject
{
  Settings& settings_;
  QPointer<QDialog> dlg_;
  QSharedPointer<Ui_SettingsDialog> ui_;

public:
  SettingsDlg( QWidget* parent )
    : settings_( settings() )
    , dlg_( new QDialog( parent ) )
    , ui_( new Ui_SettingsDialog )
  {
    ui_->setupUi( dlg_ );
    updateStyleSheet( *dlg_ );
    QFontMetrics fm( ui_->leFontSize->font() );
    ui_->leFontSize->setMaximumWidth( fm.horizontalAdvance( QLatin1String( "5555" ) ) );
    connect( ui_->tbSelectDir, &QToolButton::clicked, this, &SettingsDlg::selectDir );
    connect( ui_->buttonBox, &QDialogButtonBox::accepted, this, &SettingsDlg::validateAccept );
  }

  ~SettingsDlg() = default;

  bool exec( bool enable_mode )
  {
    ui_->gbMode->setVisible( enable_mode );
    ui_->rbSuperMelee->setChecked( settings_.mode_ == ProgMode::super_melee );
    ui_->rbTeams->setChecked( settings_.mode_ == ProgMode::teams  );
    ui_->leDataDir->setText( settings_.data_dir_ );
    ui_->cbSiteEnabled->setChecked( settings_.site_enabled_ );
    ui_->leFontSize->setText( QString::number( settings_.font_size_ ) );
    if ( ! dlg_->exec() ) return false;
    settings_.data_dir_ = ui_->leDataDir->text();
    settings_.mode_ = ui_->rbSuperMelee->isChecked() ? ProgMode::super_melee : ProgMode::teams;
    settings_.site_enabled_ = ui_->cbSiteEnabled->isChecked();
    int font_size = ui_->leFontSize->text().toInt();
    if ( font_size >= MIN_FONT_SIZE && font_size <= MAX_FONT_SIZE ) {
      settings_.font_size_ = font_size;
    } else {
      settings_.font_size_ = 0;
    }
    saveSettings( settings_ );
    return true;
  }

  void selectDir()
  {
    QString dir = QFileDialog::getExistingDirectory( dlg_, tr( "Verzeichnis auswählen" ), settings_.data_dir_ );
    if ( ! dir.isEmpty() ) {
      ui_->leDataDir->setText( dir );
    }
  }

  void validateAccept()
  {
    QString const dir( ui_->leDataDir->text() );
    if ( dir.isEmpty() || QDir( dir ).exists() ) {
      dlg_->accept();
      return;
    }
    QMessageBox::warning( dlg_, tr( "Verzeichnis-Warnung" ), tr( "Das Verzeichnis ist nicht vorhanden" ) );
  }
};

}


extern Settings const& global()
{
  return settings();
}

bool settingsDialog( QWidget* parent, bool enable_mode )
{
  SettingsDlg dlg( parent );
  return dlg.exec( enable_mode );
}

void activateSimulator()
{
  settings().simulation_enabled_ = true;
}

void updateStyleSheet(QWidget& dlg)
{
  updateStyleSheet( dlg, global().font_size_ );
}

void updateStyleSheet( QWidget& dlg, int font_size )
{
  int const initial_font_size = 9;
  if ( font_size > 0 ) {
    QString const style_sheet = QStringLiteral( "QWidget { font-size: %1pt }" )
      .arg( font_size );
    dlg.setStyleSheet( style_sheet );
  } else {
    QString const style_sheet = QStringLiteral(
      "QHeaderView { font-size: %1pt };\n"
      "QLabel { font-size: %1pt };\n"
      "QLineEdit { font-size: %1pt };\n"
      "QPushButton { font-size: %1pt };\n" )
      .arg(initial_font_size );
    dlg.setStyleSheet( style_sheet );
  }
  dlg.ensurePolished();
  dlg.update();
}

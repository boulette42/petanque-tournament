﻿#include "settings.h"
#include "ui_settings_dlg.h"
#include "version.h"
#include <qdir.h>
#include <qfiledialog.h>
#include <qmessagebox.h>
#include <qsettings.h>
#include <qstandardpaths.h>
#include <qvalidator.h>


namespace {

  QString const company( QStringLiteral( MY_COMPANY_NAME ) );
  QString const app_name( QStringLiteral( MY_PRODUCT_NAME ) );
  QString const v_prog_mode( QStringLiteral( "Modus" ) );
  QString const v_prog_mode_s( QStringLiteral( "Supermelee" ) );
  QString const v_prog_mode_t( QStringLiteral( "Teams" ) );
  QString const v_prog_mode_1( QStringLiteral( "Tete" ) );
  QString const v_point_mode( QStringLiteral( "Punktmodus" ) );
  QString const v_point_mode_f( QStringLiteral( "FormuleX" ) );
  QString const v_point_mode_s( QStringLiteral( "SwissSimple" ) );
  QString const v_point_mode_b( QStringLiteral( "SwissBuchholz" ) );
  QString const v_team_only( QStringLiteral( "TeamOnly" ) );
  QString const v_data_dir( QStringLiteral( "DataDir" ) );
  QString const v_font_size( QStringLiteral( "FontSize" ) );
  QString const v_site_enabled( QStringLiteral( "SiteEnabled" ) );
  QString const v_site_count( QStringLiteral( "SiteCount" ) );
  QString const v_password( QStringLiteral( "Password" ) );

  const int MIN_FONT_SIZE = 7;
  const int MAX_FONT_SIZE = 24;


  QString defaultDataDir()
  {
    return QStandardPaths::writableLocation( QStandardPaths::HomeLocation ) + QStringLiteral("/.petu");
  }

}


ProgMode toProgMode( QString const& s )
{
  if ( ! s.compare( v_prog_mode_t, Qt::CaseInsensitive ) ) return ProgMode::teams;
  if ( ! s.compare( v_prog_mode_1, Qt::CaseInsensitive ) ) return ProgMode::tete;
  //if ( ! s.compare( v_prog_mode_s, Qt::CaseInsensitive ) ) return ProgMode::super_melee;
  // default
  return ProgMode::super_melee;
}

QString toString( ProgMode prog_mode_ )
{
  switch ( prog_mode_ ) {
  case ProgMode::super_melee: return v_prog_mode_s;
  case ProgMode::teams: return v_prog_mode_t;
  case ProgMode::tete: return v_prog_mode_1;
  }
  return QString();
}

PointMode toPointMode( QString const& s )
{
  if ( ! s.compare( v_point_mode_s, Qt::CaseInsensitive ) ) return PointMode::swiss_simple;
  if ( ! s.compare( v_point_mode_b, Qt::CaseInsensitive ) ) return PointMode::swiss_buchholz;
  //if ( ! s.compare( v_point_mode_f, Qt::CaseInsensitive ) ) return PointMode::formule_x;
  // default
  return PointMode::formule_x;
}

QString toString( PointMode point_mode )
{
  switch ( point_mode ) {
  case PointMode::formule_x: return v_point_mode_f;
  case PointMode::swiss_simple: return v_point_mode_s;
  case PointMode::swiss_buchholz: return v_point_mode_b;
  }
  return QString();
}

struct SettingsCore
{
  QString data_dir_;
  ProgMode prog_mode_{ ProgMode::super_melee };
  PointMode point_mode_{ PointMode::formule_x };
  int font_size_{ 0 };
  int site_count_{ 9 };
  QString password_;
  bool site_enabled_{ true };
  bool team_only_{ false };
  bool simulation_enabled_{ false };

  void load()
  {
    QSettings s( company, app_name );
    prog_mode_ = toProgMode( s.value( v_prog_mode ).toString() );
    point_mode_ = toPointMode( s.value( v_point_mode ).toString() );
    team_only_ = s.value( v_team_only ).toBool();
    data_dir_ = s.value( v_data_dir ).toString();
    site_enabled_ = s.value( v_site_enabled ).toBool();
    site_count_ = s.value( v_site_count ).toInt();
    if ( site_count_ < 1 ) site_count_ = 9;
    font_size_ = s.value( v_font_size ).toInt();
    password_ = s.value(v_password).toString();
    simulation_enabled_ = false;
    QStringList const args = QCoreApplication::arguments();
    for (int i = 1; i < args.count(); ++i ) {
      if ( args.at( i ) == QStringLiteral( "-simulate" ) ) {
        simulation_enabled_ = true;
        break;
      }
    }
  }

  void save()
  {
    QSettings s( company, app_name );
    s.setValue( v_prog_mode, toString( prog_mode_ ) );
    s.setValue( v_point_mode, toString( point_mode_ ) );
    s.setValue( v_team_only, team_only_ );
    s.setValue( v_data_dir, data_dir_ );
    s.setValue( v_site_enabled, site_enabled_ );
    s.setValue( v_site_count, site_count_ );
    s.setValue( v_font_size, font_size_ );
    s.setValue( v_password, password_ );
  }
};


namespace {

class SettingsDlg : public QObject
{
  Q_DECLARE_TR_FUNCTIONS(EditPlayerDlg)

  QPointer<QDialog> dlg_;
  QSharedPointer<Ui_SettingsDialog> ui_;
  PointMode prev_point_mode_ = PointMode::formule_x;

public:
  SettingsDlg( QWidget* parent )
    : dlg_( new QDialog( parent ) )
    , ui_( new Ui_SettingsDialog )
  {
    ui_->setupUi( dlg_ );
    updateStyleSheet( *dlg_ );
    QFontMetrics fm( ui_->leFontSize->font() );
    int const w = fm.horizontalAdvance( QLatin1String( "5" ) );
    ui_->leFontSize->setMaximumWidth( 4*w );
    ui_->lePassword->setMaximumWidth( 30*w );
    ui_->lePasswordCopy->setMaximumWidth( 30*w );
    ui_->leSiteCount->setMaximumWidth( 4*w );
    ui_->leSiteCount->setValidator( new QIntValidator( 1, 999, this ) );
    connect( ui_->tbSelectDir, &QToolButton::clicked, this, &SettingsDlg::selectDir );
    connect( ui_->buttonBox, &QDialogButtonBox::accepted, this, &SettingsDlg::validateAccept );
    connect( ui_->rbTeams, &QRadioButton::toggled, this, &SettingsDlg::updateView );
    connect( ui_->cbSiteEnabled, &QCheckBox::toggled, this, &SettingsDlg::updateView );
    connect( ui_->rbSuperMelee, &QRadioButton::toggled, this, &SettingsDlg::updatePointMode );
  }

  ~SettingsDlg() = default;

  bool exec( bool enable_mode, SettingsCore& settings )
  {
    ui_->gbMode->setEnabled( enable_mode );
    ProgMode const prog_mode = settings.prog_mode_;
    ui_->rbSuperMelee->setChecked( prog_mode == ProgMode::super_melee );
    ui_->rbTeams->setChecked( prog_mode == ProgMode::teams );
    ui_->rbTete->setChecked( prog_mode == ProgMode::tete );
    PointMode const point_mode = prev_point_mode_ = settings.point_mode_;
    bool const formule_x = point_mode == PointMode::formule_x || settings.prog_mode_ == ProgMode::super_melee;
    ui_->rbFormuleX->setChecked( formule_x );
    ui_->rbSwissSimple->setChecked( !formule_x && point_mode == PointMode::swiss_simple );
    ui_->rbSwissBuchholz->setChecked( !formule_x && point_mode == PointMode::swiss_buchholz );
    ui_->leDataDir->setText( settings.data_dir_.isEmpty() ? defaultDataDir() : settings.data_dir_ );
    ui_->cbSiteEnabled->setChecked( settings.site_enabled_ );
    ui_->leSiteCount->setText( QString::number( settings.site_count_ ) );
    ui_->cbTeamsOnly->setChecked( settings.team_only_ );
    ui_->leFontSize->setText( QString::number( settings.font_size_ ) );
    ui_->lePassword->setText( settings.password_ );
    ui_->lePasswordCopy->setText( settings.password_ );
    if ( ! dlg_->exec() ) return false;
    settings.data_dir_ = ui_->leDataDir->text();
    settings.prog_mode_ = ui_->rbSuperMelee->isChecked()
      ? ProgMode::super_melee
      :  ui_->rbTeams->isChecked()
         ? ProgMode::teams
         : ProgMode::tete;
    if ( settings.prog_mode_ == ProgMode::super_melee || ui_->rbFormuleX->isChecked() ) {
      settings.point_mode_ = PointMode::formule_x;
    }
    else {
      if ( ui_->rbSwissSimple->isChecked() ) {
        settings.point_mode_ = PointMode::swiss_simple;
      }
      else {
        settings.point_mode_ = PointMode::swiss_buchholz;
      }
    }
    settings.site_enabled_ = ui_->cbSiteEnabled->isChecked();
    settings.site_count_ = ui_->leSiteCount->text().toInt();
    settings.team_only_ = ui_->cbTeamsOnly->isChecked();
    int font_size = ui_->leFontSize->text().toInt();
    if ( font_size >= MIN_FONT_SIZE && font_size <= MAX_FONT_SIZE ) {
      settings.font_size_ = font_size;
    } else {
      settings.font_size_ = 0;
    }
    settings.password_ = ui_->lePassword->text();
    settings.save();
    return true;
  }

  void selectDir()
  {
    QString dir = QFileDialog::getExistingDirectory( dlg_, tr( "Verzeichnis auswählen" ), ui_->leDataDir->text() );
    if ( ! dir.isEmpty() ) {
      ui_->leDataDir->setText( dir );
    }
  }

  void validateAccept()
  {
    if ( ui_->lePassword->text() != ui_->lePasswordCopy->text() ) {
      QMessageBox::warning( dlg_, tr( "Passwort-Warnung" ), tr( "Die Passwörter stimmen nicht überein" ) );
      return;
    }
    QString const dir( ui_->leDataDir->text() );
    if ( dir == defaultDataDir() && !QDir( dir ).exists() ) {
      QDir( dir ).mkdir( dir );
    }
    if ( dir.isEmpty() || QDir( dir ).exists() ) {
      dlg_->accept();
      return;
    }
    QMessageBox::warning( dlg_, tr( "Verzeichnis-Warnung" ), tr( "Das Verzeichnis ist nicht vorhanden" ) );
  }

  void updateView()
  {
    ui_->leSiteCount->setEnabled( ui_->cbSiteEnabled->isChecked() );
    ui_->cbTeamsOnly->setEnabled( ui_->rbTeams->isChecked() );
  }

  void updatePointMode()
  {
    if ( ui_->rbSuperMelee->isChecked() ) {
      if ( ui_->rbFormuleX->isChecked() ) {
        prev_point_mode_ = PointMode::formule_x;
      }
      if ( ui_->rbSwissSimple->isChecked() ) {
        prev_point_mode_ = PointMode::swiss_simple;
      }
      if ( ui_->rbSwissBuchholz->isChecked() ) {
        prev_point_mode_ = PointMode::swiss_buchholz;
      }
      ui_->rbFormuleX->setChecked( true );
      ui_->rbSwissSimple->setChecked( false );
      ui_->rbSwissBuchholz->setChecked( false );
      ui_->gbPoints->setEnabled( false );
    }
    else {
      ui_->rbFormuleX->setChecked( prev_point_mode_ == PointMode::formule_x );
      ui_->rbSwissSimple->setChecked( prev_point_mode_ == PointMode::swiss_simple );
      ui_->rbSwissBuchholz->setChecked( prev_point_mode_ == PointMode::swiss_buchholz );
      ui_->gbPoints->setEnabled( true );
    }
  }
};

}


Settings& global()
{
  static Settings settings;
  return settings;
}


Settings::Settings()
  : m_( new SettingsCore )
{
  m_->load();
}

Settings::~Settings() = default;

bool Settings::execDialog(QWidget* parent, bool enable_mode)
{
  SettingsDlg dlg( parent );
  return dlg.exec( enable_mode, *m_ );
}

bool Settings::isTeamMode() const
{
  return m_->prog_mode_ != ProgMode::super_melee;
}

bool Settings::isTeteMode() const
{
  return m_->prog_mode_ == ProgMode::tete;
}

bool Settings::isFormuleX() const
{
  return m_->prog_mode_ == ProgMode::super_melee
      || m_->point_mode_ == PointMode::formule_x;
}

bool Settings::isSwissSimple() const
{
  return m_->point_mode_ == PointMode::swiss_simple;
}

QString Settings::dataDir( bool to_write ) const
{
  QString const default_dir = defaultDataDir();
  if ( m_->data_dir_.isEmpty() || m_->data_dir_ == default_dir ) {
    if ( to_write ) {
      QDir().mkpath( default_dir );
    }
    return default_dir;
  }
  return m_->data_dir_;
}

int Settings::fontSize() const
{
  return m_->font_size_;
}

bool Settings::siteEnabled() const
{
  return m_->site_enabled_;
}

int Settings::siteCount() const
{
  return m_->site_count_;
}

bool Settings::isTeamOnlyShown() const
{
  return m_->team_only_;
}

bool Settings::simulationEnabled() const
{
  return m_->simulation_enabled_;
}

QString Settings::password() const
{
  return m_->password_;
}

void Settings::updateModes( ProgMode prog_mode, PointMode point_mode )
{
  m_->prog_mode_ = prog_mode;
  m_->point_mode_ = point_mode;
}

void updateStyleSheet(QWidget& dlg)
{
  updateStyleSheet( dlg, global().fontSize() );
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
      "QHeaderView { font-size: %1pt }\n"
      "QLabel { font-size: %1pt }\n"
      "QLineEdit { font-size: %1pt }\n"
      "QPushButton { font-size: %1pt }\n" )
      .arg(initial_font_size );
    dlg.setStyleSheet( style_sheet );
  }
  dlg.ensurePolished();
  dlg.update();
}

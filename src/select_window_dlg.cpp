#include "select_window_dlg.h"
#include "ui_select_window_dlg.h"
#include "settings.h"
#include <qdialog.h>
#include <qvalidator.h>

namespace {

const int MIN_FONT_SIZE = 7;
const int MAX_FONT_SIZE = 24;

class FontSizeValidator : public QValidator
{
public:
  FontSizeValidator( QObject* parent ) : QValidator( parent ) { }

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

}


SelectWindowDlg::SelectWindowDlg( QWidget* parent )
  : dlg_( new QDialog( parent ) )
  , ui_( new Ui_SelectWindowDialog() )
{
  ui_->setupUi( dlg_ );
  updateStyleSheet( *dlg_ );
  QFontMetrics fm( ui_->leFontSize->font() );
  ui_->leFontSize->setMaximumWidth( fm.horizontalAdvance( QLatin1String( "5555" ) ) );
  ui_->leFontSize->setValidator( new FontSizeValidator( dlg_ ) );
}

SelectWindowDlg::~SelectWindowDlg() = default;

bool SelectWindowDlg::exec()
{
  ui_->rbRound->setChecked(true);
  return dlg_->exec();
}

int SelectWindowDlg::fontSize() const
{
  int fs = ui_->leFontSize->text().toInt();
  return MIN_FONT_SIZE <= fs && fs <= MAX_FONT_SIZE ? fs : 0;
}

bool SelectWindowDlg::isRoundWindow() const
{
  return ui_->rbRound->isChecked();
}

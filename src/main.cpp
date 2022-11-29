#include "main_window.h"
#include "tournament.h"
#include <qapplication.h>
#include <qmessagebox.h>
#include <qtranslator.h>


namespace {

QString getArg( char c )
{
  QString ret;
  QStringList const args = QCoreApplication::arguments();
  int i = 1;
  while ( i < args.count() ) {
    if ( c == 0 ) return args[i];
    QString const& arg = args[i];
    QByteArray b_arg = arg.toLocal8Bit();
    if ( b_arg[1] == c && ( b_arg[0] == '-' || b_arg[0] == '/' ) ) {
      if ( arg.length() == 2 ) {
        ++i;
        if ( i < args.count() ) {
          ret = args[i];
        }
      } else if ( arg.length() > 2 ) {
        int p = 2;
        if ( b_arg.at( p ) == '=' ) {
          ++p;
        }
        ret = arg.mid( p );
      }
    }
    ++i;
  }
  return ret;
}

}


bool loadTranslator(QTranslator& trl, char const* name)
{
  return trl.load(
    QLocale(),
    QString::fromUtf8(name),
    QStringLiteral("_"),
    qApp->applicationDirPath());
}

// --- main ------------------------------------------------------------------

int main( int argc, char **argv )
{
  QApplication app( argc, argv );
  QTranslator tr_petu;
  if ( loadTranslator( tr_petu, "petu") ) {
    qApp->installTranslator( &tr_petu );
  }
  QTranslator tr_qt;
  if ( loadTranslator( tr_qt, "qt") ) {
    qApp->installTranslator( &tr_qt );
  }
  app.setAttribute( Qt::AA_DontShowIconsInMenus, true );
  Tournament tournament;
  QString tournament_path = getArg( 't' );
  if ( tournament_path.isEmpty() ) {
    QString const csv_path = getArg( 'p' );
    if ( ! csv_path.isEmpty() ) {
      tournament.loadPlayerList( csv_path );
    } else {
      tournament_path = getArg( 0 );
    } 
  }
  if ( ! tournament_path.isEmpty() ) {
    tournament.loadTournament( tournament_path );
  }
  MainWindow mw( tournament );
  mw.show();
  app.exec();
}

#include "main_window.h"
#include "tournament.h"
#include <qapplication.h>
#include <qtranslator.h>


namespace {

QString getArg( char c )
{
  QString ret;
  QStringList const args = QCoreApplication::arguments();
  int i = 1;
  while ( i < args.count() ) {
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


QString tr( char const* txt )
{
  return QApplication::tr( txt );
}

// --- main ------------------------------------------------------------------

int main( int argc, char **argv )
{
  QApplication app( argc, argv );
  static QTranslator tr_qt;
  if ( tr_qt.load( QStringLiteral("qtbase_de.qm"), qApp->applicationDirPath() ) ) {
    qApp->installTranslator( &tr_qt );
  }
  Tournament tournament;
  QString const tournament_path = getArg( 't' );
  if ( ! tournament_path.isEmpty() ) {
    tournament.loadTournament( tournament_path );
  } else {
    QString const csv_path = getArg( 'p' );
    if ( ! csv_path.isEmpty() ) {
      tournament.loadPlayerList( csv_path );
    }
  }
  QString const simul = getArg( 's' );
  if ( simul == QStringLiteral( "imulate" ) ) {
    extern void activateSimulator();
    activateSimulator();
  }
  MainWindow mw( tournament );
  mw.show();
  app.exec();
}

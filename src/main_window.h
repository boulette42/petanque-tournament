#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include "config.h"
#include <qmainwindow.h>

class ExternalWindow;
class RoundModel;
class PlayerModel;
class PlayerResultModel;
class SiteModel;
class TeamResultModel;
class Ui_MainWindow;
class QModelIndex;

class MainWindow : public QMainWindow
{
  Q_OBJECT

  enum class TabMode {
    current,
    player,
    site,
    round,
    result,
    all
  };

  QSharedPointer<Ui_MainWindow> ui_;
  QSharedPointer<Tournament> tournament_;
  QPointer<PlayerModel> player_model_;
  QPointer<SiteModel> site_model_;
  QPointer<RoundModel> round_model_;
  QPointer<PlayerResultModel> player_result_model_;
  QPointer<TeamResultModel> team_result_model_;
  QVector<QSharedPointer<ExternalWindow> > window_register_;

public:
  explicit MainWindow( Tournament const& tournament );
  ~MainWindow();

private:
  MainWindow( MainWindow const& );
  MainWindow& operator=( MainWindow const& );

  bool saveTournament( QString const& filename );
  void initModels();
  void updateView( TabMode tm = TabMode::current );
  void updateRoundSelect( int round_idx );
  void activateTab( TabMode mode );
  bool eventFilter( QObject* obj, QEvent* evt ) override;
  void closeEvent( QCloseEvent* event ) override;

private slots:
  void editSettings();
  void exportTournament();
  void loadPlayerList();
  void savePlayerList();
  void addPlayer();
  void newRound();
  void finishRound();
  void createWindow();
  void deleteAllWindows();
  void about();
  void updateSiteCount();
  void playerActivated( QModelIndex const& index );
  void matchActivated( QModelIndex const& index );
  void resultActivated( QModelIndex const& index );
  void roundChanged( int round_idx );
};

#endif  // MAIN_WINDOW_H

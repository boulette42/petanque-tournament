#ifndef EXTERNAL_WINDOW_H
#define EXTERNAL_WINDOW_H

#include "config.h"

class QAbstractItemModel;
class QDialog;
class QTreeView;


class ExternalWindow
{
  Q_DECLARE_TR_FUNCTIONS(ExternalWindow)

  QPointer<QDialog> dlg_;
  QPointer<QTreeView> tree_view_;
  int screen_ = 0;

public:
  ExternalWindow( QAbstractItemModel& model, int font_size );
  ~ExternalWindow();
  void setScreen( int screen ) { screen_ = screen; }
  void show();
  void close();
  void updateRound( int round_idx );
};

#endif  // EXTERNAL_WINDOW_H

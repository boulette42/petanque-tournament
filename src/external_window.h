#ifndef EXTERNAL_WINDOW_H
#define EXTERNAL_WINDOW_H

#include "config.h"

class QAbstractItemModel;
class QDialog;


class ExternalWindow
{
  Q_DECLARE_TR_FUNCTIONS(ExternalWindow)

  QPointer<QDialog> dlg_;

public:
  ExternalWindow( QAbstractItemModel& model, int font_size );
  ~ExternalWindow();
  void close();
  void updateRound( int round_idx );
};

#endif  // EXTERNAL_WINDOW_H

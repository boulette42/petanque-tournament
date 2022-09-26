#ifndef LOCK_SCREEN_H
#define LOCK_SCREEN_H

#include "config.h"

class QDialog;
class QLineEdit;
class QWidget;


class LockScreen
{
  Q_DECLARE_TR_FUNCTIONS(LockScreen)

  QPointer<QDialog> dlg_;
  QPointer<QLineEdit> le_password_;

public:
  explicit LockScreen( QWidget* parent );
  ~LockScreen();
  void exec();
};

#endif  // LOCK_SCREEN_H

#ifndef SITE_MODEL_H
#define SITE_MODEL_H

#include "config.h"
#include <qabstractitemmodel.h>


class SiteModel : public QAbstractItemModel
{
  Tournament& tournament_;

public:
  explicit SiteModel( Tournament& tournament );
  ~SiteModel();

  int columnCount() const;
  bool setSiteCount( int site_cnt );
  void setAllChecked( bool checked );

protected:
  QModelIndex index( int row, int column, QModelIndex const& parent = QModelIndex() ) const override;
  QModelIndex parent( QModelIndex const& /*mi*/ ) const override;
  int rowCount( QModelIndex const& parent ) const override;
  int columnCount( QModelIndex const& parent ) const override;
  QVariant data( QModelIndex const& mi, int role ) const override;
  bool setData( QModelIndex const& mi, QVariant const& value, int role ) override;
  Qt::ItemFlags flags( QModelIndex const& mi ) const;
};

#endif  //SITE_MODEL_H


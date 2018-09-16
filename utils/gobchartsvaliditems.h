/* Copyright (C) 2012 by William Hallatt.
 *
 * This file forms part of the "GobChartsWidget" library.
 *
 * This library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have downloaded a copy of the GNU General Public License
 * (GNUGPL.txt) and GNU Lesser General Public License (GNULGPL.txt)
 * along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 * The official website for this project is www.goblincoding.com and,
 * although not compulsory, it would be appreciated if all works of whatever
 * nature referring to or using this library include a reference to this site.
 */


#ifndef GOBCHARTSVALIDITEMS_H
#define GOBCHARTSVALIDITEMS_H

#include <QObject>
#include <QPair>
#include <QMultiMap>
#include "utils/gobchartsnocopy.h"

typedef QPair< QString, qreal > ItemPair;

/// Controls and manages valid model rows.

/** GobChartsValidItems keeps track of which data rows and columns contain valid entries (non-empty, legal types, etc) 
    and provides access to the valid values and category names based on the row number. */
class GobChartsValidItems : public QObject,
                            public GobChartsNoCopy
{
  Q_OBJECT
public:
  //! Constructor.
  explicit GobChartsValidItems( QObject *parent = 0 );

  //! Destructor.
  virtual ~GobChartsValidItems();

  /*! Creates and maps a category/value pair to its corresponding row. */
  void addValidItem( int row, QString category, qreal data );

  /*! Delete all valid items from the map. */
  void clear();

  /*! Returns a list of all rows containing valid items. */
  const QList< int > validRows() const;

  /*! Returns the category corresponding to a specific row. */
  QString category( int row ) const;

  /*! Returns the value corresponding to a specific row. */
  qreal data( int row ) const;

  /*! Returns the number of valid items in the map. */
  int size() const;

private:
  QMultiMap< int, ItemPair > m_validMap;
};

#endif // GOBCHARTSVALIDITEMS_H

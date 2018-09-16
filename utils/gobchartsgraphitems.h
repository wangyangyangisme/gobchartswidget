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

#ifndef GOBCHARTSGRAPHITEMS_H
#define GOBCHARTSGRAPHITEMS_H

#include <QObject>
#include <QMap>
#include <QModelIndex>
#include "utils/gobchartsnocopy.h"

class QGraphicsScene;
class QGraphicsItem;

/// Manages associations between graph items and model indices.

/** GobChartsGraphItems is a container class that manages associations between
    graph items (QGraphicsItem's) and their respective indices (QModelIndex's).
    The class furthermore provides the functionality to add or remove
    the mapped items to or from a QGraphicsScene and to highlight items currently selected
    in the view by changing the item's opacity to create the visual distinction. */
class GobChartsGraphItems : public QObject,
                            public GobChartsNoCopy
{
  Q_OBJECT

public:
  //! Constructor.
  explicit GobChartsGraphItems( QObject *parent = 0 );

  //! Destructor.
  virtual ~GobChartsGraphItems();

  /*! Adds all the mapped QGraphicsItem's to "scene". 
      \sa removeItemsFromScene() */
  void addItemsToScene( QGraphicsScene *scene );

  /*! Removes all the mapped QGraphicsItem's from "scene".
      \sa addItemsToScene() */
  void removeItemsFromScene( QGraphicsScene *scene );

  /*! Manipulates the QGraphicsItem corresponding to the given category row
      (if mapped and found) by changing it's opacity.  When selected the first time,
      this function lessens the item's opacity (makes it more transparent), otherwise
      it resets the item to being completely opaque.
      \sa clearSelection() */
  void setSelected( int categoryRow );

  /*! Clears the current selection.  This function will return all the graphic items
      to their original opacity. */
  void clearSelection();

  /*! Adds a QModelIndex/QGraphicsItem pair to the map.  This function furthermore ensures that
      the added item can be retrieved based on its corresponding legend text.
      \sa deleteItems() */
  void addItem( const QModelIndex &valueIndex, QGraphicsItem *item, const QString &legendText );

  /*! Delete all the mapped QGraphicsItem's and clear the map.
      \sa addItem() */
  void deleteItems();

  /*! Returns the model index corresponding to "item". */
  QModelIndex getModelIndex( QGraphicsItem *item ) const;

  /*! Returns the model index of the item with legend "text". */
  QModelIndex getModelIndex( const QString &text ) const;

  /*! Returns the rectangle of the graphics item corresponding to "index". */
  QRectF getItemRectF( const QModelIndex &index ) const;

  /*! Returns the rectangle of the graphics item corresponding to the legend with text "text". */
  QRectF getItemRectF( const QString &text ) const;

  /*! Returns the legend text associated with the graphics item. */
  QString getItemLegend( QGraphicsItem *item ) const;

  /*! Turn debug logging "on" or "off" (default "off").
      \sa lastDebugLogMsg() */
  void setDebugLoggingOn( bool logging );

signals:
  /*! Broadcast debug messages. This signal broadcasts the last debug log message and will only 
      be emitted when debug logging is turned "on".
      \sa setDebugLoggingOn() */
  void lastDebugLogMsg( QString );

private:
  QMap< QModelIndex, QGraphicsItem* > m_graphItemMap;
  QMap< QString,     QGraphicsItem* > m_legendItemMap;
  bool m_loggingOn;
};

#endif // GOBCHARTSGRAPHITEMS_H

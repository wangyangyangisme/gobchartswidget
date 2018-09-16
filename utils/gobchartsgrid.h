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

#ifndef GOBCHARTSGRID_H
#define GOBCHARTSGRID_H

#include <QList>
#include <QPen>
#include "utils/gobchartsnocopy.h"

class QGraphicsLineItem;
class QGraphicsScene;

/// Generates and manages grid and axes lines.

/** GobChartsGrid maintains a list of all grid lines as well as the chart's x and y axes.  
    Line lengths and positions are recalculated when constructGrid() is called, are
    determined by the dimensions and restricted to the boundaries of the grid's rectangle. 
    The class furthermore provides the functionality to add or remove the lines to a QGraphicsScene. */
class GobChartsGrid : public QObject,
                      public GobChartsNoCopy
{
  Q_OBJECT

public:
  //! Constructor.
  GobChartsGrid( QObject *parent = 0 );

  //! Destructor.
  virtual ~GobChartsGrid();

  /*! Adds the axes and grid lines (if any) to "scene". 
      \sa removeGridFromScene() */
  void addGridToScene( QGraphicsScene *scene );

  /*! Removes the axes and grid lines (if any) from "scene".
      \sa addGridToScene() */
  void removeGridFromScene( QGraphicsScene *scene );

  /*! Constructs grid and axes lines. Deletes all existing line items, recalculates the 
      grid's dimensions and creates the new axes and grid lines according to the geometric
      settings and custom grid specifications. */
  void constructGrid();

  /*! Sets the grid's spatial dimensions.  The grid will be confined to the 
      dimensions of the given rectangle. */
  void setGridRectF( const QRectF &rect );

  /*! Set the pen style used to draw the grid lines.
      @param style - dashed, dotted, solid, etc. (see Qt API documentation for details on the enum). */
  void setGridLineStyle( Qt::PenStyle style );

  /*! Set horizontal grid lines.
      Turn horizontal grid lines "on" or "off" and also sets the number of grid lines. */
  void setHorizontalGridLines( bool set, int number );

  /*! Set horizontal grid lines.
      Turn vertical grid lines "on" or "off" and also sets the number of grid lines. */
  void setVerticalGridLines( bool set, int number );

  /*! Sets the grid line's colour to "colour". */
  void setGridColour( QColor colour );

  /*! Returns the grid's width. */
  qreal gridWidth() const;

  /*! Turn debug logging "on" or "off" (default "off").
      \sa lastDebugLogMsg() */
  void setDebugLoggingOn( bool logging );

signals:
  /*! Broadcast debug messages.
      This signal broadcasts the last debug log message and will only be emitted when debug logging is turned "on".
      \sa setDebugLoggingOn() */
  void lastDebugLogMsg( QString );

private:
  QList< QGraphicsLineItem* > m_gridLines;
  QRectF m_rectF;
  QPen   m_gridPen;
  QPen   m_axesPen;
  int    m_nrHorizontalLines;
  int    m_nrVerticalLines;
  bool   m_showHorizontalGrid;
  bool   m_showVerticalGrid;
  bool   m_loggingOn;
};

#endif // GOBCHARTSGRID_H

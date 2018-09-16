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

#include "gobchartsgrid.h"
#include "utils/globalincludes.h"

#include <QGraphicsLineItem>
#include <QGraphicsScene>

/*--------------------------------------------------------------------------------*/

/* Distance line intercepts extend past axis. */
const int EXTEND = 5;

/*--------------------------------------------------------------------------------*/

GobChartsGrid::GobChartsGrid( QObject *parent ) :
  QObject             ( parent ),
  m_gridLines         (),
  m_rectF             (),
  m_gridPen           ( QPen( QBrush( Qt::black ), 1, Qt::DotLine ) ),
  m_axesPen           ( QPen( QBrush( Qt::black ), 2 ) ),
  m_nrHorizontalLines ( 0 ),
  m_nrVerticalLines   ( 0 ),
  m_showHorizontalGrid( false ),
  m_showVerticalGrid  ( false ),
  m_loggingOn         ( false )
{
}

/*--------------------------------------------------------------------------------*/

GobChartsGrid::~GobChartsGrid() 
{
  for( int i = 0; i < m_gridLines.size(); i++ )
  {
    QGraphicsLineItem *line = m_gridLines.at( i );

    if( line )
    {
      delete line;
      line = NULLPOINTER;
    }
  }
}

/*--------------------------------------------------------------------------------*/

void GobChartsGrid::addGridToScene( QGraphicsScene *scene ) 
{
  if( scene )
  {
    foreach( QGraphicsLineItem *line, m_gridLines )
    {
      if( line ) scene->addItem( line );
    }
  }
  else
  {
    if( m_loggingOn )
    {
      emit lastDebugLogMsg( tr( "GobChartsGrid::addGridToScene# Attempting to add grid to an invalid scene." ) );
    }
  }
}

/*--------------------------------------------------------------------------------*/

void GobChartsGrid::removeGridFromScene( QGraphicsScene *scene ) 
{
  if( scene )
  {
    foreach( QGraphicsLineItem *line, m_gridLines )
    {
      if( line ) scene->removeItem( line );
    }
  }
  else
  {
    if( m_loggingOn )
    {
      emit lastDebugLogMsg( tr( "GobChartsGrid::removeGridFromScene# Attempting to remove grid from an invalid scene." ) );
    }
  }
}

/*--------------------------------------------------------------------------------*/

void GobChartsGrid::constructGrid()
{
  /* Clear existing grid and axes lines. */
  if( !m_gridLines.empty() )
  {
    for( int i = 0; i < m_gridLines.size(); i++ )
    {
      QGraphicsLineItem *line = m_gridLines.at( i );

      if( line )
      {
        delete line;
        line = NULLPOINTER;
      }
    }

    m_gridLines.clear();   // remove pointers
  }

  /* Create x and y axes. */
  QGraphicsLineItem *xAxis = new QGraphicsLineItem;
  xAxis->setLine( m_rectF.left(), m_rectF.bottom(), m_rectF.right(), m_rectF.bottom() );
  xAxis->setPen ( m_axesPen );
  m_gridLines.append( xAxis );

  QGraphicsLineItem *yAxis = new QGraphicsLineItem;
  yAxis->setLine( m_rectF.left(), m_rectF.top(), m_rectF.left(), m_rectF.bottom() );
  yAxis->setPen ( m_axesPen );
  m_gridLines.append( yAxis );

  /* Create vertical line items. */
  if( m_nrVerticalLines > 0 )
  {
    qreal spacing = ( m_rectF.width()/m_nrVerticalLines );

    if( m_showVerticalGrid )              // vertical grid lines and "x" intercepts
    {
      for( int i = 1; i <= m_nrVerticalLines; i++ )
      {
        QGraphicsLineItem *xLine = new QGraphicsLineItem( m_rectF.left() + i * spacing,
                                                          m_rectF.top(),
                                                          m_rectF.left() + i * spacing,
                                                          m_rectF.bottom() + EXTEND );
        xLine->setPen( m_gridPen );
        m_gridLines.append( xLine );
      }
    }
  }

  /* Create horizontal line items. */
  if( m_nrHorizontalLines > 0 )
  {
    qreal spacing = ( m_rectF.height()/m_nrHorizontalLines );

    if( m_showHorizontalGrid )            // horizontal grid lines and "y" intercepts
    {
      for( int i = 0; i <= m_nrHorizontalLines; i++ )
      {
        QGraphicsLineItem *yLine = new QGraphicsLineItem( m_rectF.left() - EXTEND,
                                                          m_rectF.top() + i * spacing,
                                                          m_rectF.right(),
                                                          m_rectF.top() + i * spacing );
        yLine->setPen( m_gridPen );
        m_gridLines.append( yLine );
      }
    }
  }
}

/*--------------------------------------------------------------------------------*/

void GobChartsGrid::setGridRectF( const QRectF &rect )
{
  m_rectF = rect;
}

/*--------------------------------------------------------------------------------*/

qreal GobChartsGrid::gridWidth() const
{
  return m_rectF.width();
}

/*--------------------------------------------------------------------------------*/

void GobChartsGrid::setHorizontalGridLines( bool set, int number )
{
  m_showHorizontalGrid = set;
  m_nrHorizontalLines  = number;
}

/*--------------------------------------------------------------------------------*/

void GobChartsGrid::setVerticalGridLines( bool set, int number )
{
  m_showVerticalGrid = set;
  m_nrVerticalLines  = number;
}

/*--------------------------------------------------------------------------------*/

void GobChartsGrid::setGridColour( QColor colour )
{
  m_gridPen.setColor( colour );
}

/*--------------------------------------------------------------------------------*/

void GobChartsGrid::setGridLineStyle( Qt::PenStyle style )
{
  m_gridPen.setStyle( style );
}

/*--------------------------------------------------------------------------------*/

void GobChartsGrid::setDebugLoggingOn( bool logging )
{
  m_loggingOn = logging;
}

/*--------------------------------------------------------------------------------*/

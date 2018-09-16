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

#include "gobchartslineview.h"
#include "utils/gobchartscolours.h"

#include <QGraphicsEllipseItem>
#include <QPen>

/*--------------------------------------------------------------------------------*/

const qreal DOT_SIDE = 7;

/*--------------------------------------------------------------------------------*/

GobChartsLineView::GobChartsLineView( QWidget *parent ) :
  GobChartsView( parent )
{
}

/*--------------------------------------------------------------------------------*/


GobChartsLineView::~GobChartsLineView() 
{
  // Default destructor
}

/*--------------------------------------------------------------------------------*/

void GobChartsLineView::generateGraphicsItems()
{
  if( nrValidItems() > 0 )
  {
    emit clearLegend();

    qreal pointSpacing = gridWidth()/nrValidItems();
    qreal previousX    = innerSceneRectF().left();
    qreal nextX        = innerSceneRectF().left();
    qreal previousY    = innerSceneRectF().bottom();
    qreal nextY        = innerSceneRectF().bottom();

    QList< int > activeRows = validRowList();

    for( int row = 0; row < nrValidItems(); row++ )
    {
      QString cat = category( activeRows.at( row ) );
      qreal val   = value( activeRows.at( row ) );

      /* If the value doesn't fall within the specified data range, then ignore this particular category. */
      if( !isWithinAllowedRange( val ) )
      {
        /* Increment colours so that the look of the chart doesn't change
          when the user toggles between restricted and full ranges. */
        GobChartsColours::getNextColour();
        continue;
      }

      qreal dataPercentage = ( totalValue() > 0.0 ) ? ( val/totalValue() ) : 0.0;

      nextX = innerSceneRectF().left() + pointSpacing * row + pointSpacing/2;
      nextY = ( innerSceneRectF().bottom() - stripSpace( dataPercentage ) ) - innerSceneRectF().height() * dataPercentage;

      /* Create dot. */
      QGraphicsEllipseItem *dot = new QGraphicsEllipseItem( nextX - DOT_SIDE/2, nextY - DOT_SIDE/2, DOT_SIDE, DOT_SIDE);

      QColor colour;

      if( useFixedColour() )
      {
        colour = fixedColour();
        dot->setPen( QPen( colour, 1 ) );
        dot->setBrush( colour );
      }
      else
      {
        colour = GobChartsColours::getNextColour();
        dot->setPen( QPen( colour, 1 ) );
        dot->setBrush( colour );
      }

      dot->setZValue( nrValidItems() - row );

      /* Create line. */
      QGraphicsLineItem *lineItem = new QGraphicsLineItem( previousX, previousY, nextX, nextY, dot );
      lineItem->setPen( QPen( Qt::DotLine ) );
      lineItem->setFlag( QGraphicsItem::ItemStacksBehindParent );

      QString legendText = QString( "%1 - %2" ).arg( cat ).arg( val );
      emit createLegendItem( colour, legendText );

      addToGraphItemsContainer( model()->index( activeRows.at( row ),VALUE ), dot, legendText );

      previousX = nextX;
      previousY = nextY;
    }
  }
  else
  {
    debugLog( tr( "GobChartsLineView::generateGraphicsItems# No valid items." ) );
  }
}

/*--------------------------------------------------------------------------------*/

bool GobChartsLineView::needsGrid() const
{
  return true;
}

/*--------------------------------------------------------------------------------*/

QString GobChartsLineView::typeInteger() const
{
  return QString( "%1" ).arg( static_cast< int >( LINE ) );
}

/*--------------------------------------------------------------------------------*/

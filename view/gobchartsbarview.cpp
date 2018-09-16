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

#include "gobchartsbarview.h"
#include "utils/gobchartscolours.h"

#include <QGraphicsDropShadowEffect>
#include <QGraphicsRectItem>

/*--------------------------------------------------------------------------------*/

const int BAR_SPACING = 5;

/*--------------------------------------------------------------------------------*/

GobChartsBarView::GobChartsBarView( QWidget *parent ) : 
  GobChartsView( parent )
{
}

/*--------------------------------------------------------------------------------*/

GobChartsBarView::~GobChartsBarView()
{
}

/*--------------------------------------------------------------------------------*/

void GobChartsBarView::generateGraphicsItems()
{   
  /* Add the chart columns. */
  if( nrValidItems() > 0 )
  {
    emit clearLegend();

    qreal barColWidth = gridWidth()/nrValidItems();
    QList< int > validRows = validRowList();

    for( int row = 0; row < nrValidItems(); row++)
    {
      QString cat = category( validRows.at( row ) );
      qreal val   = value( validRows.at( row ) );

      /* If the value doesn't fall within the specified data range, then ignore this particular category. */
      if( !isWithinAllowedRange( val ) )
      {
        /* Increment colours so that the look of the chart doesn't change
          when the user toggles between restricted and full ranges. */
        GobChartsColours::getNextColour();
        continue;
      }

      qreal dataPercentage = ( totalValue() > 0.0 ) ? ( val/totalValue() ) : 0.0;

      /* Determine bar rectangle dimensions. */
      QPointF topLeft;
      QPointF bottomRight;

      if( dataPercentage < 0.01 )
      {
        /* If we don't have at least a snippet of a graphics item, a lot of the selection model's functionality
          doesn't work as well as it could.  Create at least the semblance of a bar if the value is zero. */
        topLeft = QPointF( innerSceneRectF().left() + barColWidth * row,
                           innerSceneRectF().bottom() - 1 /* pixel */ );

        bottomRight = QPointF( innerSceneRectF().left() + barColWidth * row + barColWidth - BAR_SPACING,
                               innerSceneRectF().bottom() );
      }
      else
      {
        topLeft = QPointF( innerSceneRectF().left() + barColWidth * row,
                           innerSceneRectF().bottom() - stripSpace( dataPercentage ) - dataPercentage * innerSceneRectF().height() );

        bottomRight = QPointF( innerSceneRectF().left() + barColWidth * row + barColWidth - BAR_SPACING,
                               innerSceneRectF().bottom() );
      }

      QRectF barRectangle( topLeft, bottomRight );
      QLinearGradient columnGradient( barRectangle.bottomLeft(), barRectangle.topRight() );
      columnGradient.setColorAt( 0, QColor( ( Qt::GlobalColor ) 2 ) );

      QColor colour;

      if( useFixedColour() )
      {
        colour = fixedColour();
        columnGradient.setColorAt( 1, colour );
      }
      else
      {
        colour = GobChartsColours::getNextColour();
        columnGradient.setColorAt( 1, colour );
      }

      /* Create bar item and add to graph items container. */
      QBrush columnBrush( columnGradient );
      QGraphicsRectItem *graphicsBar = new QGraphicsRectItem( barRectangle );
      graphicsBar->setBrush( columnBrush );

      QGraphicsDropShadowEffect *dropShadow = new QGraphicsDropShadowEffect;
      dropShadow->setOffset( QPointF( 2,-2 ) );         // two points to the top and right
      graphicsBar->setGraphicsEffect( dropShadow );     // takes ownership

      QString legendText = QString( "%1 - %2" ).arg( cat ).arg( val );
      emit createLegendItem( colour, legendText );

      addToGraphItemsContainer( model()->index( validRows.at( row ), VALUE ), graphicsBar, legendText );
    }
  }
  else
  {
    debugLog( tr( "GobChartsBarView::generateGraphicsItems# No valid items." ) );
  }
}

/*--------------------------------------------------------------------------------*/

bool GobChartsBarView::needsGrid() const
{
  return true;
}

/*--------------------------------------------------------------------------------*/

QString GobChartsBarView::typeInteger() const
{
  return QString( "%1" ).arg( static_cast< int >( BAR ) );
}

/*--------------------------------------------------------------------------------*/

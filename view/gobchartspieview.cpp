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

#include "gobchartspieview.h"
#include "utils/gobchartscolours.h"
#include "utils/globalincludes.h"

#include <QGraphicsDropShadowEffect>
#include <QGraphicsEllipseItem>

/*--------------------------------------------------------------------------------*/

GobChartsPieView::GobChartsPieView( QWidget *parent ) :
  GobChartsView( parent )
{
}

/*--------------------------------------------------------------------------------*/

GobChartsPieView::~GobChartsPieView() 
{
  // Default destructor
}

/*--------------------------------------------------------------------------------*/

void GobChartsPieView::generateGraphicsItems()
{
  if( nrValidItems() > 0 )
  {
    emit clearLegend();

    /* Calculate pie positioning specs. */
    qreal pieWidth  = gridWidth();
    qreal pieHeight = innerSceneRectF().height();
    QRectF pieRectangle( innerSceneRectF().left(), innerSceneRectF().top(), pieWidth, pieHeight );

    /* Add pie segments. */
    qreal fullElipse  = 5760;     // span angles in 16th of a degree (360*16)
    int lastStopAngle = 0;

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

      qreal dataPercentage = ( totalValue() > 0.0 ) ? ( val/totalValue()) : 0.0;

      /* Draw ellipse. */
      QGraphicsEllipseItem *graphSegment = new QGraphicsEllipseItem( pieRectangle );

      QColor colour;

      if( useFixedColour() )
      {
        colour = fixedColour();
        graphSegment->setBrush( colour );
      }
      else
      {
        colour = GobChartsColours::getNextColour();
        graphSegment->setBrush( colour );
      }

      graphSegment->setStartAngle( lastStopAngle );
      graphSegment->setSpanAngle( qRound( dataPercentage * fullElipse ) );

      QString legendText = QString( "%1 - %2" ).arg( cat ).arg( val );
      emit createLegendItem( colour, legendText );

      addToGraphItemsContainer( model()->index( activeRows.at( row ), VALUE ), graphSegment, legendText );

      lastStopAngle += graphSegment->spanAngle();
    }
  }
  else
  {
    debugLog( tr( "GobChartsPieView::generateGraphicsItems# No valid items." ) );
  }
}

/*--------------------------------------------------------------------------------*/

bool GobChartsPieView::needsGrid() const
{
  return false;
}

/*--------------------------------------------------------------------------------*/

QString GobChartsPieView::typeInteger() const
{
  return QString( "%1" ).arg( static_cast< int >( PIE ) );
}

/*--------------------------------------------------------------------------------*/

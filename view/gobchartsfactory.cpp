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

#include "gobchartsfactory.h"
#include "gobchartsbarview.h"
#include "gobchartspieview.h"
#include "gobchartslineview.h"

/*--------------------------------------------------------------------------------*/

GobChartsFactory *GobChartsFactory::m_instance = NULLPOINTER;

/*--------------------------------------------------------------------------------*/

GobChartsFactory *GobChartsFactory::getInstance()
{
  if( !m_instance )
  {
    m_instance = new GobChartsFactory();
  }

  return m_instance;
}

/*--------------------------------------------------------------------------------*/

GobChartsFactory::GobChartsFactory()
{
  // Default
}

/*--------------------------------------------------------------------------------*/

GobChartsView *GobChartsFactory::createChart( GobChartsType type, QWidget *parent )
{
  switch( type )
  {
  case BAR:
    return new GobChartsBarView( parent );
  case PIE:
    return new GobChartsPieView( parent );
  case LINE:
    return new GobChartsLineView( parent );
  }

  return NULLPOINTER;
}

/*--------------------------------------------------------------------------------*/

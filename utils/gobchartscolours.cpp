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

#include "gobchartscolours.h"

/*--------------------------------------------------------------------------------*/

namespace GobChartsColours
{
  namespace{
    /* Random colour generation had limited success so I decided to rather opt for a fixed list of colours to ensure that
      no two colours are the same. I believe this list should prove sufficient for most applications */
    const QList<QColor> colourList = QList<QColor>()
                                 << QColor(25,121,39)   << QColor(237,147,21)  << QColor(238,65,21)
                                 << QColor(232,22,21)   << QColor(228,20,186)  << QColor(20,25,222)   << QColor(149,14,129)
                                 << QColor(79,155,29)   << QColor(237,169,20)  << QColor(238,96,32)
                                 << QColor(231,49,57)   << QColor(243,51,205)  << QColor(33,81,220)   << QColor(169,41,149)
                                 << QColor(91,176,34)   << QColor(237,191,21)  << QColor(238,125,47)
                                 << QColor(243,42,51)   << QColor(244,82,212)  << QColor(69,107,217)  << QColor(164,64,149)
                                 << QColor(109,212,41)  << QColor(238,182,82)  << QColor(251,65,74)
                                 << QColor(237,22,20)   << QColor(244,158,212) << QColor(109,138,181) << QColor(167,96,156);

    int colourIndex(-1);         // will be set to 0 on first getNextColour call
  }

/*--------------------------------------------------------------------------------*/

  QColor getNextColour()
  {
    if( colourIndex == ( colourList.size() - 1 ) )
    {
      colourIndex = 0;          // end of list, reset
    }
    else
    {
      colourIndex++;
    }

    return QColor( colourList.at( colourIndex ) );
  }

/*--------------------------------------------------------------------------------*/

  void resetColourIndex()
  {
    colourIndex = -1;
  }

/*--------------------------------------------------------------------------------*/

}

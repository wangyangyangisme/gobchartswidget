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

#ifndef GOBCHARTSLINEVIEW_H
#define GOBCHARTSLINEVIEW_H

#include "gobchartsview.h"

/**  \ingroup ChartViews */

/// Responsible for drawing LINE charts.

/** This class implements the following GobChartsView pure virtual functions:

  -# generateGraphicsItems() 
  -# needsGrid() 
  -# typeInteger() 
*/
class GobChartsLineView : public GobChartsView
{
public:
  //! Constructor.
  GobChartsLineView( QWidget *parent = 0 );

  //! Destructor.
  virtual ~GobChartsLineView();

protected:
  /*! Graphics (chart) items.
      This function generates the chart's line segments and uses the various categories' values
      to determine each segment's relative size with respect to each other and the confines of the available space. */ 
  void generateGraphicsItems();

  /*! Grid required (returns "true"). */
  bool needsGrid() const;

  /*! Type integer is "2". */
  QString typeInteger()  const;
};

#endif // GOBCHARTSLINEVIEW_H

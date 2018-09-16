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

#ifndef GOBCHARTSFACTORY_H
#define GOBCHARTSFACTORY_H

#include "utils/globalincludes.h"
#include "utils/gobchartsnocopy.h"

class GobChartsView;
class QWidget;

/// Factory class responsible for the creation of charts at run-time.

/** This factory is implemented as a singleton and is responsible for the creation of the user-selected
    chart types at run-time.  Care must be taken when using this factory as it does not maintain
    ownership of the chart views it creates and clean-up becomes the responsibility of the calling object
    when no parent is specified. */
class GobChartsFactory : GobChartsNoCopy
{
public:
  /*! Returns the singleton instance. */
  static GobChartsFactory *getInstance();

  /*! Creates a chart view of type "type".  If no parent is provided, clean-up becomes the responsibility
      of the calling object. */
  GobChartsView *createChart( GobChartsType type, QWidget *parent );

private:
  /*! Private constructor. */
  GobChartsFactory();

  static GobChartsFactory *m_instance;
};

#endif // GOBCHARTSFACTORY_H

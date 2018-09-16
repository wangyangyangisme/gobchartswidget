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

#include "gobchartsvaliditems.h"

GobChartsValidItems::GobChartsValidItems( QObject *parent ) :
  QObject   ( parent ),
  m_validMap()
{
}

/*--------------------------------------------------------------------------------*/

GobChartsValidItems::~GobChartsValidItems() 
{
  // Default destructor
}

/*--------------------------------------------------------------------------------*/

void GobChartsValidItems::addValidItem( int row, QString category, qreal data ) 
{
  ItemPair pair;
  pair.first  = category;
  pair.second = data;
  m_validMap.insert( row, pair );
}

/*--------------------------------------------------------------------------------*/

void GobChartsValidItems::clear() 
{
  m_validMap.clear();
}

/*--------------------------------------------------------------------------------*/

const QList< int > GobChartsValidItems::validRows() const
{
  return m_validMap.keys();
}

/*--------------------------------------------------------------------------------*/

QString GobChartsValidItems::category( int row) const 
{
  if( m_validMap.find( row ) != m_validMap.end() )
  {
    return m_validMap.value( row ).first.trimmed();
  }

  return QString( "Invalid Row" );
}

/*--------------------------------------------------------------------------------*/

qreal GobChartsValidItems::data( int row ) const 
{
  if( m_validMap.find( row ) != m_validMap.end() )
  {
    return m_validMap.value( row ).second;
  }

  return 0.0;
}

/*--------------------------------------------------------------------------------*/

int GobChartsValidItems::size() const 
{
  return m_validMap.size();
}

/*--------------------------------------------------------------------------------*/

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

#include "gobchartsgraphitems.h"
#include "utils/globalincludes.h"

#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QModelIndex>

/*--------------------------------------------------------------------------------*/

const qreal OPACITY = 0.65;

/*--------------------------------------------------------------------------------*/

GobChartsGraphItems::GobChartsGraphItems( QObject *parent ) :
  QObject        ( parent ),
  m_graphItemMap (),
  m_legendItemMap(),
  m_loggingOn    ( false )
{
}

/*--------------------------------------------------------------------------------*/

GobChartsGraphItems::~GobChartsGraphItems()
{
  deleteItems();
}

/*--------------------------------------------------------------------------------*/

void GobChartsGraphItems::addItemsToScene( QGraphicsScene *scene )
{
  if( scene )
  {
    foreach( QGraphicsItem *item, m_graphItemMap.values() )
    {
      if( item ) scene->addItem( item );
    }
  }
  else
  {
    if( m_loggingOn )
    {
      emit lastDebugLogMsg( tr( "GobChartsGraphItems::addItemsToScene# Attempting to add items to an invalid scene." ) );
    }
  }
}

/*--------------------------------------------------------------------------------*/

void GobChartsGraphItems::removeItemsFromScene( QGraphicsScene *scene ) 
{
  if( scene )
  {
    foreach( QGraphicsItem *item, m_graphItemMap.values() )
    {
      if( item ) scene->removeItem( item );
    }
  }
  else
  {
    if( m_loggingOn )
    {
      emit lastDebugLogMsg( tr( "GobChartsGraphItems::removeItemsFromScene# Attempting to remove items from an invalid scene." ) );
    }
  }
}

/*--------------------------------------------------------------------------------*/

void GobChartsGraphItems::setSelected( int categoryRow )
{
  QMap<QModelIndex, QGraphicsItem*>::iterator it = m_graphItemMap.begin();

  while( it != m_graphItemMap.end() )
  {
    if( ( it.key().row() == categoryRow ) && ( it.value()->opacity() != OPACITY ) )
    {
      it.value()->setOpacity( OPACITY );
    }
    else
    {
      it.value()->setOpacity( 1.0 );
    }

    it++;
  }
}

/*--------------------------------------------------------------------------------*/

void GobChartsGraphItems::clearSelection()
{
  QMap<QModelIndex, QGraphicsItem*>::iterator it = m_graphItemMap.begin();

  while( it != m_graphItemMap.end() )
  {
    it.value()->setOpacity( 1.0 );
    it++;
  }
}

/*--------------------------------------------------------------------------------*/

void GobChartsGraphItems::addItem( const QModelIndex &valueIndex, QGraphicsItem *item, const QString &legendText )
{
  if( item )
  {
    m_graphItemMap.insert( valueIndex, item );
    m_legendItemMap.insert( legendText, item );
  }
}

/*--------------------------------------------------------------------------------*/

void GobChartsGraphItems::deleteItems() 
{
  foreach( QGraphicsItem *item, m_graphItemMap )
  {
    if( item )
    {
      delete item;
      item = NULLPOINTER;
    }
  }

  m_graphItemMap.clear();
  m_legendItemMap.clear();
}

/*--------------------------------------------------------------------------------*/

QModelIndex GobChartsGraphItems::getModelIndex( QGraphicsItem *item ) const
{
  QModelIndex defaultKey;

  if( item )
  {
    return m_graphItemMap.key( item, defaultKey );
  }

  return defaultKey;
}

/*--------------------------------------------------------------------------------*/

QModelIndex GobChartsGraphItems::getModelIndex( const QString &text  ) const
{
  QModelIndex defaultKey;

  if( m_legendItemMap.contains( text ) )
  {
    return m_graphItemMap.key( m_legendItemMap.value( text ), defaultKey );
  }

  return defaultKey;
}

/*--------------------------------------------------------------------------------*/

QRectF GobChartsGraphItems::getItemRectF( const QModelIndex &index ) const
{
  if( m_graphItemMap.contains( index ) )
  {
    QPointF point = m_graphItemMap.value( index )->sceneBoundingRect().center();
    QRectF rect( point, point );
    rect.setHeight( 1 );
    rect.setWidth( 1 );
    return rect;
  }

  return QRectF();
}

/*--------------------------------------------------------------------------------*/

QRectF GobChartsGraphItems::getItemRectF( const QString &text ) const
{
  if( m_legendItemMap.contains( text ) )
  {
    QPointF point = m_legendItemMap.value( text )->sceneBoundingRect().center();
    QRectF rect( point, point );
    rect.setHeight( 1 );
    rect.setWidth( 1 );
    return rect;
  }

  return QRectF();
}

/*--------------------------------------------------------------------------------*/

QString GobChartsGraphItems::getItemLegend( QGraphicsItem *item ) const
{
  if( item )
  {
    return m_legendItemMap.key( item );
  }

  return QString();
}

/*--------------------------------------------------------------------------------*/

void GobChartsGraphItems::setDebugLoggingOn( bool logging )
{
  m_loggingOn = logging;
}

/*--------------------------------------------------------------------------------*/

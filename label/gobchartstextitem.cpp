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

#include "gobchartstextitem.h"
#include "utils/globalincludes.h"

#include <QtCore/qmath.h>
#include <QFontMetrics>
#include <QTextDocument>
#include <QTextOption>
#include <QDomDocument>
#include <QMessageBox>
#include <QTextCursor>

/*--------------------------------------------------------------------------------*/

/* Used to calculate the margin of difference between a GobChartTextItem's height and allowed
  height in which resize events will not be triggered.  Inclusion of this "dead" space prevents
  the item's growth from triggering a shrinking calculation and vice versa. */
qreal PERCENTAGE_DEAD_SPACE   = 0.05;

/* Used to calculate the margin of difference between a GobChartTextItem's width
  based on the total available width of the provided rectangle. */
qreal PERCENTAGE_WIDTH_MARGIN = 0.05;


/*------------------------ NONMEMBER UTILITY FUNCTIONS ---------------------------*/

bool WiderThanNew( const QRectF &memberRect, const QRectF &newTextRect )
{
  return ( qFloor( memberRect.width() ) > qFloor( newTextRect.width() ) );
}

/*--------------------------------------------------------------------------------*/

bool HigherThanNew( const QRectF &memberRect, const QRectF &newTextRect )
{
  return ( qFloor( memberRect.height() ) > qFloor( newTextRect.height() ) );
}

/*--------------------------------------------------------------------------------*/

bool HasSameHeight( const QRectF &memberRect, const QRectF &newTextRect )
{
  return ( qFloor( memberRect.height() ) == qFloor( newTextRect.height() ) );
}

/*--------------------------------------------------------------------------------*/

bool HasSameWidth( const QRectF &memberRect, const QRectF &newTextRect )
{
  return ( qFloor( memberRect.width() ) == qFloor( newTextRect.width() ) );
}

/*--------------------------------------------------------------------------------*/

bool LargerThanNew( const QRectF &memberRect, const QRectF &newTextRect )
{
  return !( ( ( WiderThanNew ( memberRect, newTextRect ) || HasSameWidth ( memberRect, newTextRect ) ) && HigherThanNew( memberRect, newTextRect ) ) ||
            ( ( HigherThanNew( memberRect, newTextRect ) || HasSameHeight( memberRect, newTextRect ) ) && WiderThanNew ( memberRect, newTextRect ) ) );
}


/*------------------------------- MEMBER FUNCTIONS -------------------------------*/

GobChartsTextItem::GobChartsTextItem( Qt::Orientation orientation, QString uniqueID, QGraphicsItem *parent ):
  QGraphicsTextItem( parent ),
  m_orientation    ( orientation ),
  m_alignment      ( Qt::AlignHCenter ),
  m_textDocument   ( NULLPOINTER ),
  m_identity       ( uniqueID ),
  m_rectF          (),
  m_busyResizing   ( false ),
  m_maxFontSize    ( 11 )
{
  setTextInteractionFlags( Qt::TextEditable | Qt::TextEditorInteraction );

  /* Only catering for a standard bottom to top orientation, i.e. if you tilt your
    head to the left, you should be able to read the text left to right without difficulty. */
  if( m_orientation == Qt::Vertical )
  {
    setRotation( -90 );
  }

  /* Cleanup of m_textDocument is handled by the object tree, "this" takes ownership. */
  m_textDocument = new QTextDocument( this );

  /* Direct changes to the item's text will trigger a resize. */
  connect( m_textDocument, SIGNAL( contentsChanged() ), this, SLOT( resize() ) );

  /* Set the alignment. */
  QTextOption textOption( m_alignment );
  m_textDocument->setDefaultTextOption( textOption );
  setDocument( m_textDocument );
}

/*--------------------------------------------------------------------------------*/

void GobChartsTextItem::setRectF( const QRectF &rect )
{
  qreal widthMargin  = rect.width()  * PERCENTAGE_WIDTH_MARGIN;
  qreal heightMargin = rect.height() * PERCENTAGE_DEAD_SPACE;

  QPointF pos;

  if( m_orientation == Qt::Horizontal )
  {
    /* For horizontal orientations, do exactly what you'd expect. */
    m_rectF.setWidth ( rect.width()  - widthMargin );
    m_rectF.setHeight( rect.height() - heightMargin );

    pos = rect.topLeft();
    pos.ry() += ( heightMargin / 2 );
  } else {
    /* For vertical orientations, swap width and height. */
    m_rectF.setHeight( rect.width()  - widthMargin );
    m_rectF.setWidth ( rect.height() - heightMargin );

    pos = rect.bottomLeft();
    pos.ry() -= ( heightMargin / 2 );
  }

  pos.rx() += ( widthMargin  / 2 );
  setPos( pos );

  setTextWidth( m_rectF.width() );
  resize();
}

/*--------------------------------------------------------------------------------*/

void GobChartsTextItem::setMaxFontSize( const QFont &font )
{
  m_maxFontSize = font.pointSize();
  setFont( font );
}

/*--------------------------------------------------------------------------------*/

void GobChartsTextItem::setFont( const QFont &font )
{
  QGraphicsTextItem::setFont( font );
  resize();
}

/*--------------------------------------------------------------------------------*/

void GobChartsTextItem::setPlainText( const QString &text )
{
  QGraphicsTextItem::setPlainText( text );

  QTextCursor cursor( textCursor() );
  cursor.movePosition( QTextCursor::EndOfLine );
  setTextCursor( cursor );
}

/*--------------------------------------------------------------------------------*/

void GobChartsTextItem::setAlignment( Qt::Alignment alignment )
{
  /* Formatting the text document triggers a resize through
    the QTextDocument::contentsChanged() signal. */
  m_alignment = alignment;
  QTextOption textOption( m_alignment );
  m_textDocument->setDefaultTextOption( textOption );
}

/*--------------------------------------------------------------------------------*/

Qt::Alignment GobChartsTextItem::alignment() const
{
  return m_alignment;
}

/*--------------------------------------------------------------------------------*/

void GobChartsTextItem::resize()
{
  /* Avoid triggering resizing recursively with the setFont() calls below. */
  if( !m_busyResizing && !m_rectF.isNull() )
  {
    m_busyResizing = true;

    /* Creating a temporary item rather than updating this object's font the
      whole time speeds things up a little bit (we delay setting the item's new
      font until it's the desired size), but it is predominantly to make the
      logic that follows easier to understand and the code more legible. */
    QGraphicsTextItem tempItem( toPlainText() );
    tempItem.setTextWidth( textWidth() );

    QFont newFont( font() );
    tempItem.setFont( newFont );

    QRectF newRect( tempItem.boundingRect() );
    bool   fontTooSmall( WiderThanNew ( m_rectF, newRect ) || HigherThanNew( m_rectF, newRect ) );

    /* If the user set a specific point size, we don't want to exceed it. */
    if( fontTooSmall && ( newFont.pointSize() < m_maxFontSize ) )
    {
      /* Increase the font size for as long as we stay within the acceptable boundaries. */
      while( fontTooSmall && !LargerThanNew( m_rectF, newRect ) )
      {
        newFont.setPointSize( newFont.pointSize() + 1 );
        tempItem.setFont( newFont );
        newRect = tempItem.boundingRect();
        fontTooSmall = ( WiderThanNew ( m_rectF, newRect ) || HigherThanNew( m_rectF, newRect ) );
      }

      /* If the last iteration pushed the size beyond the acceptable
        limits, decrease the font size with one. */
      if( LargerThanNew( m_rectF, newRect ) && newFont.pointSize() > 1 )
      {
        newFont.setPointSize( newFont.pointSize() - 1 );
        tempItem.setFont( newFont );
        newRect = tempItem.boundingRect();
      }
    } else {
      /* Decreases the font size until we're within the acceptable boundaries. */
      while( ( LargerThanNew( m_rectF, newRect ) || newFont.pointSize() > m_maxFontSize ) &&
               newFont.pointSize() > 1 )
      {
        newFont.setPointSize( newFont.pointSize() - 1 );
        tempItem.setFont( newFont );
        newRect = tempItem.boundingRect();
      }
    }

    setFont( newFont );
    m_busyResizing = false;
  }
}

/*--------------------------------------------------------------------------------*/

void GobChartsTextItem::receiveKeyEvent( QKeyEvent *event )
{
  keyPressEvent( event );
}

/*--------------------------------------------------------------------------------*/

QString GobChartsTextItem::getStateXML() const
{
  QDomDocument doc;
  QDomElement root = doc.createElement( "LabelDetails" );
  doc.appendChild( root );

  QDomElement fontElement = doc.createElement( "Font" );
  fontElement.setAttribute( "value", font().toString() );
  root.appendChild( fontElement );

  QDomElement fontColour = doc.createElement( "FontColour" );
  fontColour.setAttribute( "red",   defaultTextColor().red() );
  fontColour.setAttribute( "green", defaultTextColor().green() );
  fontColour.setAttribute( "blue",  defaultTextColor().blue() );
  root.appendChild( fontColour );

  QDomElement maxFont = doc.createElement( "MaxFontSize" );
  maxFont.setAttribute( "value", m_maxFontSize );
  root.appendChild( maxFont );

  QDomElement text = doc.createElement( "Text" );
  text.setAttribute( "value", toPlainText() );
  root.appendChild( text );

  QDomElement align = doc.createElement( "Alignment" );

  if( m_alignment == Qt::AlignLeft )
  {
    align.setAttribute( "value", "left" );
  }
  else if( m_alignment == Qt::AlignRight )
  {
    align.setAttribute( "value", "right" );
  }
  else
  {
    align.setAttribute( "value", "centre" );
  }

  root.appendChild( align );

  return doc.toString( 2 );
}

/*--------------------------------------------------------------------------------*/

bool GobChartsTextItem::setStateXML( const QDomNode &node )
{
  if( !node.isNull() )
  {
    QDomNode labelDetails = node.firstChildElement( "LabelDetails" );

    m_maxFontSize = labelDetails.firstChildElement( "MaxFontSize" ).attribute( "value" ).toInt();

    QFont tempFont;
    tempFont.fromString( labelDetails.firstChildElement( "Font" ).attribute( "value" ) );
    setFont( tempFont );

    setDefaultTextColor( QColor( labelDetails.firstChildElement( "FontColour" ).attribute( "red" ).toInt(),
                                 labelDetails.firstChildElement( "FontColour" ).attribute( "green" ).toInt(),
                                 labelDetails.firstChildElement( "FontColour" ).attribute( "blue" ).toInt() ) );

    setPlainText( labelDetails.firstChildElement( "Text" ).attribute( "value" ) );

    QString alignMent = labelDetails.firstChildElement( "Alignment" ).attribute( "value" );

    if( alignMent == "left" )
    {
      m_alignment = Qt::AlignLeft;
    }
    else if( alignMent == "right" )
    {
      m_alignment = Qt::AlignRight;
    }
    else
    {
      m_alignment = Qt::AlignCenter;    // if we get an unknown, return the default
    }

    return true;
  }

  return false;
}

/*--------------------------------------------------------------------------------*/

void GobChartsTextItem::mousePressEvent( QGraphicsSceneMouseEvent *event )
{
  QGraphicsTextItem::mousePressEvent( event );
  emit identity( m_identity );
}

/*--------------------------------------------------------------------------------*/

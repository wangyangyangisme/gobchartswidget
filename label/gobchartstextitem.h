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

#ifndef GOBCHARTSTEXTITEM_H
#define GOBCHARTSTEXTITEM_H

#include <QGraphicsTextItem>
#include "utils/gobchartsnocopy.h"

class QTextDocument;
class QDomNode;

/// A convenience text item class that can resize itself.

/** GobChartsTextItem automatically resizes itself to fit into whichever space is
    allocated to it and allows for the text alignment to be specified. */

class GobChartsTextItem : public QGraphicsTextItem,
                          public GobChartsNoCopy
{
  Q_OBJECT

public:
  //! Constructor.
  explicit GobChartsTextItem( Qt::Orientation orientation, QString uniqueID, QGraphicsItem *parent = 0 );

  /*! Set maximum font size. Updates the item's font, but also keeps track of the user selected
      font size to ensure that, when automatically increasing the text size,
      the user preference is respected. */
  void setMaxFontSize( const QFont &font );

  /*! Sets the text alignment option (the default is Qt::AlignHCenter).
      \sa alignment() */
  void setAlignment( Qt::Alignment alignment );

  /*! Returns the current text alignment.
      \sa setAlignment() */
  Qt::Alignment alignment() const;

  /*! Receives forwarded keyboard input events. */
  void receiveKeyEvent( QKeyEvent *event );

  /*! Returns the label's content and settings as an XML QString.
      Charts are saved to file as XML, this function takes all the label's settings, converts it to XML and returns
      the XML string.
      \sa setStateXML() */
  QString getStateXML() const;

  /*! Set the label's values when loading from file.
      When loading a label from file, the content of the file is parsed from XML and the QDomNodes 
      containing the necessary information for building the label is provided to this function as parameter.
      \sa getStateXML() */
  bool setStateXML( const QDomNode &node );

  /*! Re-implemented from QGraphicsTextItem.  See Qt API documentation for details. */
  void setPlainText( const QString &text );

public slots:
  /*! Sets the dimensions within which the text item's bounding
      rectangle must fit.  The item will reposition itself to be
      centred within this given rectangular space (it's position
      will be calculated in terms of this provided rectangle). */
  void setRectF( const QRectF &rect );

signals:
  /*! Broadcast the item's identity. */
  void identity( const QString& );

protected:
  /*! Re-implemented from QGraphicsTextItem.  See Qt API documentation for details. */
  void mousePressEvent( QGraphicsSceneMouseEvent *event );

  /*! Re-implemented from QGraphicsTextItem to call resize() whenever the font is changed. */
  void setFont( const QFont &font );

private slots:
  void resize();

private:
  Qt::Orientation m_orientation;
  Qt::Alignment   m_alignment;
  QTextDocument  *m_textDocument;
  QString         m_identity;
  QRectF          m_rectF;
  bool            m_busyResizing;
  int             m_maxFontSize;
};

#endif // GOBCHARTSTEXTITEM_H

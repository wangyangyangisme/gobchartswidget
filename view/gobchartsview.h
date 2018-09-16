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

#ifndef GOBCHARTSVIEW_H
#define GOBCHARTSVIEW_H

#include <QAbstractItemView>
#include "utils/globalincludes.h"
#include "utils/gobchartsnocopy.h"

class QGraphicsView;
class QGraphicsItem;
class QDomNode;
class GobChartsTextItem;

/// Abstract base class from which all chart type (view) classes must inherit.

/**  \defgroup ChartViews Chart Views
    
    Each view class represents a specific chart type (with the exception of GobChartsView, the
    abstract base class from which all these views derive).
    
    The three supported chart types and their corresponding view classes are:\n
    - BAR - GobChartsBarView
    - PIE - GobChartsPieView
    - LINE - GobChartsLineView
*/

/** Derived classes must implement the following pure virtual functions:

  -# generateGraphicsItems() - creates the graphics items required for the 
                               specific chart type and positions them within the view.

  -# needsGrid() - if the chart type supports grids, this function must return "true" 
                   (e.g. PIE charts don't support chart grids whereas BAR charts do).

  -# typeInteger() - returns an integer value as a QString.  
                     The value thus returned must match one of the GobChartsType enum values.
*/
class GobChartsView : public QAbstractItemView,
                      public GobChartsNoCopy
{
  Q_OBJECT

public:
  /*! Constructor.
      Public virtual constructor so that we may delete "new"-ed derived objects from pointer-to-base.
      GobChartsFactory and the underlying strategy pattern depend on this for run-time type polymorphism. */
  virtual ~GobChartsView();

  /*! Displays the chosen chart type.
      Generates and displays the chart within the confines of the QGraphicsView which contains it. */
  void drawChart();

  /*! Informs the view that a legend of name "text" has been selected. */
  void legendItemSelected( const QString &text );

  /*! Set the chart values when loading from file.
      When loading a chart from file, the content of the file is parsed from XML and the QDomNodes 
      containing the necessary information for building the chart is provided to this function as parameters.
      @param viewNode - the QDomNode containing settings specific to the chart view itself.
      @param dataNode - the QDomNode containing the data read from the saved file.
      @param includeData - whether or not the dataNode's content must be read (if "false", it will be ignored). 
      \sa getStateXML() */
  void setStateXML( const QDomNode &viewNode, const QDomNode &dataNode, bool includeData = true );

  /*! Returns the chart content and settings as an XML QString.
      Charts are saved to file as XML, this function takes all the view settings, converts it to XML and returns
      the XML string.
      @param includeData - specifies whether or not the chart data must also be saved (if "false", then only the
      view settings are provided in the XML QString). 
      \sa setStateXML() */
  QString getStateXML( bool includeData = true ) const;

  /*! Re-implemented from QAbstractItemView. See the Qt API documentation for details. */
  QRect visualRect( const QModelIndex &index ) const;

  /*! Re-implemented from QAbstractItemView. See the Qt API documentation for details. */
  QModelIndex indexAt( const QPoint &point ) const;

  /*! Re-implemented from QAbstractItemView. See the Qt API documentation for details. */
  void scrollTo( const QModelIndex &index, ScrollHint hint = EnsureVisible );

  /*! Re-implemented from QAbstractItemView. See the Qt API documentation for details. */
  void setModel( QAbstractItemModel *model );

public slots:
  /*! Set chart label.
      Sets the text, font, colour and alignment of the label specified.
      @param label - the header, x-axis label or y-axis label. 
      \sa requestLabelDetails() and emitLabelDetails */
  void setLabelDetails( GobChartsLabel label, const QString &text, const QFont &font, const QColor &colour, Qt::Alignment align );

  /*! Request a specific label's settings.
      Calling this function will result in the emitLabelDetails() signal being emitted.  The signal's parameters
      contain all the information for the relevant label - text, font, colour and alignment.
      @param label - the header, x-axis label or y-axis label. 
      \sa setLabelDetails() and emitLabelDetails() */
  void requestLabelDetails( GobChartsLabel label );

  /*! Grid line style.
      Set the pen style used to draw the grid lines.
      @param style - dashed, dotted, solid, etc. (see Qt API documentation for details on the enum). */
  void setGridLineStyle( Qt::PenStyle style );

  /*! Set horizontal grid lines.
      Turn horizontal grid lines "on" or "off" and also sets the number of grid lines. */
  void setHorizontalGridLines( bool set, int number );

  /*! Set horizontal grid lines.
      Turn vertical grid lines "on" or "off" and also sets the number of grid lines. */
  void setVerticalGridLines( bool set, int number );

  /*! Set grid colour.
      Sets the grid line's colour to "colour". */
  void setGridColour( QColor colour );

  /*! Fix the chart colour.
      Fix the chart colour to "colour".  This means that the different data categories will all be
      displayed in "colour" as opposed to having randomly generated (different) colours. 
      \sa setRandomColours(), useFixedColour() and fixedColour() */
  void setFixedColour( QColor colour );

  /*! Use different colours.
      Re-enables random colour generation (the default) for chart items after a call to setFixedColour().
      \sa setFixedColour(), useFixedColour() and fixedColour() */
  void setRandomColours();

  /*! Limit data range.
      Setting the allowed data range determines which categories will be shown in the final chart (i.e. only those
      data categories with values within the range will be shown).  By default the entire value range is drawn. 
      @param lowerBoundary - the lowest allowed value to be displayed.
      @param upperBoundary - the highest allowed value to be displayed.
      \sa setShowTotalRange() and isWithinAllowedRange() */
  void setAllowedDataRange( qreal lowerBoundary, qreal upperBoundary );

  /*! Show total data range.
      Displays all categories (i.e. the entire data value range).
      \sa setAllowedDataRange() and isWithinAllowedRange() */
  void setShowTotalRange();

  /*! Debug log.
      Turn debug logging messages "on" or "off" (default "off").
      \sa lastDebugLogMsg() */
  void setDebugLoggingOn( bool logging );

signals:
  /*! Debug messages.
      This signal broadcasts the last debug log message and will only be emitted when debug logging is turned "on".
      \sa setDebugLoggingOn() */
  void lastDebugLogMsg( QString );

  /*! Obtain label details.
      This signal is emitted when requestLabelDetails() is called and contains all the information relevant to the specified label.
      \sa requestLabelDetails() and setLabelDetails() */
  void emitLabelDetails( GobChartsLabel label, const QString &text, const QFont &font, const QColor &colour, Qt::Alignment align );

  /*! Emitted when a new graphics item was created.
      This signal is emitted shortly after a graphics item is created and contains the details for the
      corresponding legend item. */
  void createLegendItem( const QColor &colour, const QString &text );

  /*! Emitted when a chart is redrawn and its graphics items are reset. */
  void clearLegend();

  /*! Emitted when a graphics item is selected. */
  void highLightLegendItem( const QString &text );

protected:
  /*! Expected data model columns. 
      GobChartsWidget is only capable of displaying one to one mappings between categories and data values and expects
      the first column of the data model to contain the chart's category names and the second the corresponding data values. */
  enum GobChartsColumn { CATEGORY, VALUE };

  //! Destructor.
  explicit GobChartsView( QWidget *parent = 0 );

  /*! Derived classes must implement this function.
      This function must create the graphics items required for the specific chart type 
      and position them within the confines of the view. */
  virtual void generateGraphicsItems() = 0;

  /*! Derived classes must implement this function.
      This function must return "true" when the chart type supports grid lines and "false"
      otherwise (e.g. PIE charts don't support chart grids whereas BAR charts do). */
  virtual bool needsGrid() const = 0;

  /*! Derived classes must implement this function.
      This function must return an integer value that matches one of the GobChartsType enum
      values as a QString (this information is used when saving and loading charts to and
      from file). */
  virtual QString typeInteger()  const = 0;

  /*! Returns "true" if the chart colour is fixed or "false" if random colours must be generated.
      \sa setFixedColour(), setRandomColours and fixedColour() */
  bool useFixedColour() const;

  /*! If the chart colour is fixed, this function returns the colour used to draw the chart items.
      \sa setFixedColour(), setRandomColours and useFixedColour() */
  QColor fixedColour() const;

  /*! Stores dynamically created graphics items. This function adds newly created graphics items 
      to a map so that they can be retrieved based on their corresponding model indices later on.
      @param valueIndex - the QModelIndex that the graphics item represents.
      @param item - the newly created graphics item. */
  void addToGraphItemsContainer( const QModelIndex &valueIndex, QGraphicsItem *item, const QString &legendText );

  /*! Returns the number of valid rows in the data model.  "Valid rows" are rows defined as those containing
      chart categories or data values and excludes empty rows).
      \sa validRowList(), category() and value() */
  int nrValidItems() const;

  /*! Returns a list of valid data row numbers.
      \sa nrValidItems(), category() and value() */
  QList< int > validRowList() const;

  /*! Returns the category name corresponding to "row" in validRowList()
      \sa nrValidItems() and value() */
  QString category( int row ) const;

  /*! Returns the data value corresponding to "row" in validRowList()
      \sa nrValidItems() and category() */
  qreal value( int row ) const;

  /*! Returns the rectangle corresponding to the "inner scene".  The "inner scene" is the rectangle
      defined as the space within which the chart items must fit.  Within the context of the entire
      view, this means that the space taken up by labels is excluded. */
  const QRectF &innerSceneRectF() const;

  /*! Determines whether or not a data value falls within the allowed data range. 
      \sa setAllowedDataRange() and setShowTotalRange() */
  bool  isWithinAllowedRange( qreal value ) const;

  /*! Returns the grid width.  Within the context of the allowed space, this is the width that the grid may occupy. */
  qreal gridWidth() const;

  /*! Calculates the amount of space at the top of the view that is "extra".  This extra space can be
      stripped from all item height calculations and is needed to ensure that as much space is 
      used for the display of the chart as possible. */
  qreal stripSpace( qreal perc ) const;

  /*! Returns the total, consolidated value of all category values. */
  qreal totalValue() const;

  /*! Re-implemented from QAbstractItemView. See the Qt API documentation for details. */
  QModelIndex moveCursor( QAbstractItemView::CursorAction cursorAction, Qt::KeyboardModifiers modifiers );

  /*! Re-implemented from QAbstractItemView. See the Qt API documentation for details. */
  QRegion visualRegionForSelection( const QItemSelection &selection ) const;

  /*! Re-implemented from QAbstractItemView. See the Qt API documentation for details. */
  int  horizontalOffset() const;

  /*! Re-implemented from QAbstractItemView. See the Qt API documentation for details. */
  int  verticalOffset() const;

  /*! Re-implemented from QAbstractItemView. See the Qt API documentation for details. */
  bool isIndexHidden( const QModelIndex &index ) const;

  /*! Re-implemented from QAbstractItemView. See the Qt API documentation for details. */
  void setSelection( const QRect& rect, QItemSelectionModel::SelectionFlags command );

  /*! Re-implemented from QAbstractItemView. See the Qt API documentation for details. */
  void keyPressEvent( QKeyEvent *event );

protected slots:
  /*! Log debug messages. This slot receives logging information from privately owned objects. */ 
  void debugLog( QString msg );

  /*! Re-implemented from QAbstractItemView. See the Qt API documentation for details. */
  void dataChanged( const QModelIndex &topLeft, const QModelIndex &bottomRight );

  /*! Re-implemented from QAbstractItemView. See the Qt API documentation for details. */
  void rowsInserted( const QModelIndex &parent, int start, int end );

  /*! Re-implemented from QAbstractItemView. See the Qt API documentation for details. */
  void rowsAboutToBeRemoved( const QModelIndex &parent, int start, int end );

  /*! Re-implemented from QAbstractItemView. See the Qt API documentation for details. */
  void mousePressEvent( QMouseEvent *p_event );

private:
  /*! The PIMPL idiom will help with future binary compatibility. Since the
     GobChartsViewPrivate class is completely restricted to this class, the
     "friend" status does not cause any adverse coupling (the class is basically
     a container of all the data members that would have been declared as "private"
     anyway, with one or two utility methods thrown in). */
  friend class GobChartsViewPrivate;
  class GobChartsViewPrivate;
  GobChartsViewPrivate* const m_private;

  /*! Re-implemented from QAbstractItemView. See the Qt API documentation for details. */
  void resizeEvent( QResizeEvent *event );

private slots:
  /*! Sets the header or label item that's supposed to receive keyboard input. */
  void setSelectedTextItem( const QString &itemName );
};

#endif // GOBCHARTSVIEW_H

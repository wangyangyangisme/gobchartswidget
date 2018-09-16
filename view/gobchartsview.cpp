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

#include "gobchartsview.h"
#include "label/gobchartstextitem.h"
#include "utils/gobchartscolours.h"
#include "utils/gobchartsgrid.h"
#include "utils/gobchartsgraphitems.h"
#include "utils/gobchartsvaliditems.h"

#include <QtCore/qmath.h>
#include <QGraphicsView>
#include <QVBoxLayout>
#include <QDomDocument>

/*--------------------------------------------------------------------------------*/

/* Percentages to be used for header and label text positioning. */

/* PERC_HEADER_SPACE + PERC_HEADER_TOP <= 1.0 */
const qreal PERC_HEADER_SPACE      = 0.7;   // of total Y margin
const qreal PERC_HEADER_TOP        = 0.2;   // from top of scene

/* PERC_XLABEL_SPACE + ( 1.0 - PERC_XLABEL_BOTTOM ) <= 1.0 */
const qreal PERC_XLABEL_SPACE      = 0.7;   // of total Y margin
const qreal PERC_XLABEL_BOTTOM     = 0.8;   // from bottom of the scene

/* PERC_YLABEL_SPACE + PERC_YLABEL_LEFT <= 1.0 */
const qreal PERC_YLABEL_SPACE      = 0.7;   // of total X margin
const qreal PERC_YLABEL_LEFT       = 0.2;   // from left-hand side of the scene

const qreal LEFT_RIGHT_MARGIN_PERC = 0.15;  // of total width
const qreal TOP_BOTTOM_MARGIN_PERC = 0.15;  // of total height

/* Arbitrary number selected on the basis of the resulting cosmetic appearance. */
const qreal STRIPSPACE_OFFSET      = 0.05;



/*--------------------------------- PIMPL CLASS ----------------------------------*/

class GobChartsView::GobChartsViewPrivate
{
public:

  /* Emits the lastDebugLogMsg signal if logging is turned on. */
  inline void emitDebugLogMsg( QString msg )
  {
    if( m_loggingOn )
    {
      emit m_gobChartsView->lastDebugLogMsg( msg );
    }
  }

  /*--------------------------------------------------------------------------------*/

  /* Calculates the total for all active rows and also determines which value is the maximum value. */
  void calculateActiveTotals( int end )
  {
    m_totalValue = 0.0;
    m_maxValue   = 0.0;
    m_maxRow     = 0;
    m_validItems->clear();

    bool toDoubleOK = true;

    for( int row = 0; row < end; row++ )
    {
      QString cat = m_gobChartsView->model()->data( m_gobChartsView->model()->index( row, CATEGORY ) ).toString().trimmed();
      QString val = m_gobChartsView->model()->data( m_gobChartsView->model()->index( row, VALUE ) ).toString().trimmed();
      qreal   value;

      /* Check for empty entries */
      if( !cat.isEmpty() || !val.isEmpty() )
      {
        if( cat.isEmpty() )
        {
          cat = QString( "Uncategorised" );
          emitDebugLogMsg( tr( "GobChartsView::calculateActiveTotals# No category provided for row [%1]." ).arg( row + 1 ) );
        }

        if( val.isEmpty() )
        {
          value = 0.0;
          emitDebugLogMsg( tr( "GobChartsView::calculateActiveTotals# No value provided for row [%1], defaulting to \"0.0\"." ).arg( row + 1 ) );
        }
        else
        {
          value = val.toDouble( &toDoubleOK );

          if( toDoubleOK )
          {
            if ( value < 0.0)
            {
              value = 0.0;
              emitDebugLogMsg( tr( "GobChartsView::calculateActiveTotals# Value provided for row [%1] is negative, defaulting to \"0.0\"." ).arg( row + 1 ) );
            }

            m_totalValue += value;
            m_maxValue = ( value > m_maxValue ) ? value : m_maxValue;
          }
          else
          {
            toDoubleOK = true;   // reset for next iteration
            emitDebugLogMsg( tr( "GobChartsView::calculateActiveTotals# Failed to convert the data value of row [%1] to \"double\"" ).arg( row + 1 ) );
          }
        }

        m_validItems->addValidItem( row, cat, value );
      }

      m_maxRow++;
    }
  }

  /*--------------------------------------------------------------------------------*/

  /* Calculates and sets all the chart's dimensions and allowed areas. */
  void calculateGeometries()
  {
    m_leftRightMargin = m_gobChartsView->rect().width()  * LEFT_RIGHT_MARGIN_PERC;
    m_topBottomMargin = m_gobChartsView->rect().height() * TOP_BOTTOM_MARGIN_PERC;

    m_graphScene->setSceneRect( QRectF( m_gobChartsView->rect() ) );
    m_graphicsView->setSceneRect( QRectF( m_gobChartsView->rect() ) );

    m_innerSceneRectF.setRect( m_gobChartsView->rect().x() + m_leftRightMargin,
                               m_gobChartsView->rect().y() + m_topBottomMargin,
                               m_gobChartsView->rect().width()  - 2 * m_leftRightMargin,
                               m_gobChartsView->rect().height() - 2 * m_topBottomMargin );

    m_grid->setGridRectF( QRectF( m_innerSceneRectF.left(),
                                  m_innerSceneRectF.top(),
                                  m_innerSceneRectF.width(),
                                  m_innerSceneRectF.height() ) );

    m_header->setRectF( QRectF( QPointF( m_innerSceneRectF.left(), m_topBottomMargin * PERC_HEADER_TOP ),
                                QSizeF( m_innerSceneRectF.width(), m_topBottomMargin * PERC_HEADER_SPACE ) ) );

    m_xLabel->setRectF( QRectF( QPointF( m_innerSceneRectF.left(), m_gobChartsView->rect().height() - m_topBottomMargin * PERC_XLABEL_BOTTOM ),
                                QSizeF( m_innerSceneRectF.width(), m_topBottomMargin * PERC_XLABEL_SPACE ) ) );

    m_yLabel->setRectF( QRectF( QPointF( m_leftRightMargin * PERC_YLABEL_LEFT, m_innerSceneRectF.top() ),
                                QSizeF( m_leftRightMargin * PERC_YLABEL_SPACE, m_innerSceneRectF.height() ) ) );
  }

  /*--------------------------------------------------------------------------------*/

  QString labelText( GobChartsLabel label )
  {
    if( m_labels.contains( label ) )
    {
      return m_labels.value( label )->toPlainText();
    }

    return  QString( "" );
  }

  /*--------------------------------------------------------------------------------*/

  QFont labelFont( GobChartsLabel label )
  {
    if( m_labels.contains( label ) )
    {
      return m_labels.value( label )->font();
    }

    return QFont();
  }

  /*--------------------------------------------------------------------------------*/

  QColor labelColour( GobChartsLabel label )
  {
    if( m_labels.contains( label ) )
    {
      return m_labels.value( label )->defaultTextColor();
    }

    return QColor();
  }

  /*--------------------------------------------------------------------------------*/

  Qt::Alignment labelAlignment( GobChartsLabel label )
  {
    if( m_labels.contains( label ) )
    {
      return m_labels.value( label )->alignment();
    }

    return Qt::AlignHCenter;
  }

  /*--------------------------------------------------------------------------------*/

  GobChartsTextItem *labelGraphicsItem( GobChartsLabel label )
  {
    if( m_labels.contains( label ) )
    {
      return const_cast< GobChartsTextItem* >( m_labels.value( label ) );
    }

    /* We really shouldn't ever get here...but rather safe than sorry. */
    return m_header;
  }

  /*--------------------------------------------------------------------------------*/

  /* Constructor and destructor. */
  GobChartsViewPrivate( GobChartsView * view )
    :
    m_gobChartsView    ( view ),
    m_graphicsView     ( new QGraphicsView ),
    m_graphScene       ( new QGraphicsScene ),
    m_header           ( new GobChartsTextItem( Qt::Horizontal, "HEADER" ) ),
    m_yLabel           ( new GobChartsTextItem( Qt::Vertical,   "YLABEL" ) ),
    m_xLabel           ( new GobChartsTextItem( Qt::Horizontal, "XLABEL" ) ),
    m_graphItems       ( new GobChartsGraphItems ),
    m_grid             ( new GobChartsGrid ),
    m_validItems       ( new GobChartsValidItems ),
    m_selectedLabel    ( NONE ),
    m_innerSceneRectF  (),
    m_fixedColour      (),
    m_legendText       ( "" ),
    m_leftRightMargin  ( 0.0 ),
    m_topBottomMargin  ( 0.0 ),
    m_maxValue         ( 0.0 ),    
    m_lowerDataBoundary( 0.0 ),
    m_upperDataBoundary( 0.0 ),
    m_totalValue       ( 0.0 ),
    m_maxRow           ( 0 ),
    m_showTotalRange   ( true ),
    m_loggingOn        ( false ),
    m_fixedColourOn    ( false ),
    m_chartIsLoading   ( false )
  {
    m_graphScene->setBackgroundBrush( QBrush( QColor( 245,245,245 ) ) );

    m_graphScene->addItem( m_header );  // takes ownership
    m_graphScene->addItem( m_xLabel );  // takes ownership
    m_graphScene->addItem( m_yLabel );  // takes ownership

    m_graphicsView->setScene( m_graphScene );
    m_graphicsView->setResizeAnchor( QGraphicsView::AnchorViewCenter );
    m_graphicsView->setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
    m_graphicsView->setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOff );

    /* Convenience mapping. */
    m_labels.insert( HEADER, m_header );
    m_labels.insert( YLABEL, m_yLabel );
    m_labels.insert( XLABEL, m_xLabel );

    m_labelNames.insert( "HEADER", HEADER );
    m_labelNames.insert( "YLABEL", YLABEL );
    m_labelNames.insert( "XLABEL", XLABEL );
  }

  ~GobChartsViewPrivate()
  {
    m_graphItems->removeItemsFromScene( m_graphScene );
    m_grid->removeGridFromScene( m_graphScene );

    delete m_graphicsView;
    delete m_graphScene;
    delete m_graphItems;
    delete m_grid;
    delete m_validItems;
  }

  GobChartsView       *m_gobChartsView;       // pointer to the object to which we have friend access to
  QGraphicsView       *m_graphicsView;
  QGraphicsScene      *m_graphScene;
  GobChartsTextItem   *m_header;
  GobChartsTextItem   *m_yLabel;
  GobChartsTextItem   *m_xLabel;
  GobChartsGraphItems *m_graphItems;
  GobChartsGrid       *m_grid;
  GobChartsValidItems *m_validItems;
  GobChartsLabel       m_selectedLabel;       // to keep track of the selected text item to ensure the correct item receives the keyboard input
  QRectF               m_innerSceneRectF;
  QColor               m_fixedColour;
  QString              m_legendText;
  qreal                m_leftRightMargin;
  qreal                m_topBottomMargin;
  qreal                m_maxValue;
  qreal                m_lowerDataBoundary;
  qreal                m_upperDataBoundary;
  qreal                m_totalValue;
  int                  m_maxRow;              // the last row containing valid items
  bool                 m_showTotalRange;
  bool                 m_loggingOn;
  bool                 m_fixedColourOn;
  bool                 m_chartIsLoading;

  /* Convenience mappings to rid us of all the "switch" statements required otherwise. */
  QMap< GobChartsLabel, GobChartsTextItem* > m_labels;
  QMap< QString,        GobChartsLabel     > m_labelNames;
};


/*------------------------------- MEMBER FUNCTIONS -------------------------------*/

GobChartsView::GobChartsView( QWidget *parent ) :
  QAbstractItemView( parent ),
  m_private        ( new GobChartsViewPrivate( this ) )
{
  setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
  setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOff );

  m_private->calculateGeometries();

  /* Labels. */
  connect( m_private->m_header, SIGNAL( identity           ( const QString& ) ),
                          this, SLOT  ( setSelectedTextItem( const QString& ) ) );

  connect( m_private->m_xLabel, SIGNAL( identity           ( const QString& ) ),
                          this, SLOT  ( setSelectedTextItem( const QString& ) ) );

  connect( m_private->m_yLabel, SIGNAL( identity           ( const QString& ) ),
                          this, SLOT  ( setSelectedTextItem( const QString& ) ) );

  /* Debug logging. */
  connect( m_private->m_graphItems, SIGNAL( lastDebugLogMsg( QString ) ), this, SLOT( debugLog( QString ) ) );
  connect( m_private->m_grid,       SIGNAL( lastDebugLogMsg( QString ) ), this, SLOT( debugLog( QString ) ) );

  QVBoxLayout *layout = new QVBoxLayout;
  layout->addWidget( m_private->m_graphicsView );
  setLayout( layout );

  setViewport( m_private->m_graphicsView );
}

/*--------------------------------------------------------------------------------*/

GobChartsView::~GobChartsView() 
{
  delete m_private;
}

/*--------------------------------------------------------------------------------*/

void GobChartsView::setLabelDetails( GobChartsLabel label, const QString &text, const QFont &font, const QColor &colour, Qt::Alignment align )
{
  if( m_private->m_labels.contains( label ) )
  {
    /* Values returned from the map aren't modifiable by default. */
    const_cast< GobChartsTextItem* >( m_private->m_labels.value( label ) )->setPlainText( text );
    const_cast< GobChartsTextItem* >( m_private->m_labels.value( label ) )->setMaxFontSize( font );
    const_cast< GobChartsTextItem* >( m_private->m_labels.value( label ) )->setAlignment( align );
    const_cast< GobChartsTextItem* >( m_private->m_labels.value( label ) )->setDefaultTextColor( colour );
  }
}

/*--------------------------------------------------------------------------------*/

void GobChartsView::requestLabelDetails( GobChartsLabel label )
{
  if( m_private->m_labels.contains( label ) )
  {
    m_private->m_selectedLabel = label;
    emit emitLabelDetails( label, m_private->labelText( label ), m_private->labelFont( label ), m_private->labelColour( label ), m_private->labelAlignment( label ) );

    m_private->m_graphScene->clearFocus();
    m_private->m_graphScene->setFocusItem( m_private->labelGraphicsItem( label ) );
  }
  else
  {
    m_private->m_selectedLabel = NONE;
  }
}

/*--------------------------------------------------------------------------------*/

void GobChartsView::setSelectedTextItem( const QString &itemName )
{
  requestLabelDetails( m_private->m_labelNames.value( itemName ) );
}

/*--------------------------------------------------------------------------------*/

void GobChartsView::setGridLineStyle( Qt::PenStyle style )
{
  m_private->m_grid->setGridLineStyle( style );
  drawChart();
}

/*--------------------------------------------------------------------------------*/

void GobChartsView::setHorizontalGridLines( bool set, int number )
{
  m_private->m_grid->setHorizontalGridLines( set, number );
  drawChart();
}

/*--------------------------------------------------------------------------------*/

void GobChartsView::setVerticalGridLines( bool set, int number )
{
  m_private->m_grid->setVerticalGridLines( set, number );
  drawChart();
}

/*--------------------------------------------------------------------------------*/

void GobChartsView::setGridColour( QColor colour )
{
  m_private->m_grid->setGridColour( colour );
  drawChart();
}

/*--------------------------------------------------------------------------------*/

void GobChartsView::drawChart()
{
  if( model() )
  {
    m_private->calculateGeometries();

    m_private->m_graphItems->removeItemsFromScene( m_private->m_graphScene );
    m_private->m_graphItems->deleteItems();

    if( needsGrid() )
    {
      m_private->m_grid->removeGridFromScene( m_private->m_graphScene );
      m_private->m_grid->constructGrid();                               // delete old lines and generate new ones
      m_private->m_grid->addGridToScene( m_private->m_graphScene );
    }

    GobChartsColours::resetColourIndex();
    generateGraphicsItems();

    m_private->m_graphItems->addItemsToScene( m_private->m_graphScene );
  }
  else
  {
    m_private->emitDebugLogMsg( tr( "GobChartsView::drawChart# No valid data model set." ) );
  }
}

/*--------------------------------------------------------------------------------*/

void GobChartsView::legendItemSelected( const QString &text )
{
  QRectF rectF = m_private->m_graphItems->getItemRectF( text );
  QRect rect = rectF.toRect();
  m_private->m_legendText = text;

  selectionModel()->setCurrentIndex( m_private->m_graphItems->getModelIndex( text ), QItemSelectionModel::NoUpdate );
  setSelection( rect, QItemSelectionModel::Select /* this flag isn't actually used */ );
}

/*--------------------------------------------------------------------------------*/

void GobChartsView::setStateXML( const QDomNode &viewNode, const QDomNode &dataNode, bool includeData )
{
  /* View related. */
  if( !viewNode.isNull() )
  {
    /* Header. */
    QDomNode label = viewNode.firstChildElement( "Header" );

    if( !m_private->m_header->setStateXML( label ) )
    {
      m_private->emitDebugLogMsg( tr( "GobChartsView::setStateXML# Header node is invalid or NULL." ) );;
    }

    /* Y Label. */
    label = viewNode.firstChildElement( "YLabel" );

    if( !m_private->m_yLabel->setStateXML( label ) )
    {
      m_private->emitDebugLogMsg( tr( "GobChartsView::setStateXML# Y Label node is invalid or NULL." ) );
    }

    /* X Label. */
    label = viewNode.firstChildElement( "XLabel" );

    if( !m_private->m_xLabel->setStateXML( label ) )
    {
      m_private->emitDebugLogMsg( tr( "GobChartsView::setStateXML# Y Label node is invalid or NULL." ) );
    }

    /* If the chart is not of a type that can have a grid, then it makes no sense
        to set the x or y labels either (e.g. PIE charts). */
    if( needsGrid() )
    {
      m_private->m_yLabel->setVisible( true );
      m_private->m_xLabel->setVisible( true );
    }
    else
    {
      m_private->m_yLabel->setVisible( false );
      m_private->m_xLabel->setVisible( false );
    }
  }
  else
  {
    m_private->emitDebugLogMsg( tr( "GobChartsView::setStateXML# View node is invalid or NULL." ) );
  }

  /* Data related. */
  if( includeData )
  {
    if( model() )
    {
      if( !dataNode.isNull() )
      {
        /* We don't want to execute the functionality in dataChanged() until we're done. */
        m_private->m_chartIsLoading = true;

        /* Clear all existing data from the model. */
        for( int row = 0; row < model()->rowCount(); row++ )
        {
          model()->setData( model()->index( row, CATEGORY ), QString() );
          model()->setData( model()->index( row, VALUE    ), QString() );
        }

        /* Add new data to model. */
        QDomElement itemElement = dataNode.firstChildElement( "Item" );
        int row( 0 );

        while( !itemElement.isNull() )
        {
          model()->setData( model()->index( row, CATEGORY ), itemElement.attribute( "category", "Uncategorised" ) );
          model()->setData( model()->index( row, VALUE ),    itemElement.attribute( "value", "0.0" ) );
          itemElement = itemElement.nextSiblingElement( "Item" );
          row++;
        }

        m_private->m_chartIsLoading = false;
        m_private->calculateActiveTotals( row );
      }
      else
      {
        m_private->emitDebugLogMsg( tr( "GobChartsView::setStateXML# Data node is invalid or NULL." ) );
      }
    }
    else
    {
      m_private->emitDebugLogMsg( tr( "GobChartsView::setStateXML# No data model associated with this view." ) );
    }
  }
}

/*--------------------------------------------------------------------------------*/

QString GobChartsView::getStateXML( bool includeData ) const
{
  /* Tried returning a QDomDocumentFragment but couldn't get it to work
    so decided on manually creating the XML here as it is pretty straightforward. */
  QString xml;
  xml += "<View>";

  xml += "<Header>";
  xml += m_private->m_header->getStateXML();
  xml += "</Header>";

  xml += "<YLabel>";
  xml += m_private->m_yLabel->getStateXML();
  xml += "</YLabel>";

  xml += "<XLabel>";
  xml += m_private->m_xLabel->getStateXML();
  xml += "</XLabel>";

  xml += "<ChartType value=\"" + typeInteger() + "\" />";
  xml += "</View>";

  if( includeData )
  {
    xml += "<Data>";

    foreach( int i, validRowList() )
    {
      xml += "<Item category=\"" + category( i ) + "\" value=\"" + QString( "%1" ).arg( value( i ) ) + "\" />";
    }

    xml += "</Data>";
  }

  return xml;
}

/*--------------------------------------------------------------------------------*/

/* The maximum value is used to determine the available "free space" at the top
  of the chart (BAR and LINE), i.e. the space that will not be entered into by any of the
  categories, so we'll strip this space out to maximise visual effect. */
qreal GobChartsView::stripSpace( qreal perc ) const
{
  if( m_private->m_totalValue > 0.0 )
  {
    /* I know, this calculation looks nasty, but it really isn't. */
    qreal maxStrip = m_private->m_innerSceneRectF.height() - ( ( m_private->m_maxValue/m_private->m_totalValue ) + STRIPSPACE_OFFSET ) * m_private->m_innerSceneRectF.height();
    return ( perc/( m_private->m_maxValue/m_private->m_totalValue ) ) * maxStrip;
  }

  return 0;
}

/*--------------------------------------------------------------------------------*/

void GobChartsView::setFixedColour( QColor colour ) 
{
  if( colour.isValid() )
  {
    m_private->m_fixedColour = colour;
    m_private->m_fixedColourOn = true;
  }
  else
  {
    m_private->emitDebugLogMsg( tr( "GobChartsView::setFixedColour# Invalid colour provided" ) );
    m_private->m_fixedColourOn = false;
  }

  drawChart();
}

/*--------------------------------------------------------------------------------*/

void GobChartsView::setRandomColours() 
{
  m_private->m_fixedColourOn = false;
  drawChart();
}

/*--------------------------------------------------------------------------------*/

bool GobChartsView::useFixedColour() const
{
  return m_private->m_fixedColourOn;
}

/*--------------------------------------------------------------------------------*/

QColor GobChartsView::fixedColour() const
{
  return m_private->m_fixedColour;
}

/*--------------------------------------------------------------------------------*/

void GobChartsView::setDebugLoggingOn( bool logging )
{
  m_private->m_loggingOn = logging;
  m_private->m_grid->setDebugLoggingOn( logging );
  m_private->m_graphItems->setDebugLoggingOn( logging );
}

/*--------------------------------------------------------------------------------*/

void GobChartsView::setAllowedDataRange( qreal  lowerBoundary, qreal  upperBoundary ) 
{
  m_private->m_lowerDataBoundary = lowerBoundary;
  m_private->m_upperDataBoundary = upperBoundary;
  m_private->m_showTotalRange = false;
  drawChart();
}

/*--------------------------------------------------------------------------------*/

void GobChartsView::setShowTotalRange() 
{
  m_private->m_showTotalRange = true;
  drawChart();
}

/*--------------------------------------------------------------------------------*/

bool GobChartsView::isWithinAllowedRange( qreal value ) const
{
  if( m_private->m_showTotalRange )
  {
    return true;
  }
  else
  {
    return ( value >= m_private->m_lowerDataBoundary ) && ( value <= m_private->m_upperDataBoundary );
  }
}

/*--------------------------------------------------------------------------------*/

void GobChartsView::addToGraphItemsContainer( const QModelIndex &valueIndex, QGraphicsItem *item, const QString &legendText )
{
  m_private->m_graphItems->addItem( valueIndex, item, legendText );
}

/*--------------------------------------------------------------------------------*/

qreal GobChartsView::gridWidth() const
{
  return m_private->m_grid->gridWidth();
}

/*--------------------------------------------------------------------------------*/

int GobChartsView::nrValidItems() const
{
  return m_private->m_validItems->size();
}

/*--------------------------------------------------------------------------------*/

QList<int> GobChartsView::validRowList() const
{
  return m_private->m_validItems->validRows();
}

/*--------------------------------------------------------------------------------*/

QString GobChartsView::category( int row ) const
{
  return m_private->m_validItems->category( row );
}

/*--------------------------------------------------------------------------------*/

qreal GobChartsView::value( int row ) const
{
  return m_private->m_validItems->data( row );
}

/*--------------------------------------------------------------------------------*/

const QRectF& GobChartsView::innerSceneRectF() const
{
  return m_private->m_innerSceneRectF;
}

/*--------------------------------------------------------------------------------*/

qreal GobChartsView::totalValue() const
{
  return m_private->m_totalValue;
}

/*--------------------------------------------------------------------------------*/

void GobChartsView::debugLog( QString msg )
{
  m_private->emitDebugLogMsg( msg );
}



/*---------------------- RE-IMPLEMENTED FROM QABSTRACTITEMVIEW -------------------*/

void GobChartsView::resizeEvent( QResizeEvent *event )
{
  QAbstractItemView::resizeEvent( event );
  drawChart();
}

/*--------------------------------------------------------------------------------*/

void GobChartsView::dataChanged( const QModelIndex &topLeft, const QModelIndex &bottomRight )
{
  QAbstractItemView::dataChanged( topLeft, bottomRight );

  if( !m_private->m_chartIsLoading )
  {
    int rowCount = ( ( bottomRight.row() + 1 ) > m_private->m_maxRow ) ? ( bottomRight.row() + 1 ) : m_private->m_maxRow;
    m_private->calculateActiveTotals( rowCount );
    drawChart();
  }
}

/*--------------------------------------------------------------------------------*/

void GobChartsView::rowsInserted( const QModelIndex &parent, int start, int end )
{
  QAbstractItemView::rowsInserted( parent, start, end );
  int rowCount = ( ( end + 1 ) > m_private->m_maxRow ) ? ( end + 1 ) : m_private->m_maxRow;
  m_private->calculateActiveTotals( rowCount );
}

/*--------------------------------------------------------------------------------*/

void GobChartsView::rowsAboutToBeRemoved( const QModelIndex &parent, int start, int end )
{
  QAbstractItemView::rowsAboutToBeRemoved( parent, start, end );
  int rowCount = ( ( end + 1 ) > m_private->m_maxRow ) ? ( end + 1 ) : m_private->m_maxRow;
  m_private->calculateActiveTotals( rowCount - ( end - start ) );
}

/*--------------------------------------------------------------------------------*/

QRect GobChartsView::visualRect( const QModelIndex &index ) const
{            
  if( !m_private->m_graphItems->getItemRectF( index ).isNull() )
  {
    QRectF rect = m_private->m_graphItems->getItemRectF( index );
    return rect.toRect();
  }

  return m_private->m_graphicsView->rect();
}

/*--------------------------------------------------------------------------------*/

void GobChartsView::scrollTo( const QModelIndex &index, ScrollHint hint )
{
  Q_UNUSED( index );
  Q_UNUSED( hint );
}

/*--------------------------------------------------------------------------------*/

void GobChartsView::setModel( QAbstractItemModel *model )
{
  QAbstractItemView::setModel( model );

  if( model )
  {
    m_private->calculateActiveTotals( model->rowCount() );
  }
  else
  {
    m_private->emitDebugLogMsg( tr( "GobChartsView::setModel# Attempting to set a NULL model on this view." ) );
  }
}

/*--------------------------------------------------------------------------------*/

QModelIndex GobChartsView::indexAt( const QPoint &point ) const 
{
  if( m_private->m_graphItems->getModelIndex( m_private->m_graphScene->itemAt( point) ).isValid() )
  {
    return m_private->m_graphItems->getModelIndex( m_private->m_graphScene->itemAt( point ) );
  }

  return QModelIndex();
}

/*--------------------------------------------------------------------------------*/

QModelIndex GobChartsView::moveCursor( QAbstractItemView::CursorAction cursorAction, Qt::KeyboardModifiers modifiers )
{
  Q_UNUSED( modifiers );

  QModelIndex current = currentIndex();

  switch ( cursorAction )
  {
  case MoveLeft:
    /* Deliberate fall-through. */
  case MoveUp:
    if( current.row() > 0 )
    {
      current = model()->index( current.row() - 1, current.column(), rootIndex() );
    }
    else
    {
      current = model()->index( 0, current.column(), rootIndex() );
    }
    break;
  case MoveRight:
    /* Deliberate fall-through. */
  case MoveDown:
    if( current.row() < model()->rowCount( model()->parent( current ) ) - 1 )
    {
      current = model()->index( current.row() + 1, current.column(), rootIndex() );
    }
    else
    {
      current = model()->index( model()->rowCount( model()->parent( current ) ) - 1,
                                current.column(), rootIndex() );
    }
    break;
  default:
    break;
  }

  drawChart();
  return current;
}

/*--------------------------------------------------------------------------------*/

int GobChartsView::horizontalOffset() const 
{
  return 0;
}

/*--------------------------------------------------------------------------------*/

int GobChartsView::verticalOffset() const 
{
  return 0;
}

/*--------------------------------------------------------------------------------*/

bool GobChartsView::isIndexHidden( const QModelIndex &index) const 
{
  Q_UNUSED( index );
  return false;
}

/*--------------------------------------------------------------------------------*/

void GobChartsView::setSelection( const QRect &rect, QItemSelectionModel::SelectionFlags command )
{
  Q_UNUSED( command );

  if( selectionModel() )
  {
    int firstRow    = m_private->m_maxRow;
    int lastRow     = 0;
    int firstColumn = 1;
    int lastColumn  = 0;

    QList< QGraphicsItem* > itemList = m_private->m_graphScene->items( rect );

    /* We don't need or want multiple selections to be made. */
    if( itemList.size() == 1 )
    {
      foreach( QGraphicsItem* itemIt, itemList )
      {
        QModelIndex index = m_private->m_graphItems->getModelIndex( itemIt );

        m_private->m_legendText = m_private->m_graphItems->getItemLegend( itemIt );
        emit highLightLegendItem( m_private->m_legendText );

        if( index.isValid() )
        {
          m_private->m_graphItems->setSelected( index.row() );     //highlight graph item corresponding to selection
          firstRow    = qMin( firstRow, index.row() );
          lastRow     = qMax( lastRow, index.row() );
          firstColumn = qMin( firstColumn, index.column() );
          lastColumn  = qMax( firstColumn, index.column() );
        }
      }

      QItemSelection selection( model()->index( firstRow, firstColumn, rootIndex() ),
                                model()->index( lastRow,  lastColumn,  rootIndex() ) );

      selectionModel()->select( selection, QItemSelectionModel::ClearAndSelect );

    }
    else
    {
      QModelIndex noIndex = m_private->m_graphItems->getModelIndex( m_private->m_legendText );
      QItemSelection selection( noIndex, noIndex );
      selectionModel()->select( selection, QItemSelectionModel::ClearAndSelect );
      m_private->m_graphItems->clearSelection();
    }
  }
  else
  {
    m_private->emitDebugLogMsg( tr( "GobChartsView::setModel# No selection model associated with this view." ) );
  }
}

/*--------------------------------------------------------------------------------*/

void GobChartsView::mousePressEvent( QMouseEvent *event ) 
{
  QAbstractItemView::mousePressEvent( event );
  m_private->m_selectedLabel = NONE;          // reset if selected item isn't a label or header
}

/*--------------------------------------------------------------------------------*/

void GobChartsView::keyPressEvent( QKeyEvent *event ) 
{
  if( m_private->m_selectedLabel == HEADER )
  {
    m_private->m_header->receiveKeyEvent( event );          // forward event to header
    requestLabelDetails( HEADER );
  }
  else if( m_private->m_selectedLabel == XLABEL )
  {
    m_private->m_xLabel->receiveKeyEvent( event );          // forward event to y label
    requestLabelDetails( XLABEL );
  }
  else if( m_private->m_selectedLabel == YLABEL )
  {
    m_private->m_yLabel->receiveKeyEvent( event );          // forward event to x label
    requestLabelDetails( YLABEL );
  }
  else
  {
    QAbstractItemView::keyPressEvent( event );
  }
}

/*--------------------------------------------------------------------------------*/

QRegion GobChartsView::visualRegionForSelection( const QItemSelection &selection ) const
{
  QRegion region;

  foreach( QModelIndex indexIt, selection.indexes() )
  {
    QRectF rect = m_private->m_graphItems->getItemRectF( indexIt );

    if( !rect.isNull() )
    {
      region += rect.toRect();
    }
  }

  return region;
}

/*--------------------------------------------------------------------------------*/

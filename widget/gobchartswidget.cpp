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

#include "gobchartswidget.h"
#include "view/gobchartsfactory.h"
#include "view/gobchartsview.h"
#include "toolswidget/gobchartstoolswidget.h"

#include <QtCore/qmath.h>
#include <QVBoxLayout>
#include <QListWidget>
#include <QSplitter>
#include <QMessageBox>
#include <QFileDialog>
#include <QTextStream>
#include <QDomDocument>
#include <QIcon>
#include <QPixmap>
#include <QPainter>

/*------------------------ NON-MEMBER HELPER FUNCTION ----------------------------*/

static QIcon RoundedIcon( const QColor& color )
{
  QPixmap pixMap( QSize( 16, 16 ) );
  pixMap.fill( QColor( 0, 0, 0, 0 ) );

  QPainterPath path;
  path.addRoundedRect( QRectF( pixMap.rect() ), 15, 45, Qt::RelativeSize );

  QPainter painter( &pixMap );
  painter.setRenderHint( QPainter::Antialiasing );
  painter.setPen( QPen( color, 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin ) );
  painter.setBrush( color );
  painter.drawPath( path );

  return QIcon( pixMap );
}

/*--------------------------------- PIMPL CLASS ----------------------------------*/

struct GobChartsWidget::GobChartsWidgetPrivate
{
  /* Constructor and destructor. */
  GobChartsWidgetPrivate() :
    m_gobChartsView     ( NULLPOINTER ),
    m_toolsWidget       ( new GobChartsToolsWidget ),
    m_legend            ( new QListWidget ),
    m_horizontalSplitter( new QSplitter ),
    m_verticalSplitter  ( new QSplitter ),
    m_frame             ( new QFrame ),
    m_model             ( NULLPOINTER ),
    m_selectionModel    ( NULLPOINTER ),
    m_loggingOn         ( false )
  {
    m_legend->setAlternatingRowColors( true );
    m_horizontalSplitter->addWidget( m_legend );   // chart view added later

    QVBoxLayout *frameLayout = new QVBoxLayout( m_frame );
    frameLayout->addWidget( m_horizontalSplitter );
    m_frame->setLayout( frameLayout );
    m_frame->setFrameStyle( QFrame::Box | QFrame::Sunken );

    m_verticalSplitter->setOrientation( Qt::Vertical );
    m_verticalSplitter->addWidget( m_toolsWidget );
    m_verticalSplitter->addWidget( m_frame );   // takes ownership
    m_verticalSplitter->setStretchFactor( 1, 1 );
  }

  ~GobChartsWidgetPrivate()
  {
    if( m_gobChartsView )
    {
      delete m_gobChartsView;
    }

    delete m_toolsWidget;
    delete m_legend;
    delete m_horizontalSplitter;
    delete m_verticalSplitter;

    /* Not owned:
      m_frame
      m_model
      m_selectionModel
    */
  }

  /* Members. */
  GobChartsView        *m_gobChartsView;
  GobChartsToolsWidget *m_toolsWidget;
  QListWidget          *m_legend;
  QSplitter            *m_horizontalSplitter;
  QSplitter            *m_verticalSplitter;
  QFrame               *m_frame;
  QAbstractItemModel   *m_model;           // model owned elsewhere
  QItemSelectionModel  *m_selectionModel;  // selection model owned elsewhere
  bool                  m_loggingOn;
};


/*------------------------------- MEMBER FUNCTIONS -------------------------------*/

GobChartsWidget::GobChartsWidget( QWidget *parent ) :
  QFrame   ( parent ),
  m_private( new GobChartsWidgetPrivate )
{
  connect( m_private->m_toolsWidget, SIGNAL( createChart( GobChartsType ) ),
                               this, SLOT  ( createChart( GobChartsType ) ) );

  connect( m_private->m_toolsWidget, SIGNAL( clearChart() ),
                               this, SLOT  ( clearChart() ) );

  connect( m_private->m_toolsWidget, SIGNAL( saveChart() ),
                               this, SLOT  ( saveChart() ) );

  connect( m_private->m_toolsWidget, SIGNAL( loadChart() ),
                               this, SLOT  ( loadChart() ) );

  connect( m_private->m_legend, SIGNAL( itemClicked( QListWidgetItem* ) ),
                          this, SLOT  ( legendItemSelected( QListWidgetItem* ) ) );

  /* Hide chart until created. */
  m_private->m_horizontalSplitter->setVisible( false );

  QVBoxLayout *mainLayout = new QVBoxLayout( this );
  mainLayout->addWidget( m_private->m_verticalSplitter );
  this->setLayout( mainLayout );
  this->setFrameStyle( QFrame::Box | QFrame::Sunken );
}

/*--------------------------------------------------------------------------------*/

GobChartsWidget::~GobChartsWidget()
{
  delete m_private;
}

/*--------------------------------------------------------------------------------*/

void GobChartsWidget::setDebugLoggingOn( bool log )
{
  if( m_private->m_gobChartsView )
  {
    m_private->m_gobChartsView->setDebugLoggingOn( log );
  }

  /* Remember setting for later when charts are created and/or recreated. */
  m_private->m_loggingOn = log;
}

/*--------------------------------------------------------------------------------*/

void GobChartsWidget::setModel( QAbstractItemModel *model )
{
  if( model )
  {
    m_private->m_model = model;
  }
}

/*--------------------------------------------------------------------------------*/

void GobChartsWidget::setSelectionModel( QItemSelectionModel *selectionModel )
{
  if( selectionModel )
  {
    m_private->m_selectionModel = selectionModel;
  }
}

/*--------------------------------------------------------------------------------*/

void GobChartsWidget::graphicsItemSelected( const QString &legendText )
{
  for( int i = 0; i < m_private->m_legend->count(); ++i )
  {
    QListWidgetItem *item = m_private->m_legend->item( i );

    if( legendText == item->text() )
    {
      m_private->m_legend->setCurrentItem( item );
      break;
    }
  }
}

/*--------------------------------------------------------------------------------*/

void GobChartsWidget::legendItemSelected( QListWidgetItem *item )
{
  if( m_private->m_gobChartsView )
  {
    m_private->m_gobChartsView->legendItemSelected( item->text() );
  }
}

/*--------------------------------------------------------------------------------*/

void GobChartsWidget::createLegendItem( const QColor &colour, const QString &text )
{
  QListWidgetItem *item = new QListWidgetItem( RoundedIcon( colour ), text, m_private->m_legend );
  Q_UNUSED( item );
}

/*--------------------------------------------------------------------------------*/

void GobChartsWidget::clearLegend()
{
  m_private->m_legend->clear();     // deletes items
}

/*--------------------------------------------------------------------------------*/

void GobChartsWidget::createChart( GobChartsType type )
{
  /* Save the label state before whacking the chart (GobChartsView
    is the only object that has access to all three labels' details). */
  QString labels( "" );

  if( m_private->m_gobChartsView )
  {
    labels = m_private->m_gobChartsView->getStateXML( false );

    m_private->m_gobChartsView->close();
    delete m_private->m_gobChartsView;
    m_private->m_gobChartsView = NULLPOINTER;
  }

  m_private->m_gobChartsView = GobChartsFactory::getInstance()->createChart( type, this );

  if( m_private->m_gobChartsView )
  {
    m_private->m_gobChartsView->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );

    connect( m_private->m_gobChartsView, SIGNAL( lastDebugLogMsg( QString ) ),
             this,                       SIGNAL( lastDebugLogMsg( QString ) ) );

    /* Data range. */
    connect( m_private->m_toolsWidget,   SIGNAL( setAllowedDataRange( qreal, qreal ) ),
             m_private->m_gobChartsView, SLOT  ( setAllowedDataRange( qreal, qreal ) ) );

    connect( m_private->m_toolsWidget,   SIGNAL( setShowTotalRange() ),
             m_private->m_gobChartsView, SLOT  ( setShowTotalRange() ) );

    /* Colour related. */
    connect( m_private->m_toolsWidget,   SIGNAL( setFixedColour( QColor ) ),
             m_private->m_gobChartsView, SLOT  ( setFixedColour( QColor ) ) );

    connect( m_private->m_toolsWidget,   SIGNAL( setRandomColours() ),
             m_private->m_gobChartsView, SLOT  ( setRandomColours() ) );

    /* Grid related. */
    connect( m_private->m_toolsWidget,   SIGNAL( setGridLineStyle( Qt::PenStyle ) ),
             m_private->m_gobChartsView, SLOT  ( setGridLineStyle( Qt::PenStyle ) ) );

    connect( m_private->m_toolsWidget,   SIGNAL( setHorizontalGridLines( bool, int ) ),
             m_private->m_gobChartsView, SLOT  ( setHorizontalGridLines( bool, int ) ) );

    connect( m_private->m_toolsWidget,   SIGNAL( setVerticalGridLines( bool, int ) ),
             m_private->m_gobChartsView, SLOT  ( setVerticalGridLines( bool, int ) ) );

    connect( m_private->m_toolsWidget,   SIGNAL( setGridColour( QColor) ),
             m_private->m_gobChartsView, SLOT  ( setGridColour( QColor ) ) );

    /* Label related. */
    connect( m_private->m_toolsWidget,   SIGNAL( setLabelDetails( GobChartsLabel,QString,QFont,QColor,Qt::Alignment ) ),
             m_private->m_gobChartsView, SLOT  ( setLabelDetails( GobChartsLabel,QString,QFont,QColor,Qt::Alignment ) ) );

    connect( m_private->m_toolsWidget,   SIGNAL( requestLabelDetails( GobChartsLabel ) ),
             m_private->m_gobChartsView, SLOT  ( requestLabelDetails( GobChartsLabel ) ) );

    connect( m_private->m_gobChartsView, SIGNAL( emitLabelDetails   ( GobChartsLabel,QString,QFont,QColor,Qt::Alignment ) ),
             m_private->m_toolsWidget,   SLOT  ( receiveLabelDetails( GobChartsLabel,QString,QFont,QColor,Qt::Alignment ) ) );

    /* Legend related. */
    connect( m_private->m_gobChartsView, SIGNAL( highLightLegendItem( QString) ),
             this,                       SLOT  ( graphicsItemSelected( QString ) ) );

    connect( m_private->m_gobChartsView, SIGNAL( createLegendItem( QColor, QString ) ),
             this,                       SLOT  ( createLegendItem( QColor, QString ) ) );

    connect( m_private->m_gobChartsView, SIGNAL( clearLegend() ),
             this,                       SLOT  ( clearLegend() ) );

    /* The previous state (if any) saved in the tools widget will be re-applied by emitting all
    the information via the signals above (with the exception of the label information). */
    m_private->m_toolsWidget->emitStateSignals();

    /* Update the labels with the saved data from above. */
    if( !labels.isEmpty() )
    {
      QDomDocument doc;

      /* If this fails, it isn't the end of the world. */
      if( doc.setContent( labels ) )
      {
        m_private->m_gobChartsView->setStateXML( doc.namedItem( "View" ), QDomNode(), false );
      }
    }

    m_private->m_gobChartsView->setDebugLoggingOn( m_private->m_loggingOn );

    if( m_private->m_model )
    {
      m_private->m_gobChartsView->setModel( m_private->m_model );
    }

    if( m_private->m_selectionModel )
    {
      m_private->m_gobChartsView->setSelectionModel( m_private->m_selectionModel );
    }

    /* Chart should get maximum space. */
    m_private->m_horizontalSplitter->insertWidget( 0, m_private->m_gobChartsView );
    m_private->m_horizontalSplitter->setStretchFactor( 0, 1 );
    m_private->m_verticalSplitter->setStretchFactor( 1, 1 );

    if( !m_private->m_horizontalSplitter->isVisible() )
    {
      QList< int > horList = QList< int >() << qFloor( static_cast< qreal >( rect().width() ) * 0.7 )
                                            << qFloor( static_cast< qreal >( rect().width() ) * 0.3 );

      m_private->m_horizontalSplitter->setSizes( horList );
      m_private->m_horizontalSplitter->setVisible( true );
    }

    m_private->m_gobChartsView->drawChart();
    m_private->m_gobChartsView->show();
  }
  else
  {
    QMessageBox::critical( this, tr( "Error" ), tr( "Unknown chart type selected." ) );
  }
}

/*--------------------------------------------------------------------------------*/

void GobChartsWidget::clearChart()
{
  m_private->m_horizontalSplitter->setVisible( false );

  if( m_private->m_gobChartsView )
  {
    m_private->m_gobChartsView->close();
    delete m_private->m_gobChartsView;
    m_private->m_gobChartsView = NULLPOINTER;
  }
}

/*--------------------------------------------------------------------------------*/

void GobChartsWidget::saveChart()
{
  if( m_private->m_gobChartsView )
  {
    QString fileName = QFileDialog::getSaveFileName( this, tr( "Save file as" ), QDir::homePath(), tr("XML files (*.xml)" ) );

    if( !fileName.isEmpty() )     // "Cancel" selected
    {
      QFile saveFile( fileName );

      if( saveFile.open( QIODevice::ReadWrite | QIODevice::Text | QIODevice::Truncate ) )
      {
        /* I tried any number of combinations of QDomDocumentFragment (from the getStateXML() functions in
          GobChartsView, GobChartsToolsWidget and GobChartsText) and QDomNode's and couldn't get it to work
          so decided on manually creating the XML here as it is pretty straightforward. */
        QString fileContent;
        fileContent += "<GobChart>";
        fileContent += m_private->m_gobChartsView->getStateXML();
        fileContent += m_private->m_toolsWidget->getStateXML();
        fileContent += "</GobChart>";

        QTextStream outStream( &saveFile );

        /* Check XML and apply "pretty print". */
        QDomDocument doc;

        if( doc.setContent( fileContent ) )
        {
          outStream << doc.toString( 2 );
        }
        else
        {
          QMessageBox::critical( this, tr( "Error" ), tr( "Failed to convert chart to required file format. XML is broken." ) );
        }

        saveFile.close();
      }
      else
      {
        QMessageBox::critical( this, tr( "Error" ), tr( "Failed to open file for saving." ) );
      }
    }
  }
  else
  {
    QMessageBox::information( this, tr( "Info" ), tr( "No active chart to save." ) );
  }
}

/*--------------------------------------------------------------------------------*/

void GobChartsWidget::loadChart()
{
  int returnButton = QMessageBox::question( this,
                                            "Warning",
                                            "This action will replace all current data.  Continue?",
                                            QMessageBox::Ok | QMessageBox::Cancel,
                                            QMessageBox::Cancel );

  if( returnButton == QMessageBox::Ok )
  {
    QString fileName = QFileDialog::getOpenFileName( this, tr( "Load file" ), QDir::homePath(), tr("XML files (*.xml)" ) );

    if( !fileName.isEmpty() )     // "Cancel" selected
    {
      QFile loadFile( fileName );

      if( loadFile.open( QIODevice::ReadOnly | QIODevice::Text ) )
      {
        QDomDocument doc;

        if( doc.setContent( &loadFile ) )
        {
          QString strVal = doc.firstChildElement( "GobChart").
                           firstChildElement    ( "View" ).
                           firstChildElement    ( "ChartType" ).
                           attribute            ( "value" );

          GobChartsType type = static_cast< GobChartsType >( strVal.toInt() );
          createChart( type );

          if( m_private->m_gobChartsView )
          {
            m_private->m_gobChartsView->setStateXML( doc.namedItem( "GobChart" ).namedItem( "View" ),
                                                     doc.namedItem( "GobChart" ).namedItem( "Data" ) );
          }

          m_private->m_toolsWidget->setStateXML( type, doc.namedItem( "GobChart" ).namedItem( "ToolsWidget" ) );
        }
        else
        {
          QMessageBox::critical( this, tr( "Error" ), tr( "Failed to load chart from file. XML is broken." ) );
        }

        loadFile.close();
      }
      else
      {
        QMessageBox::critical( this, tr( "Error" ), tr( "Failed to open file for loading." ) );
      }
    }
  }
}

/*--------------------------------------------------------------------------------*/

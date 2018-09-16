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

#include "gobchartstoolswidget.h"
#include "ui_gobchartstoolswidget.h"

#include <QButtonGroup>
#include <QColorDialog>
#include <QDomDocument>
#include <QMessageBox>

/*-------------------------------- FRIEND CLASS ----------------------------------*/

class GobChartsToolsWidget::LabelDetails
{
public:
  void setupLabel( const GobChartsToolsWidget * widget )
  {
    m_font = widget->ui->labelFontComboBox->currentFont();
    /* m_fontColour assigned in labelColourPreferenceChanged. */
    m_text = widget->ui->labelLineEdit->text();
    /* m_align assigned in labelAlignmentPreferenceChanged. */
    m_fontSize = widget->ui->labelFontSizeComboBox->currentText().toInt();
    m_bold = widget->ui->labelBoldButton->isChecked();
    m_italics = widget->ui->labelItalicsButton->isChecked();
    m_underlined = widget->ui->labelUnderlineButton->isChecked();

    m_font.setBold( m_bold );
    m_font.setItalic( m_italics );
    m_font.setUnderline( m_underlined );
    m_font.setPointSize( m_fontSize );
  }

  Qt::Alignment m_align;
  QFont         m_font;
  QColor        m_fontColour;
  QString       m_text;  
  int           m_fontSize;
  bool          m_bold;
  bool          m_italics;
  bool          m_underlined;
  bool          m_labelDetailsReceived;

  LabelDetails() :
    m_align     ( Qt::AlignCenter ),
    m_font      (),
    m_fontColour(),
    m_text      ( "" ),
    m_fontSize  ( 11 ),
    m_bold      ( false ),
    m_italics   ( false ),
    m_underlined( false ),
    m_labelDetailsReceived( false ) {}
};

/*--------------------------------- PIMPL STRUCT ---------------------------------*/

struct GobChartsToolsWidget::GobChartsToolsWidgetPrivate
{
  bool m_loadingChart;
  GobChartsToolsWidget::LabelDetails m_labelDetails;

  /* Chart colour. */
  QColor m_chartColour;
  bool   m_fixedColour;

  /* Data range. */
  qreal m_lowerBound;
  qreal m_upperBound;
  bool  m_showTotalRange;
  bool  m_rangePreferenceSet;

  /* Grid. */
  Qt::PenStyle m_penStyle;
  QColor       m_gridColour;
  bool         m_gridColourSet;
  bool         m_verticalGrid;
  bool         m_horizontalGrid;

  /* Convenience map for easy button/type look-ups. */
  QMap< GobChartsType, QToolButton* > m_typeButtonMap;

  /* These particular button groups need to have their "exclusive"
    settings changed and must therefore be accessible (which is
    why they're members). */
  QButtonGroup m_labelButtonGroup;
  QButtonGroup m_chartButtonGroup;

  GobChartsToolsWidgetPrivate() :
    m_loadingChart      ( false ),
    m_labelDetails      (),
    m_chartColour       (),
    m_fixedColour       ( false ),
    m_lowerBound        ( 0.0 ),
    m_upperBound        ( 0.0 ),
    m_showTotalRange    ( true ),
    m_rangePreferenceSet( false ),
    m_penStyle          ( Qt::DotLine ),
    m_gridColour        (),
    m_gridColourSet     ( false ),
    m_verticalGrid      ( false ),
    m_horizontalGrid    ( false ),
    m_typeButtonMap     ()
  {}
};


/*------------------------------- MEMBER FUNCTIONS -------------------------------*/

GobChartsToolsWidget::GobChartsToolsWidget( QWidget *parent ) :
  QFrame   ( parent ),
  ui       ( new Ui::GobChartsToolsWidget ),
  m_private( new GobChartsToolsWidgetPrivate )
{
  ui->setupUi( this );
  ui->chartsRangeGroupBox->setEnabled( false );

  /* Disable until a chart is created. */
  ui->labelTextGroupBox->setEnabled( false );
  ui->labelStyleGroupBox->setEnabled( false );
  ui->gridGroupBox->setEnabled( false );
  ui->saveButton->setEnabled( false );
  ui->chartClearButton->setEnabled( false );
  ui->chartColourPickerButton->setEnabled( false );
  ui->chartRangeToolButton->setEnabled( false );

  connect( ui->saveButton, SIGNAL( clicked() ), this, SIGNAL( saveChart() ) );
  connect( ui->loadButton, SIGNAL( clicked() ), this, SIGNAL( loadChart() ) );

  connect( ui->chartClearButton, SIGNAL( clicked() ), this, SLOT  ( clearAndReset() ) );
  connect( ui->chartClearButton, SIGNAL( clicked() ), this, SIGNAL( clearChart() ) );
  connect( ui->chartColourPickerButton, SIGNAL( clicked() ), this, SLOT( chartColourPreferenceChanged() ) );

  /* Data range related. */
  connect( ui->chartRangeToolButton, SIGNAL( clicked() ), this, SLOT( chartRangePreferenceChanged() ) );
  connect( ui->chartRangeLowerBoundarySpinBox, SIGNAL( valueChanged( double ) ), this, SLOT( chartRangePreferenceSelected() ) );
  connect( ui->chartRangeUpperBoundarySpinBox, SIGNAL( valueChanged( double ) ), this, SLOT( chartRangePreferenceSelected() ) );

  /* Chart type related. */
  m_private->m_chartButtonGroup.setExclusive( true );
  m_private->m_chartButtonGroup.addButton( ui->chartBarToolButton );
  m_private->m_chartButtonGroup.addButton( ui->chartPieToolButton );
  m_private->m_chartButtonGroup.addButton( ui->chartLineToolButton );
  connect( &m_private->m_chartButtonGroup, SIGNAL( buttonClicked( QAbstractButton* ) ), this, SLOT( chartTypeChanged( QAbstractButton* ) ) );

  m_private->m_typeButtonMap.insert( BAR,  ui->chartBarToolButton );
  m_private->m_typeButtonMap.insert( PIE,  ui->chartPieToolButton );
  m_private->m_typeButtonMap.insert( LINE, ui->chartLineToolButton );

  /* Grid related. */
  QButtonGroup *gridLineButtonGroup = new QButtonGroup( this );
  gridLineButtonGroup->setExclusive( true );
  gridLineButtonGroup->addButton( ui->gridDashedLineToolButton );
  gridLineButtonGroup->addButton( ui->gridDotLineToolButton );
  gridLineButtonGroup->addButton( ui->gridSolidLineToolButton );
  connect( gridLineButtonGroup, SIGNAL( buttonClicked( QAbstractButton* ) ), this, SLOT( gridLinePreferenceChanged( QAbstractButton* ) ) );

  connect( ui->gridHorizontalToolButton, SIGNAL( clicked() ), this, SLOT( gridLinesSelected() ) );
  connect( ui->gridVerticalToolButton, SIGNAL( clicked() ), this, SLOT( gridLinesSelected() ) );
  connect( ui->gridColourToolButton, SIGNAL( clicked() ), this, SLOT( gridColourSelected() ) );
  connect( ui->gridHorizontalSpinBox, SIGNAL( valueChanged( int ) ), this, SLOT( gridLineNumbersChanged() ) );
  connect( ui->gridVerticalSpinBox, SIGNAL( valueChanged( int ) ), this, SLOT( gridLineNumbersChanged() ) );

  /* Label related. */
  m_private->m_labelButtonGroup.setExclusive( true );
  m_private->m_labelButtonGroup.addButton( ui->labelHeaderToolButton );
  m_private->m_labelButtonGroup.addButton( ui->labelXToolButton );
  m_private->m_labelButtonGroup.addButton( ui->labelYToolButton );
  connect( &m_private->m_labelButtonGroup, SIGNAL( buttonClicked( QAbstractButton* ) ), this, SLOT( generateLabelDetailRequest( QAbstractButton* ) ) );

  QButtonGroup *labelAlignmentButtonGroup = new QButtonGroup( this );
  labelAlignmentButtonGroup->setExclusive( true );
  labelAlignmentButtonGroup->addButton( ui->labelAlignCentreButton );
  labelAlignmentButtonGroup->addButton( ui->labelAlignLeftButton );
  labelAlignmentButtonGroup->addButton( ui->labelAlignRightButton );
  connect( labelAlignmentButtonGroup, SIGNAL ( buttonClicked( QAbstractButton* ) ), this, SLOT( labelAlignmentPreferenceChanged( QAbstractButton* ) ) );

  connect( ui->labelFontComboBox, SIGNAL( currentIndexChanged( QString ) ), this, SLOT( updateLabelDetails() ) );
  connect( ui->labelFontSizeComboBox, SIGNAL( currentIndexChanged( QString ) ), this, SLOT( updateLabelDetails() ) );
  connect( ui->labelLineEdit, SIGNAL( textChanged( QString ) ), this, SLOT( updateLabelDetails() ) );
  connect( ui->labelBoldButton, SIGNAL( clicked() ), this, SLOT( updateLabelDetails() ) );
  connect( ui->labelItalicsButton, SIGNAL( clicked() ), this, SLOT( updateLabelDetails() ) );
  connect( ui->labelUnderlineButton, SIGNAL( clicked() ), this, SLOT( updateLabelDetails() ) );
  connect( ui->labelFontColourToolButton, SIGNAL( clicked() ), this, SLOT( labelColourPreferenceChanged() ) );
}

/*--------------------------------------------------------------------------------*/

GobChartsToolsWidget::~GobChartsToolsWidget()
{
  delete ui;
  delete m_private;
}

/*--------------------------------------------------------------------------------*/

void GobChartsToolsWidget::chartTypeChanged( QAbstractButton *button )
{
  if( button == ui->chartBarToolButton )
  {
    ui->gridGroupBox->setEnabled( true );
    ui->labelXToolButton->setEnabled( true );
    ui->labelYToolButton->setEnabled( true );
    emit createChart( BAR );    
  }
  else if( button == ui->chartPieToolButton )
  {
    ui->gridGroupBox->setEnabled( false );    // makes no sense to have a grid with a pie chart
    ui->labelXToolButton->setEnabled( false );
    ui->labelYToolButton->setEnabled( false );
    emit createChart( PIE );
  }
  else if( button == ui->chartLineToolButton )
  {
    ui->gridGroupBox->setEnabled( true );
    ui->labelXToolButton->setEnabled( true );
    ui->labelYToolButton->setEnabled( true );
    emit createChart( LINE );
  }

  ui->labelTextGroupBox->setEnabled( true );
  ui->labelStyleGroupBox->setEnabled( true );
  ui->saveButton->setEnabled( true );
  ui->chartClearButton->setEnabled( true );
  ui->chartColourPickerButton->setEnabled( true );
  ui->chartRangeToolButton->setEnabled( true );
}

/*--------------------------------------------------------------------------------*/

void GobChartsToolsWidget::chartColourPreferenceChanged()
{
  if( ui->chartColourPickerButton->isChecked() )
  {
    QColor colour = QColorDialog::getColor();

    if( colour.isValid() )
    {
      emit setFixedColour( colour );
      m_private->m_chartColour = colour;
      m_private->m_fixedColour = true;
    }
    else
    {
      emit setRandomColours();
      ui->chartColourPickerButton->setChecked( false );
      m_private->m_fixedColour = false;
    }
  }
  else
  {
    emit setRandomColours();
    m_private->m_fixedColour = false;
  }
}

/*--------------------------------------------------------------------------------*/

void GobChartsToolsWidget::chartRangePreferenceChanged()
{
  if( ui->chartRangeToolButton->isChecked() )
  {
    ui->chartsRangeGroupBox->setEnabled( true );

    /* If the user had previously specified a range, they might be toggling
      the view to see what difference it makes, so we need to update the chart accordingly. */
    if( m_private->m_rangePreferenceSet )
    {
      chartRangePreferenceSelected();
    }
  }
  else
  {
    ui->chartsRangeGroupBox->setEnabled( false );
    emit setShowTotalRange();
    m_private->m_showTotalRange = true;
  }
}

/*--------------------------------------------------------------------------------*/

void GobChartsToolsWidget::chartRangePreferenceSelected()
{
  /* In the process of loading the chart from XML, this function was found to be
     called too quickly in succession to be effective, so a flag was introduced to make
     sure that this call isn't made until we're done reading in all the values. */
  if( !m_private->m_loadingChart )
  {
    qreal lower( ui->chartRangeLowerBoundarySpinBox->value() );
    qreal upper( ui->chartRangeUpperBoundarySpinBox->value() );

    m_private->m_showTotalRange = false;
    m_private->m_rangePreferenceSet = true;
    m_private->m_lowerBound = lower;
    m_private->m_upperBound = upper;

    emit setAllowedDataRange( lower, upper );
  }
}

/*--------------------------------------------------------------------------------*/

void GobChartsToolsWidget::gridLinePreferenceChanged( QAbstractButton *button )
{
  if( button == ui->gridSolidLineToolButton )
  {
    m_private->m_penStyle = Qt::SolidLine;
    emit setGridLineStyle( Qt::SolidLine );
  }
  else if( button == ui->gridDashedLineToolButton )
  {
    m_private->m_penStyle = Qt::DashLine;
    emit setGridLineStyle( Qt::DashLine );
  }
  else if( button == ui->gridDotLineToolButton )
  {
    m_private->m_penStyle = Qt::DotLine;
    emit setGridLineStyle( Qt::DotLine );
  }
}

/*--------------------------------------------------------------------------------*/

void GobChartsToolsWidget::gridLineNumbersChanged()
{
  emit setHorizontalGridLines( ui->gridHorizontalToolButton->isChecked(),
                               ui->gridHorizontalSpinBox->value() );

  emit setVerticalGridLines( ui->gridVerticalToolButton->isChecked(),
                             ui->gridVerticalSpinBox->value() );
}

/*--------------------------------------------------------------------------------*/

void GobChartsToolsWidget::gridLinesSelected()
{
  m_private->m_verticalGrid   = ui->gridVerticalToolButton->isChecked();
  m_private->m_horizontalGrid = ui->gridHorizontalToolButton->isChecked();
  emit setVerticalGridLines  ( m_private->m_verticalGrid, ui->gridVerticalSpinBox->value() );
  emit setHorizontalGridLines( m_private->m_horizontalGrid, ui->gridHorizontalSpinBox->value() );
}

/*--------------------------------------------------------------------------------*/

void GobChartsToolsWidget::gridColourSelected()
{
  QColor colour = QColorDialog::getColor();

  if( colour.isValid() )
  {
    emit setGridColour( colour );
    m_private->m_gridColour = colour;
    m_private->m_gridColourSet = true;
  }
}

/*--------------------------------------------------------------------------------*/

void GobChartsToolsWidget::updateLabelDetails()
{
  /* We don't want to end up emitting changes if we just received label details via request. */
  if( !m_private->m_labelDetails.m_labelDetailsReceived )
  {
    m_private->m_labelDetails.setupLabel( this );

    if( ui->labelHeaderToolButton->isChecked() )
    {
      emit setLabelDetails( HEADER,
                            m_private->m_labelDetails.m_text,
                            m_private->m_labelDetails.m_font,
                            m_private->m_labelDetails.m_fontColour,
                            m_private->m_labelDetails.m_align );
    }
    else if( ui->labelXToolButton->isChecked() )
    {
      emit setLabelDetails( XLABEL,
                            m_private->m_labelDetails.m_text,
                            m_private->m_labelDetails.m_font,
                            m_private->m_labelDetails.m_fontColour,
                            m_private->m_labelDetails.m_align );
    }
    else if( ui->labelYToolButton->isChecked() )
    {
      emit setLabelDetails( YLABEL,
                            m_private->m_labelDetails.m_text,
                            m_private->m_labelDetails.m_font,
                            m_private->m_labelDetails.m_fontColour,
                            m_private->m_labelDetails.m_align );
    }
  }
}

/*--------------------------------------------------------------------------------*/

void GobChartsToolsWidget::labelColourPreferenceChanged()
{
  QColor colour = QColorDialog::getColor();

  if( colour.isValid() )
  {
    m_private->m_labelDetails.m_fontColour = colour;
    updateLabelDetails();
  }
}

/*--------------------------------------------------------------------------------*/

void GobChartsToolsWidget::labelAlignmentPreferenceChanged( QAbstractButton *button )
{
  if( button == ui->labelAlignCentreButton )
  {
    m_private->m_labelDetails.m_align = Qt::AlignCenter;
  }
  else if( button == ui->labelAlignLeftButton )
  {
    m_private->m_labelDetails.m_align = Qt::AlignLeft;
  }
  else if( button == ui->labelAlignRightButton )
  {
    m_private->m_labelDetails.m_align = Qt::AlignRight;
  }

  updateLabelDetails();
}

/*--------------------------------------------------------------------------------*/

void GobChartsToolsWidget::generateLabelDetailRequest( QAbstractButton *button )
{
  if( button == ui->labelHeaderToolButton )
  {
    emit requestLabelDetails( HEADER );
  }
  else if( button == ui->labelYToolButton )
  {
    emit requestLabelDetails( YLABEL );
  }
  else if( button == ui->labelXToolButton )
  {
    emit requestLabelDetails( XLABEL );
  }

  ui->labelLineEdit->setFocus();
}

/*--------------------------------------------------------------------------------*/

void GobChartsToolsWidget::receiveLabelDetails( GobChartsLabel label, QString text, QFont font, QColor colour, Qt::Alignment align )
{
  /* Avoid recursively calling this function through signals emitted by the changes below. */
  m_private->m_labelDetails.m_labelDetailsReceived = true;

  if( label == HEADER )
  {
    ui->labelHeaderToolButton->setChecked( true );
  }
  else if( label == YLABEL )
  {
    ui->labelYToolButton->setChecked( true );
  }
  else if( label == XLABEL )
  {
    ui->labelXToolButton->setChecked( true );
  }

  ui->labelLineEdit->setText( text );
  ui->labelFontComboBox->setCurrentFont( font );
  ui->labelBoldButton->setChecked( font.bold() );
  ui->labelItalicsButton->setChecked( font.italic() );
  ui->labelUnderlineButton->setChecked( font.underline() );

  m_private->m_labelDetails.m_fontColour = colour;

  for( int i = 0; i < ui->labelFontSizeComboBox->count(); i++ )
  {
    if( ui->labelFontSizeComboBox->itemText( i ) == QString( "%1" ).arg( font.pointSize() ) )
    {
      ui->labelFontSizeComboBox->setCurrentIndex( i );
      break;
    }
  }

  if( align == Qt::AlignCenter )
  {
    ui->labelAlignCentreButton->setChecked( true );
  }
  else if( align == Qt::AlignLeft )
  {
     ui->labelAlignLeftButton->setChecked( true );
  }
  else if( align == Qt::AlignRight )
  {
    ui->labelAlignRightButton->setChecked( true );
  }

  m_private->m_labelDetails.m_labelDetailsReceived = false;
}

/*--------------------------------------------------------------------------------*/

QString GobChartsToolsWidget::getStateXML() const
{
  QDomDocument doc;
  QDomElement root = doc.createElement( "ToolsWidget" );
  doc.appendChild( root );

  /* Chart. */
  QDomElement chart = doc.createElement( "Chart" );

  if( m_private->m_fixedColour )
  {
    chart.setAttribute( "fixed", "true" );

    QDomElement colour = doc.createElement( "ChartColour" );
    colour.setAttribute( "red",   m_private->m_chartColour.red() );
    colour.setAttribute( "green", m_private->m_chartColour.green() );
    colour.setAttribute( "blue",  m_private->m_chartColour.blue() );

    chart.appendChild( colour );
  }
  else
  {
    chart.setAttribute( "fixed", "false" );
  }

  root.appendChild( chart );

  /* Range. */
  QDomElement range = doc.createElement( "Range" );

  if( m_private->m_showTotalRange )
  {
    range.setAttribute( "total", "true" );
  }
  else
  {
    range.setAttribute( "total", "false" );
    range.setAttribute( "lower", m_private->m_lowerBound );
    range.setAttribute( "upper", m_private->m_upperBound );
  }

  root.appendChild( range );

  /* Grid. */
  QDomElement grid = doc.createElement( "Grid" );

  if( m_private->m_gridColourSet )
  {
    grid.setAttribute( "fixedColour", "true" );

    QDomElement colour = doc.createElement( "GridColour" );
    colour.setAttribute( "red",   m_private->m_gridColour.red() );
    colour.setAttribute( "green", m_private->m_gridColour.green() );
    colour.setAttribute( "blue",  m_private->m_gridColour.blue() );

    grid.appendChild( colour );
  }
  else
  {
    grid.setAttribute( "fixedColour", "false" );
  }

  if( m_private->m_verticalGrid )
  {
    grid.setAttribute( "vertical", "true" );
    grid.setAttribute( "vertlines", QString( "%1" ).arg( ui->gridVerticalSpinBox->value() ) );
  }
  else
  {
    grid.setAttribute( "vertical", "false" );
  }

  if( m_private->m_horizontalGrid )
  {
    grid.setAttribute( "horizontal", "true" );
    grid.setAttribute( "horlines", QString( "%1" ).arg( ui->gridHorizontalSpinBox->value() ) );
  }
  else
  {
    grid.setAttribute( "horizontal", "true" );
  }

  if( m_private->m_penStyle == Qt::SolidLine )
  {
    grid.setAttribute( "style", "solid" );
  }
  else if( m_private->m_penStyle == Qt::DotLine )
  {
    grid.setAttribute( "style", "dot" );
  }
  else
  {
    grid.setAttribute( "style", "dash" );
  }

  root.appendChild( grid );

  /* Labels are not the responsibility of this widget as they don't form part of the widget's state. */

  return doc.toString( 2 );
}

/*--------------------------------------------------------------------------------*/

void GobChartsToolsWidget::setStateXML( GobChartsType type, const QDomNode &toolNode )
{
  if( !toolNode.isNull() )
  {
    clearAndReset();

    /* If any of the QString to "int" or to "double" conversions fail we just inform the user,
      no further action is taken. */
    bool ok( true );

    /* Chart. */
    if( toolNode.firstChildElement( "Chart" ).attribute( "fixed" ) == "true" )
    {
      m_private->m_fixedColour = true;
      QDomNamedNodeMap colourMap = toolNode.namedItem( "Chart" ).namedItem( "ChartColour" ).attributes();

      QColor fixedColour;
      fixedColour.setRed  ( colourMap.namedItem( "red" ).nodeValue().toInt( &ok ) );
      fixedColour.setGreen( colourMap.namedItem( "green" ).nodeValue().toInt( &ok ) );
      fixedColour.setBlue ( colourMap.namedItem( "blue" ).nodeValue().toInt( &ok ) );

      m_private->m_chartColour = fixedColour;
      ui->chartColourPickerButton->setChecked( true );
    }
    else
    {
      m_private->m_fixedColour = false;
      ui->chartColourPickerButton->setChecked( false );
    }

    /* Range. */
    QDomNamedNodeMap rangeMap = toolNode.namedItem( "Range" ).attributes();

    if( rangeMap.namedItem( "total" ).nodeValue() == "true" )
    {
      m_private->m_showTotalRange = true;
      ui->chartRangeToolButton->setChecked( false );
    }
    else
    {
      m_private->m_showTotalRange = false;
      m_private->m_rangePreferenceSet = true;
      m_private->m_lowerBound = rangeMap.namedItem( "lower" ).nodeValue().toDouble( &ok );
      m_private->m_upperBound = rangeMap.namedItem( "upper" ).nodeValue().toDouble( &ok );

      /* Set the "loading chart" flag so that the chartRangePreferenceChanged() slot does
        not interfere with the process. */
      m_private->m_loadingChart = true;
      ui->chartRangeLowerBoundarySpinBox->setValue( m_private->m_lowerBound );
      ui->chartRangeUpperBoundarySpinBox->setValue( m_private->m_upperBound );
      m_private->m_loadingChart = false;
      chartRangePreferenceSelected();

      ui->chartRangeToolButton->setChecked( true );
    }

    /* Grid. */
    QDomNamedNodeMap gridMap = toolNode.namedItem( "Grid" ).attributes();

    if( gridMap.namedItem( "fixedColour" ).nodeValue() == "true" )
    {
      m_private->m_gridColourSet = true;

      QDomNamedNodeMap colourMap = toolNode.namedItem( "Grid" ).namedItem( "GridColour" ).attributes();

      QColor gridColour;
      gridColour.setRed  ( colourMap.namedItem( "red" ).nodeValue().toInt( &ok ) );
      gridColour.setGreen( colourMap.namedItem( "green" ).nodeValue().toInt( &ok ) );
      gridColour.setBlue ( colourMap.namedItem( "blue" ).nodeValue().toInt( &ok ) );

      m_private->m_gridColour = gridColour;
    }
    else
    {
      m_private->m_gridColourSet = false;
    }

    if( gridMap.namedItem( "vertical" ).nodeValue() == "true" )
    {
      m_private->m_verticalGrid = true;
      ui->gridGroupBox->setEnabled( true );
      ui->gridVerticalToolButton->setChecked( true );
      ui->gridVerticalSpinBox->setValue( gridMap.namedItem( "vertlines" ).toAttr().value().toInt() );
    }
    else
    {
      m_private->m_verticalGrid = false;
      ui->gridVerticalToolButton->setChecked( false );
    }

    if( gridMap.namedItem( "horizontal" ).nodeValue() == "true" )
    {
      m_private->m_horizontalGrid = true;
      ui->gridGroupBox->setEnabled( true );
      ui->gridHorizontalToolButton->setChecked( true );
      ui->gridHorizontalSpinBox->setValue( gridMap.namedItem( "horlines" ).toAttr().value().toInt() );
    }
    else
    {
      m_private->m_horizontalGrid = false;
      ui->gridHorizontalToolButton->setChecked( false );
    }

    if( gridMap.namedItem( "style" ).nodeValue() == "solid" )
    {
      m_private->m_penStyle = Qt::SolidLine;
      ui->gridSolidLineToolButton->setChecked( true );
    }
    else if( gridMap.namedItem( "style" ).nodeValue() == "dot" )
    {
      m_private->m_penStyle = Qt::DotLine;
      ui->gridDotLineToolButton->setChecked( true );
    }
    else
    {
      m_private->m_penStyle = Qt::DashLine;
      ui->gridDashedLineToolButton->setChecked( true );
    }

    if( !ok )
    {
      QMessageBox::critical( this, "Error", "Failed to convert one or more data values to their appropriate type." );
    }
    else
    {
      /* Labels are not the responsibility of this widget as they don't form part of the widget's state,
        so we don't want to attempt a label update when emitting the state signals. */
      m_private->m_labelDetails.m_labelDetailsReceived = true;
      emitStateSignals();
      m_private->m_labelDetails.m_labelDetailsReceived = false;
    }

    ui->labelTextGroupBox->setEnabled( true );
    ui->labelStyleGroupBox->setEnabled( true );
    ui->saveButton->setEnabled( true );
    ui->chartClearButton->setEnabled( true );
    ui->chartColourPickerButton->setEnabled( true );
    ui->chartRangeToolButton->setEnabled( true );

    const_cast< QToolButton* >( m_private->m_typeButtonMap.value( type ) )->setChecked( true );
  }
  else
  {
    QMessageBox::critical( this, "Error", "Failed to load chart." );
  }
}

/*--------------------------------------------------------------------------------*/

void GobChartsToolsWidget::emitStateSignals()
{
  /* Chart colour. */
  if( m_private->m_fixedColour )
  {
    emit setFixedColour( m_private->m_chartColour );
  }

  /* Data range. */
  if( m_private->m_showTotalRange )
  {
    emit setShowTotalRange();
  }
  else
  {
    emit setAllowedDataRange( m_private->m_lowerBound, m_private->m_upperBound );
  }

  /* Grid. */
  if( ui->gridGroupBox->isEnabled() )
  {
    emit setHorizontalGridLines( m_private->m_horizontalGrid, ui->gridHorizontalSpinBox->value() );
    emit setGridLineStyle      ( m_private->m_penStyle );
    emit setVerticalGridLines  ( m_private->m_verticalGrid, ui->gridVerticalSpinBox->value() );

    if( m_private->m_gridColourSet )
    {
      emit setGridColour( m_private->m_gridColour );
    }
  }

  /* Label details are dealt with in GobChartsWidget::createChart. */
}

/*--------------------------------------------------------------------------------*/

void GobChartsToolsWidget::clearAndReset()
{
  ui->labelTextGroupBox->setEnabled( false );
  ui->labelStyleGroupBox->setEnabled( false );
  ui->gridGroupBox->setEnabled( false );
  ui->saveButton->setEnabled( false );
  ui->chartClearButton->setEnabled( false );
  ui->chartColourPickerButton->setEnabled( false );
  ui->chartRangeToolButton->setEnabled( false );

  ui->gridHorizontalToolButton->setChecked( false );
  ui->gridVerticalToolButton->setChecked( false );
  ui->gridDotLineToolButton->setChecked( true );

  ui->labelBoldButton->setChecked( false );
  ui->labelItalicsButton->setChecked( false );
  ui->labelUnderlineButton->setChecked( false );
  ui->labelAlignCentreButton->setChecked( false );
  ui->labelAlignLeftButton->setChecked( false );
  ui->labelAlignRightButton->setChecked( false );

  /* If we don't set the button group's exclusive flag to false
    then we cannot ensure that all three chart buttons are set
    as unchecked after the reset. */
  m_private->m_chartButtonGroup.setExclusive( false );
  ui->chartBarToolButton->setChecked( false );
  ui->chartPieToolButton->setChecked( false );
  ui->chartLineToolButton->setChecked( false );
  m_private->m_chartButtonGroup.setExclusive( true );

  /* If we don't set the button group's exclusive flag to false
    then we cannot ensure that all three label buttons are set
    as unchecked after the reset. */
  m_private->m_labelButtonGroup.setExclusive( false );
  ui->labelHeaderToolButton->setChecked( false );
  ui->labelYToolButton->setChecked( false );
  ui->labelXToolButton->setChecked( false );
  m_private->m_labelButtonGroup.setExclusive( true );

  ui->labelLineEdit->clear();

  ui->chartRangeLowerBoundarySpinBox->setValue( 0.0 );
  ui->chartRangeUpperBoundarySpinBox->setValue( 0.0 );
  ui->gridHorizontalSpinBox->setValue( 0 );
  ui->gridVerticalSpinBox->setValue( 0 );

  m_private->m_labelDetails.m_labelDetailsReceived = false;
  m_private->m_loadingChart = false;
  m_private->m_fixedColour = false;
  m_private->m_gridColourSet = false;
  m_private->m_verticalGrid = false;
  m_private->m_horizontalGrid = false;
  m_private->m_rangePreferenceSet = false;
  m_private->m_showTotalRange = true;
}

/*--------------------------------------------------------------------------------*/

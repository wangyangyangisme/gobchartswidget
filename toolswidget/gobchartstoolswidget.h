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

#ifndef GOBCHARTSTOOLSWIDGET_H
#define GOBCHARTSTOOLSWIDGET_H

#include <QFrame>
#include "utils/globalincludes.h"
#include "utils/gobchartsnocopy.h"

namespace Ui
{
  class GobChartsToolsWidget;
}

class QAbstractButton;
class QDomNode;

/// Provides the end-user with the tools to select and customise charts.

/** GobChartsToolsWidget keeps track of user preferences, emits signals relating to user decisions
    and updates itself on occasions where the user manipulates the chart directly. */
class GobChartsToolsWidget : public QFrame,
                             public GobChartsNoCopy
{
  Q_OBJECT

public:
  //! Constructor.
  explicit GobChartsToolsWidget( QWidget *parent = 0 );

  //! Destructor.
  ~GobChartsToolsWidget();

  /*! Returns the tool widget's content and settings as an XML QString.
      This function takes all the tool settings, converts them to XML and returns the XML string.
      \sa setStateXML() and emitStateSignals() */
  QString getStateXML() const;

  /*! Used when saving or loading a chart to or from file.  After reading in
      the relevant data from the QDomNode, all the relevant state signals are
      emitted in succession to update the listener with the changes just loaded.
      \sa getStateXML() and emitStateSignals()  */
  void setStateXML( GobChartsType type, const QDomNode &toolNode );

  /*! Emits all the tool's state signals after loading data from XML. 
      \sa getStateXML() and setStateXML() */
  void emitStateSignals();

public slots:
  /*! Receives label details. Changing the label that is currently in focus (i.e when the user clicks on 
      any of the three label buttons "H", "X" or "Y"), we wish to update the font in the dropdown, the text 
      alignment, italics, bold and underlined buttons, etc in line with the chosen label.  This is how we get that 
      information from the main widget and we request that information be sent via requestLabelDetails(). */
  void receiveLabelDetails( GobChartsLabel label, QString text, QFont font, QColor colour, Qt::Alignment align);

signals:
  /*! Emitted when the user clicks the "Clear" button. */
  void clearChart();

  /*! Emitted when the user clicks the "Save" button.
      \sa loadChart() */
  void saveChart();

  /*! Emitted when the user clicks the "Load" button. 
      \sa saveChart() */
  void loadChart();

  /*! Emitted when the user clicks on any of the chart tool buttons (BAR, PIE or LINE). */
  void createChart( GobChartsType );

  /*! Emitted when the user selects a fixed chart colour.
      Fix the chart colour to "colour".  This means that the different data categories will all be
      displayed in "colour" as opposed to having randomly generated (different) colours. 
      \sa setRandomColours() */
  void setFixedColour( QColor );

  /*! Emitted when the user wishes random colour selection to be re-enabled.
      Re-enables random colour generation (the default) for chart items after a call to setFixedColour().
      \sa setFixedColour() */
  void setRandomColours();

  /*! Emitted when the user chooses to restrict the chart's data range.
      Setting the allowed data range determines which categories will be shown in the final chart (i.e. only those
      data categories with values within the range will be shown).  By default the entire value range is drawn. 
      The first parameter contains the lowest allowed value to be displayed and the second the highest.
      \sa setShowTotalRange() */
  void setAllowedDataRange( qreal, qreal );

  /*! Emitted when the user chooses to display the entire data range (normally after a previous call to
      setAllowedDataRange).
      \sa setAllowedDataRange() */      
  void setShowTotalRange();

  /*! Emitted when the pen style used to draw the grid lines changes.  See Qt API documentation for details on the enum. */
  void setGridLineStyle( Qt::PenStyle );

  /*! Emitted when the user turns the display of the vertical grid lines "on" or "off"
      or when the number of vertical grid lines changes. */
  void setVerticalGridLines( bool, int );

  /*! Emitted when the user turns the display of the horizontal grid lines "on" or "off"
      or when the number of horizontal grid lines changes. */
  void setHorizontalGridLines( bool, int );

  /*! Emits a user-selected grid colour. */
  void setGridColour( QColor );

  /*! Emitted when a label's details changes.
      \sa requestLabelDetails() and receiveLabelDetails() */
  void setLabelDetails( GobChartsLabel, QString, QFont, QColor, Qt::Alignment );

  /*! Emitted when an update to a label's details is required.
      \sa setLabelDetails() and receiveLabelDetails() */
  void requestLabelDetails( GobChartsLabel );

private slots:
  void chartTypeChanged( QAbstractButton *button );
  void chartColourPreferenceChanged();
  void chartRangePreferenceChanged();
  void chartRangePreferenceSelected();
  void gridLinePreferenceChanged( QAbstractButton *button );
  void gridLineNumbersChanged();
  void gridLinesSelected();
  void gridColourSelected();
  void updateLabelDetails();
  void labelColourPreferenceChanged();
  void labelAlignmentPreferenceChanged( QAbstractButton *button );
  void generateLabelDetailRequest( QAbstractButton *button );
  void clearAndReset();

private:
  Ui::GobChartsToolsWidget *ui;

  friend class LabelDetails;
  class LabelDetails;

  struct GobChartsToolsWidgetPrivate;
  GobChartsToolsWidgetPrivate *m_private;
};

#endif // GOBCHARTSTOOLSWIDGET_H

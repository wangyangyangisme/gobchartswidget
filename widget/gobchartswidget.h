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

#ifndef GOBCHARTSWIDGET_H
#define GOBCHARTSWIDGET_H

#include <QFrame>
#include "utils/gobchartswidgetdef.h"
#include "utils/globalincludes.h"
#include "utils/gobchartsnocopy.h"

class QAbstractItemModel;
class QItemSelectionModel;
class QListWidgetItem;

/*! \mainpage The GobChartsWidget Library

    \section intro_sec Introduction
 
    If you are looking for a complex, end-user charting library for use with Qt, this is probably
    not it.  However, if your application is in need of some basic charting functionality and
    your primary data consists of a one to one mapping of category to value, then I believe that
    this widget may just be what you need (please see GobChartsWidget for important information 
    regarding usage).

    If you want to know more, the <a href="http://goblincoding.com/qt-chart-widget/">official site</a>
    contains all the information about features and limitations as well as why this library is the way
    it is.

    Please also feel free to <a href="http://goblincoding.com/contact">contact me</a> for any
    reason whatsoever.
 
    \section download Download

    If you haven't yet, please see the <a href="http://goblincoding.com/chartdownload">download page</a>
    for a list of possible download options and <b>ensure that you download the correct version for your
    compiler.</b>

    \section using Using the Downloaded Libraries
 
    Simply add \n

          LIBS += -L$$PWD/../path/to/lib -lgobchartswidget
          INCLUDEPATH += $$PWD/../path/to/headers
          DEPENDPATH += $$PWD/..//path/to/headers
    
    to your Qt project file and make sure you "#include <gobchartswidget.h>" where you want to instantiate
    the widget (the above will work for static linking, if you wish to load the library at runtime, have a look
    at the API documentation for QLibrary).

    Alternatively, if you're using QtCreator, simply right-click on your Qt project, select "Add Library" and
    follow the prompts.
 
    \subsection issues1 Known Issues: Release vs Debug

    If you run into the following error when compiling your application with GobChartsWidget: \n
    
      "QWidget: Must construct a QApplication before a QPaintDevice"

    then there is most probably a discrepancy between the compile mode you are using and that of the library 
    you are linking to, i.e. if you are compiling in "release", then make sure you're using the "release" version of 
    the library and, obviously, if you're compiling in "debug" mode, use the "debug" version.

    \subsection issues2 Known Issues: Widget Style Sheets
    
    I developed this widget on Windows 7 and subsequently tested the widget on an Ubuntu (12.04 LTS) 
    virtual machine where I noticed that some of the widget's elements looked downright rubbish.\n
    
    It seems that the only workaround is to set specific style sheets on the  widget for a more polished look.  Since 
    I do not wish to make the call with regards to what the widget should or should not look like, I leave this up to 
    you (having said that, I <i>did</i> set a very basic style sheet on the Ubuntu distribution to ensure that the widget
    layout is correct, the style sheet is distributed with the source - see the "toolswidget" folder for the .txt file).
    
    If you do come up with nice style sheets, I would appreciate it greatly if you could simply 
    copy and paste your style sheet string into this
    <a href="http://goblincoding.com/contact">contact form</a> along with the OS name and version you implemented it on
    so that I may start distributing them along with the widget's source.
*/

/// Main widget class.

/** As an application developer, this is the only class that really needs to concern you
    (unless, of course, you are interested in changing any of the underlying functionality). \n

    GobChartsWidget will provide you with a final, usable widget that contains all the necessary
    GUI tools for an end-user to create and customise any of the available chart types (BAR, PIE or LINE).
    All you need to do is to create an instance of GobChartsWidget and inform it of your item and
    selection models via setModel() and setSelectionModel().  It is important to note that
    GobChartsWidget does NOT take ownership of either of the models set on it. \n

    <i>VERY IMPORTANT:</i> The data model you use <b>must</b> contain your data categories in its 
    first column and the corresponding values in the second.  Furthermore, GobChartsWidget can only 
    handle one-to-one mappings of categories to data, i.e. you cannot draw charts with data
    values spanning multiple columns and, finally, all negative data values are automatically rounded
    up to zero.  The reason for this is that pie charts do not lend themselves to the display of multiple
    data sets per category, nor the display of negative numbers and I decided to exclude these
    possibilities altogether in favour of a design allowing for greater flexibility at run-time and fewer
    complications with regards to the implementation.

    Finally, if you would like to see what GobChartsWidget gets up to in the background, you can turn debug
    logging on via setDebugLoggingOn() and connect to GobChartsWidget's lastDebugLogMsg() signal
    in order to receive the log output.
*/
class GOBCHARTSWIDGETSHARED_EXPORT GobChartsWidget : public QFrame,
                                                     public GobChartsNoCopy
{
  Q_OBJECT

public:
  //! Constructor.
  explicit GobChartsWidget( QWidget *parent = 0 );

  //! Destructor.
  ~GobChartsWidget();

  /*! Set the item model (GobChartsWidget does NOT take ownership). */
  void setModel( QAbstractItemModel *model );

  /*! Set the item selection model (GobChartsWidget does NOT take ownership). */
  void setSelectionModel( QItemSelectionModel *selectionModel );

  /*! Turn debug logging messages "on" or "off". 
      \sa lastDebugLogMsg() */
  void setDebugLoggingOn( bool log );

signals:
  /*! Emits the last debug log message.
      \sa setDebugLoggingOn() */
  void lastDebugLogMsg( QString );

private slots:
  void graphicsItemSelected( const QString & legendText );
  void legendItemSelected( QListWidgetItem *item );
  void createLegendItem( const QColor &colour, const QString &text );
  void clearLegend();

  void createChart( GobChartsType type );
  void clearChart();
  void saveChart();
  void loadChart();

private:
  struct GobChartsWidgetPrivate;
  GobChartsWidgetPrivate* const m_private;
};

#endif // GOBCHARTSWIDGET_H

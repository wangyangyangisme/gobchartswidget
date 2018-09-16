# gobchartswidget
https://sourceforge.net/p/gobchartswidget/code/ci/default/tree/

https://goblincoding.com/qt-chart-widget/

# Qt Chart Widget

#### **Great news! Qt Charts is now available under Qt 5.7 under GPLv3**

##### *NB: “GobChartsWidget” was a simple playground project intended for inclusion in Qt applications in need of very basic  graphical chart display functionality (in short, a free Qt chart  widget). This project was purely intended as a learning experience and  any ideas of taking it further were abandoned as soon as I realised  there were far more evolved, existing libraries out there.*

##### *Feel free to browse the source for ideas and please see what  this little widget is capable of, but if you are in search of  industry-level Qt charting capabilities you should rather consider: Qwt or QCustomPlot (open source solutions) or QtCharts or KD Chart (commercial).*

#### Jump to: [Limitations](https://goblincoding.com/qt-chart-widget/#chartwidget-limits) / [Features](https://goblincoding.com/qt-chart-widget/#chartwidget-features) / [Screenshots](https://goblincoding.com/qt-chart-widget/#chartwidget-screenshots) / [Downloads](https://goblincoding.com/qt-chart-widget/#chartwidget-downloads)

# Usage and Documentation

GobChartWidget’s implementation is pretty straightforward. In  addition to instantiating the widget, only two function calls are  compulsory (please see the [section on limitations](https://goblincoding.com/qt-chart-widget/#chartwidget-limits) page or the [API documentation](http://gobchartswidget.sourceforge.net/) for important information regarding restrictions on the model).

```
#include <gobchartswidget.h>

m_chartWidget = new GobChartsWidget;
m_chartWidget->setModel( model );
m_chartWidget->setSelectionModel( selectionModel );

/* Only required if you want to see what GobChartsWidget gets up to. */
m_chartWidget->setDebugLoggingOn( true );
connect( m_chartWidget, SIGNAL( lastDebugLogMsg( QString ) ), this, SLOT( logEntry( QString ) ) );

Add and show it as you would any other widget and you're done.
```

If you’ve never before used an external library in a Qt project, you  will have to tell Qt where to find GobChartsWidget after you  extract/build it. Simply add

```
LIBS += -L$$PWD/../path/to/lib -lgobchartswidget
INCLUDEPATH += $$PWD/../path/to/headers
DEPENDPATH += $$PWD/..//path/to/headers
```

to your Qt project file (works for static linking, if you wish to  load the library at runtime, have a look at the API documentation for [QLibrary](http://qt-project.org/doc/qt-4.8/qlibrary.html)).  More information is available in the [qMake documentation](http://qt-project.org/doc/qt-4.8/qmake-project-files.html#declaring-other-libraries).

Alternatively, if you’re using QtCreator, see [adding libraries to projects](http://doc-snapshot.qt-project.org/qtcreator-2.5/creator-project-qmake-libraries.html).

# Limitations

I started a Qt chart library quite some time ago (called the  “GobCharts” library) and through multiple iterations, the project  evolved to the point where I realised I was getting more and more  involved in the intricacies of charting and less in learning about  Qt/C++ and library development (which is what I actually initially set  out to do).

I ultimately decided to wrap what I had in an end-user widget for use  in Qt applications and distribute it “as is”.  Although not a  comprehensive chart widget/library, I’m hoping that some of you may find  it useful.

Two of the limitations of the widget is directly attributable to the decision to halt further development:

- Grid lines are of a purely cosmetic nature and have no bearing on data values whatsoever and
- The data model you use **\*must*** contain your data categories in the first column and the corresponding values in the second.

The other notable limitations are due to design considerations:

- GobChartsWidget can only handle one-to-one mappings of categories to  data, i.e. you cannot draw charts with data values spanning multiple  columns.
- All negative numbers are automatically rounded up to zero.

The reason for this is that pie charts do not lend themselves to the  display of multiple data sets per category, nor to the display of  negative numbers so I decided to exclude these possibilities altogether  in favour of a design allowing for greater flexibility at run-time and  fewer complications with regards to the implementation.

# Features

This video is a recording of the widget in action using my test  client.  In other words, the log window and table model do NOT form part  of the widget (if you really want the test client, however, you are  free to have it and can download it separately [here](https://bitbucket.org/goblincoding/gobchartswidgetclient)).
#  Copyright (C) 2012 by William Hallatt.
#
#  This file forms part of the "GobChartsWidget" library.
#
#  This library is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#
#  This library is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have downloaded a copy of the GNU General Public License
#  (GNUGPL.txt) and GNU Lesser General Public License (GNULGPL.txt)
#  along with this library.  If not, see <http://www.gnu.org/licenses/>.
#
#  The official website for this project is www.goblincoding.com and,
#  although not compulsory, it would be appreciated if all works of whatever
#  nature referring to or using this library include a reference to this site.

QT       += xml

TARGET = gobchartswidget
TEMPLATE = lib

DEFINES += GOBCHARTSWIDGET_LIBRARY

SOURCES += \
    widget/gobchartswidget.cpp \
    view/gobchartsview.cpp \
    view/gobchartspieview.cpp \
    view/gobchartslineview.cpp \
    view/gobchartsfactory.cpp \
    view/gobchartsbarview.cpp \
    utils/gobchartsvaliditems.cpp \
    utils/gobchartsgrid.cpp \
    utils/gobchartsgraphitems.cpp \
    utils/gobchartscolours.cpp \
    toolswidget/gobchartstoolswidget.cpp \
    label/gobchartstextitem.cpp

HEADERS += \
    widget/gobchartswidget.h \
    view/gobchartsview.h \
    view/gobchartspieview.h \
    view/gobchartslineview.h \
    view/gobchartsfactory.h \
    view/gobchartsbarview.h \
    utils/gobchartsvaliditems.h \
    utils/gobchartsnocopy.h \
    utils/gobchartsgrid.h \
    utils/gobchartsgraphitems.h \
    utils/gobchartscolours.h \
    utils/globalincludes.h \
    utils/gobchartswidgetdef.h \
    toolswidget/gobchartstoolswidget.h \
    label/gobchartstextitem.h

FORMS += \
    toolswidget/gobchartstoolswidget.ui

RESOURCES += \
    resources/gobchartsrc.qrc

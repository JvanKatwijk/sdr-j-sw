#
TEMPLATE    = lib
CONFIG      += plugin
QT          += core gui network widgets

INCLUDEPATH += . \
	       ../ ../.. \
	       ../../../includes \
	       ../../../includes/various
HEADERS     = ../rig-interface.h \
	      ../../../includes/swradio-constants.h \
	      ../../../includes/various/ringbuffer.h \
	      ./sdrt9r.h 
SOURCES     =  ./sdrt9r.cpp
TARGET      = $$qtLibraryTarget(device_sdrt9r)
FORMS	+= ./widget.ui

#for 32 bits windows we use
win32 {
DESTDIR     = ../../../../../../windows-bin/input-plugins
# includes in mingw differ from the includes in fedora linux
INCLUDEPATH += /usr/i686-mingw32/sys-root/mingw/include
INCLUDEPATH += /usr/i686-mingw32/sys-root/mingw/include/qwt
LIBS	+= -lqwt
LIBS	+= -lole32
LIBS	+= -lwinmm
LIBS	+= -lusb-1.0
}

unix{
DESTDIR     = ../../../../../linux-bin/input-plugins-sw
#LIBS	+= -lusb-1.0 -ldl
}


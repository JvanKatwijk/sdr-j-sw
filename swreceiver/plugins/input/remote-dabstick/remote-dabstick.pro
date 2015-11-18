#
TEMPLATE    = lib
CONFIG      += plugin
QT          += core gui network  widgets

INCLUDEPATH += . \
	       ../ ../.. \
	       ../../../includes \
	       ../../../includes/various
HEADERS     = ../rig-interface.h \
	      ../../../includes/swradio-constants.h \
	      ../../../includes/various/ringbuffer.h \
	      ./remote-dabstick.h 
SOURCES     =  ./remote-dabstick.cpp
TARGET      = $$qtLibraryTarget(device_remote-dabstick)
FORMS	+= ./widget.ui

#for 32 bits windows we use
win32 {
DESTDIR     = ../../../../../../windows-bin-sw/input-plugins-sw
# includes in mingw differ from the includes in fedora linux
INCLUDEPATH += /usr/i686-mingw32/sys-root/mingw/include
#INCLUDEPATH += /usr/i686-mingw32/sys-root/mingw/include/qt5/qwt
#LIBS	+= -lqwt-qt5
LIBS	+= -lole32
LIBS	+= -lwinmm
LIBS	+= -lusb-1.0
}

unix{
DESTDIR     = ../../../../linux-bin/input-plugins-sw
#LIBS	+= -lusb-1.0 -ldl
}


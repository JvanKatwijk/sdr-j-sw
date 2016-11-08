#
TEMPLATE    = lib
CONFIG      += plugin
QT          += core gui widgets

DEFINES		+= SDRPLAY_LIBRARY_NEW

INCLUDEPATH += . \
	       ../ ../.. \
	       ../../../includes \
	       ../../../includes/filters \
	       ../../../includes/various
HEADERS     = ../rig-interface.h \
	      ../../../includes/swradio-constants.h \
	      ./sdrplay.h \
	      ./sdrplay-worker.h \
	      ./sdrplay-loader.h \
	      ../../../includes/filters/fir-filters.h \
	      ../../../includes/various/ringbuffer.h
SOURCES     =  ./sdrplay.cpp \
	       ./sdrplay-worker.cpp \
	       ./sdrplay-loader.cpp \
	       ../../../src/filters/fir-filters.cpp
TARGET      = $$qtLibraryTarget(device_sdrPlay)
FORMS	+= ./widget.ui

#for 32 bits windows we use
win32 {
DESTDIR     = ../../../../../../windows-bin/input-plugins
# includes in mingw differ from the includes in fedora linux
INCLUDEPATH	+= /usr/i686-mingw32/sys-root/mingw/include
HEADERS		+= mir_sdr-windows.h
LIBS		+= -lole32
LIBS		+= -lwinmm
LIBS		+= -lusb-1.0
}

unix{
DESTDIR     = ../../../../linux-bin/input-plugins-sw
LIBS	+= -lusb-1.0 -ldl
}


#
TEMPLATE    = lib
CONFIG      += plugin
QT          += core gui widgets
INCLUDEPATH += . \
	       ../ ../.. \
	       ../../../includes \
	       ../../../includes/various
HEADERS     = ../rig-interface.h \
	      ../../../includes/swradio-constants.h \
	      ../../../includes/various/ringbuffer.h \
	      ./cardreader.h \
	      ../pa-reader.h
SOURCES     = ./cardreader.cpp \
	      ../pa-reader.cpp
TARGET      = $$qtLibraryTarget(device_cardReader)
FORMS	+= ./widget.ui

win32 {
DESTDIR     = ../../../../../../windows-bin-sw/input-plugins-sw
# includes in mingw differ from the includes in fedora linux
INCLUDEPATH += /usr/i686-w64-mingw32/sys-root/mingw/include
LIBS	+= -lportaudio
LIBS	+= -lusb-1.0
LIBS	+= -lole32
LIBS	+= -lwinmm
}

unix{
DESTDIR     = ../../../../linux-bin/input-plugins-sw
LIBS	+= -lusb-1.0 -lportaudio
}


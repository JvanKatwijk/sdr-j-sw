#
TEMPLATE    = lib
CONFIG      += plugin
QT          += core gui widgets
INCLUDEPATH += . \
	       ../ ../.. \
	       ../../../includes \
	       ../../../includes/various
HEADERS     = ../rig-interface.h \
	      ./dll-handler.h \
	      ../pa-reader.h \
	      ../../../includes/swradio-constants.h \
	      ../../../includes/various/ringbuffer.h
SOURCES     = dll-handler.cpp  ../pa-reader.cpp
TARGET      = $$qtLibraryTarget(device_dllhandler)
FORMS	+= ./widget.ui

win32 {
DESTDIR     = ../../../../../../windows-bin-sw/input-plugins-sw
# includes in mingw differ from the includes in fedora linux
INCLUDEPATH += /usr/i686-w64-mingw32/sys-root/mingw/include
INCLUDEPATH += /usr/i686-w64-mingw32/sys-root/mingw/include/qwt
LIBS	+= -lportaudio
LIBS	+= -lusb-1.0
LIBS	+= -lole32
LIBS	+= -lwinmm
}

unix {
DESTDIR     = ../../../../linux-bin/input-plugins-sw
LIBS	+= -lportaudio
}


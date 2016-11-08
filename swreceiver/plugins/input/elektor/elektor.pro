#
TEMPLATE    = lib
CONFIG      += plugin
QT          += core gui widgets
INCLUDEPATH += . \
	       ../ ../.. \
	       ../../../includes \
	       ../../../includes/various 
HEADERS     = ../rig-interface.h \
	      ./elektor.h \
	      ./band.h \
	      ./frequencytables.h \
	      ../../../includes/swradio-constants.h \
	      ../../../includes/various/ringbuffer.h \
	      ../pa-reader.h
SOURCES     = ./elektor.cpp \
	      ./band.cpp \
	      ./frequencytables.cpp \
	      ../pa-reader.cpp
TARGET      = $$qtLibraryTarget(device_elektor)
FORMS	+= ./widget.ui

unix {
DESTDIR     = ../../../../linux-bin/input-plugins-sw
INCLUDEPATH	+= /usr/include/libftdi1
LIBS	  	+=  -lftdi1
}

win32 {
DESTDIR     = ../../../../../../windows-bin/input-plugins
# includes in mingw differ from the includes in fedora linux
INCLUDEPATH += /usr/i686-w64-mingw32/sys-root/mingw/include
INCLUDEPATH += /usr/i686-w64-mingw32/sys-root/mingw/include/qwt
LIBS	+= -lportaudio
LIBS	+= -lole32
LIBS	+= -lwinmm
}


#
TEMPLATE    = lib
CONFIG      += plugin
QT          += core gui widgets

INCLUDEPATH += . \
	       ../ ../.. \
	       ../decoder-utils \
	       ../../../includes \
	       ../../../includes/filters \
	       ../../../includes/various
HEADERS     = ../decoder-interface.h \
	      ./rtty-decoder.h \
	      ../../../includes/filters/iir-filters.h \
	      ../../../includes/various/oscillator.h \
	      ../../../includes/various/utilities.h \
	      ../../../includes/swradio-constants.h
SOURCES     = ./rtty-decoder.cpp \
	      ../../../src/filters/iir-filters.cpp \
	      ../../../src/various/oscillator.cpp \
	      ../../../src/various/utilities.cpp \
	      ./main.cpp 
TARGET      = $$qtLibraryTarget(rttydecoder)
FORMS	+= widget.ui

win32 {
DESTDIR     = ../../../../../../windows-bin-sw/decoder-plugins
# includes in mingw differ from the includes in fedora linux
INCLUDEPATH += /usr/i686-w64-mingw32/sys-root/mingw/include
LIBS	+= -lole32
LIBS	+= -lwinmm
}

unix {
DESTDIR     = ../../../../../linux-bin/decoder-plugins
}


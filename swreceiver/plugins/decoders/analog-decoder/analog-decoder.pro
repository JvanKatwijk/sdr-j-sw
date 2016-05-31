#
TEMPLATE    = lib
CONFIG      += plugin
QT          += core gui widgets

INCLUDEPATH += . \
	       ../ ../.. \
	       ../decoder-utils \
	       ../../radio-utils \
	       ../../../includes \
	       ../../../includes/filters \
	       ../../../includes/various \
	       ../../../includes/output
HEADERS     = ../decoder-interface.h \
	      ./analog-decoder.h \
	      ./squelchClass.h \
	      ../decoder-utils/pllC.h \
	      ../decoder-utils/filter.h \
	      ../../../includes/filters/fir-filters.h \
	      ../../../includes/filters/iir-filters.h \
	      ../../../includes/various/sincos.h \
	      ../../../includes/various/utilities.h \
	      ../../../includes/swradio-constants.h
SOURCES     = ./analog-decoder.cpp \
	      ../decoder-utils/pllC.cpp \
	      ../../../src/filters/fir-filters.cpp \
	      ../../../src/filters/iir-filters.cpp \
	      ../../../src/various/sincos.cpp \
	      ../../../src/various/utilities.cpp \
	      ./main.cpp

TARGET      = $$qtLibraryTarget(analogdecoder)
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


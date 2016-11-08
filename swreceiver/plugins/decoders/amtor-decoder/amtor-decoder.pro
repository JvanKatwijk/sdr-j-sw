TEMPLATE    = lib
CONFIG      += plugin
QT          += core gui widgets

INCLUDEPATH += . \
	       ../ ../.. \
	       ../decoder-utils \
	       ../../radio-utils \
	       ../../../includes \
	       ../../../includes/filters \
	       ../../../includes/various
HEADERS     = ../decoder-interface.h \
	      ./amtor-decoder.h \
	      ../../../includes/filters/fir-filters.h \
	      ../../../includes/various/oscillator.h \
	      ../../../includes/various/utilities.h \
	      ../../../includes/swradio-constants.h
SOURCES     = ./amtor-decoder.cpp \
	      ../../../src/filters/fir-filters.cpp \
	      ../../../src/various/oscillator.cpp \
	      ../../../src/various/utilities.cpp \
	      ./main.cpp 
TARGET      = $$qtLibraryTarget(amtordecoder)
FORMS	+= widget.ui

win32 {
DESTDIR	= ../../../../../../windows-bin/decoder-plugins
# includes in mingw differ from the includes in fedora linux
INCLUDEPATH += /usr/i686-w64-mingw32/sys-root/mingw/include
INCLUDEPATH += /usr/i686-w64-mingw32/sys-root/mingw/include/qwt
LIBS	+= -lfftw3f
LIBS	+= -lqwt-qt5
LIBS	+= -lportaudio
LIBS	+= -lole32
LIBS	+= -lwinmm
}

unix {
DESTDIR	= ../../../../linux-bin/decoder-plugins
}


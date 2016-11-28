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
	      ./psk-decoder.h \
	      ../decoder-utils/viterbi.h \
	      ../../../includes/filters/iir-filters.h  \
	      ../../../includes/various/oscillator.h \
	      ../../../includes/various/utilities.h \
	      ../../../includes/various/resampler.h \
	      ../../../includes/swradio-constants.h
SOURCES     = ./psk-decoder.cpp \
	      ../decoder-utils/viterbi.cpp \
	      ../../../src/filters/iir-filters.cpp  \
	      ../../../src/various/oscillator.cpp \
	      ../../../src/various/utilities.cpp \
	      ../../../src/various/resampler.cpp \
	      ./main.cpp 
TARGET      = $$qtLibraryTarget(pskdecoder)
FORMS	+= widget.ui

win32 {
DESTDIR     = ../../../../../../windows-bin/decoder-plugins
# includes in mingw differ from the includes in fedora linux
INCLUDEPATH += /usr/i686-w64-mingw32/sys-root/mingw/include
INCLUDEPATH += /usr/i686-w64-mingw32/sys-root/mingw/include/qt5/qwt
LIBS	+= -lqwt-qt5
LIBS	+= -lsamplerate
LIBS	+= -lole32
LIBS	+= -lwinmm
}

unix {
DESTDIR     = ../../../../../linux-bin/decoder-plugins
INCLUDEPATH	+= /usr/include/qt5/qwt
#LIBS	+= -lqwt			# ubuntu 15.04
LIBS	+= -lqwt-qt5			# fedora 21
}


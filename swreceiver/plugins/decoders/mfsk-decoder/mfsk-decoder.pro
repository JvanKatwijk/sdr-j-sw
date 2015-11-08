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
HEADERS     = ./mfsk-decoder.h \
	      ../decoder-interface.h \
	      ../../../includes/filters/fir-filters.h \
	      ../../../includes/filters/fft-filters.h \
	      ../../../includes/various/oscillator.h \
	      ../../../includes/various/fft.h \
	      ../../../includes/various/utilities.h \
	      ../decoder-utils/slidingfft.h \
	      ../decoder-utils/interleave.h \
	      ../decoder-utils/viterbi.h \
	      ../../../includes/swradio-constants.h
SOURCES     = ./mfsk-decoder.cpp \
	      ../../../src/filters/fir-filters.cpp \
	      ../../../src/filters/fft-filters.cpp \
	      ../../../src/various/oscillator.cpp \
	      ../../../src/various/fft.cpp \
	      ../../../src/various/utilities.cpp \
	      ../decoder-utils/slidingfft.cpp \
	      ../decoder-utils/interleave.cpp \
	      ../decoder-utils/viterbi.cpp \
	      ./main.cpp 

TARGET      = $$qtLibraryTarget(mfskdecoder)
FORMS	+= widget.ui

win32 {
DESTDIR     = ../../../../../../windows-bin-sw/decoder-plugins
# includes in mingw differ from the includes in fedora linux
INCLUDEPATH += /usr/i686-w64-mingw32/sys-root/mingw/include
INCLUDEPATH += /usr/i686-w64-mingw32/sys-root/mingw/include/qt5/qwt
LIBS	+= -lfftw3f
LIBS	+= -lqwt-qt5
LIBS	+= -lole32
LIBS	+= -lwinmm
}

unix {
DESTDIR     = ../../../../../linux-bin/decoder-plugins
#LIBS	+= -lfftw3f -lqwt			# ubuntu 15.04
LIBS	+= -lfftw3f -lqwt-qt5			# fedora 21
}


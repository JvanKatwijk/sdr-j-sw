#
TEMPLATE    = lib
CONFIG      += plugin
QT          += core gui widgets

INCLUDEPATH += . \
	       ../ ../.. \
	       ../decoder-utils \
	       ../../../includes \
	       ../../../includes/filters \
	       ../../../includes/various \
	       ../../../includes/scopes-qwt6
HEADERS     = ../decoder-interface.h \
	      ./throb-decoder.h  \
	      ./throb.h \
	      ../../../includes/filters/fir-filters.h \
	      ../../../includes/filters/fft-filters.h \
	      ../../../includes/scopes-qwt6/fft-scope.h \
	      ../../../includes/scopes-qwt6/scope.h \
	      ../../../includes/scopes-qwt6/spectrogramdata.h \
	      ../../../includes/various/utilities.h \
	      ../../../includes/various/fft.h \
	      ../../../includes/various/oscillator.h \
	      ../../../includes/swradio-constants.h
SOURCES     = ./throb-decoder.cpp \ 
	      ./throb.cpp \
	      ../../../src/filters/fir-filters.cpp \
	      ../../../src/filters/fft-filters.cpp \
	      ../../../src/scopes-qwt6/fft-scope.cpp \
	      ../../../src/scopes-qwt6/scope.cpp \
	      ../../../src/various/utilities.cpp \
	      ../../../src/various/fft.cpp \
	      ../../../src/various/oscillator.cpp \
	      ./main.cpp 
TARGET      = $$qtLibraryTarget(throbdecoder)
FORMS	 += widget.ui

win32 {
DESTDIR	= ../../../../../../windows-bin-sw/decoder-plugins
# includes in mingw differ from the includes in fedora linux
INCLUDEPATH += /usr/i686-w64-mingw32/sys-root/mingw/include
INCLUDEPATH += /usr/i686-w64-mingw32/sys-root/mingw/include/qt5/qwt
LIBS	+= -lfftw3f
LIBS	+= -lqwt-qt5
LIBS	+= -lole32
LIBS	+= -lwinmm
}

unix {
DESTDIR	= ../../../../../linux-bin/decoder-plugins
INCLUDEPATH	+= /usr/include/qwt
#LIBS	+= -lqwt -lfftw3f			# ubuntu 15.04
LIBS	+= -lqwt-qt5 -lfftw3f			# fedora 21
}


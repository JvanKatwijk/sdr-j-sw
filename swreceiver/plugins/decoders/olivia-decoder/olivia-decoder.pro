#
TEMPLATE    = lib
CONFIG      += plugin
QT          += core gui widgets

INCLUDEPATH += . \
	       ../ ../.. \
	       ../decoder-utils \
	       ../../../includes \
	       ../../../includes/various
HEADERS     = ../decoder-interface.h \
	       ./olivia-decoder.h \
	      ../../../includes/various/utilities.h \
	      ../../../includes/swradio-constants.h
SOURCES     = ./olivia-decoder.cpp \
	      ../../../src/various/utilities.cpp \
	      ./main.cpp 
TARGET      = $$qtLibraryTarget(oliviadecoder)
FORMS	+= widget.ui

win32 {
DESTDIR     = ../../../../../../windows-bin-sw/decoder-plugins
# includes in mingw differ from the includes in fedora linux
INCLUDEPATH += /usr/i686-w64-mingw32/sys-root/mingw/include
INCLUDEPATH += /usr/i686-w64-mingw32/sys-root/mingw/include/qt5/qwt
LIBS	+= -lole32
LIBS	+= -lwinmm
}

unix {
DESTDIR     = ../../../../../linux-bin/decoder-plugins
}


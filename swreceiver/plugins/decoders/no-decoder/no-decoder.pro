#
TEMPLATE    = lib
CONFIG      += plugin
QT          += core gui widgets

INCLUDEPATH += . \
	       ../ ../../  \
	       ../../../includes 
	       
HEADERS     = ../decoder-interface.h \
	      ./no-decoder.h 
SOURCES     =  ./no-decoder.cpp \
	       ./main.cpp
TARGET      = $$qtLibraryTarget(no_decoder)
FORMS	+= widget.ui

win32 {
DESTDIR     = ../../../../../../windows-bin/decoder-plugins
# includes in mingw differ from the includes in fedora linux
INCLUDEPATH += /usr/i686-w64-mingw32/sys-root/mingw/include
INCLUDEPATH += /usr/i686-w64-mingw32/sys-root/mingw/include/qwt
LIBS	+= -lole32
LIBS	+= -lwinmm
}

unix {
DESTDIR     = ../../../../linux-bin/decoder-plugins
}


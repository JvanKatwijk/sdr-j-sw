#
TEMPLATE    = lib
CONFIG      += plugin
QT          += core gui widgets
INCLUDEPATH += . \
	       ../ ../.. \
	       ../../../includes \
	       ../../../includes/filters \
	       ../../../includes/various
HEADERS     = ../rig-interface.h \
	      ./filereader.h \
	      ./filehulp.h \
	      ../../../includes/filters/fir-filters.h \
	      ../../../includes/various/utilities.h \
	      ../../../includes/swradio-constants.h
SOURCES     = ./filereader.cpp \
	      ./filehulp.cpp 
	      
TARGET      = $$qtLibraryTarget(device_filereader)
FORMS	+= ./widget.ui

win32 {
DESTDIR     = ../../../../../../windows-bin-sw/input-plugins-sw
# includes in mingw differ from the includes in fedora linux
INCLUDEPATH += /usr/i686-w64-mingw32/sys-root/mingw/include
LIBS	+= -lfftw3f
LIBS	+= -lsndfile
LIBS	+= -lsamplerate
LIBS	+= -lole32
LIBS	+= -lwinmm
}

unix{
DESTDIR     = ../../../../linux-bin/input-plugins-sw
LIBS	+= -lsamplerate -lsndfile
}


#
TEMPLATE    = lib
CONFIG      += plugin
QT          += core gui widgets

INCLUDEPATH += . \
	       ../ ../../  \
	       ../../../includes \
	       ../../../includes/filters \
	       ../../../includes/various
	       
HEADERS     = ../decoder-interface.h \
	      ./sstv-decoder.h \
	      ./sstv-worker.h  \
	      ./sstv-buffer.h \
	      ./snr-calc.h \
	      ../../../includes/swradio-constants.h \
	      ../../../includes/filters/iir-filters.h \
	      ../../../includes/filters/fir-filters.h \
	      ../../../includes/various/fft.h \
	      ../../../includes/various/oscillator.h \	
	      ../../../includes/various/ringbuffer.h
SOURCES     =  ./sstv-decoder.cpp \
	       ./sstv-worker.cpp \
	       ./sstv-buffer.cpp \
	       ./snr-calc.cpp \
	       ./sstv-modespec.cpp \
	       ../../../src/filters/iir-filters.cpp \
	       ../../../src/filters/fir-filters.cpp \
	       ../../../src/various/oscillator.cpp \
	       ../../../src/various/fft.cpp \
	       ./main.cpp
TARGET      = $$qtLibraryTarget(sstv_decoder)
FORMS	+= widget.ui

win32 {
DESTDIR     = ../../../../../../windows32-bin/decoder-plugins
# includes in mingw differ from the includes in fedora linux
INCLUDEPATH += /usr/i686-w64-mingw32/sys-root/mingw/include
INCLUDEPATH += /usr/i686-w64-mingw32/sys-root/mingw/include/qwt
LIBS	+= -lole32
LIBS	+= -lwinmm
}

unix {
DESTDIR     = ../../../../../linux-bin/decoder-plugins
}


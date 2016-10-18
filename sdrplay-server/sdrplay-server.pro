#
TEMPLATE    = app
CONFIG      += console
QT          += core gui network widgets
INCLUDEPATH += . \
	      /usr/local/include 
HEADERS     = ./sdrplay-server.h \
	      ./sdrplay-worker.h \
	      ./sdrplay-loader.h \
	      ./ringbuffer.h\
	      ./fir-filters.h
SOURCES     =  ./sdrplay-server.cpp \
	       ./sdrplay-worker.cpp \
	       ./sdrplay-loader.cpp \
	       ./main.cpp \
	       ./fir-filters.cpp
TARGET      = sdrplayServer
FORMS	+= ./widget.ui

unix{
LIBS	+= -lusb-1.0 -ldl
}


#
TEMPLATE    = app
CONFIG      += console
QT          += core gui network widgets
INCLUDEPATH += . \
	      /usr/local/include 
HEADERS     = ./dabstick-server.h \
	      ./dongleselect.h \
	      ./ringbuffer.h\
	      ./fir-filters.h
SOURCES     =  ./dabstick-server.cpp \
	       ./dongleselect.cpp \
	       ./main.cpp \
	       ./fir-filters.cpp
TARGET      = dabstickServer
FORMS	+= ./widget.ui

unix{
LIBS	+= -lusb-1.0 -ldl
}


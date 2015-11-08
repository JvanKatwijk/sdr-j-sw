cmake_minimum_required( VERSION 2.8 )
#set ( CMAKE_BUILD_TYPE Release )
set (objectName device_filereader)
ADD_DEFINITIONS (-DQT_PLUGIN)
ADD_DEFINITIONS (-DQT_SHARED)
ADD_DEFINITIONS (-DQT_NO_DEBUG)
set (CMAKE_INSTALL_PREFIX ../../../../../../../)
add_definitions ( -Wall )

	find_package (Qt5Widgets REQUIRED)
	find_package (Qt5Declarative REQUIRED)

	find_library (DYNAMIC_LOAD dl)
	if(NOT(DYNAMIC_LOAD))
	   message(FATAL_ERROR "please install -ldl")
	else(NOT(DYNAMIC_LOAD))
	   set(extraLibs ${DYNAMIC_LOAD})
	endif(NOT(DYNAMIC_LOAD))
#
	find_library (PTHREADS pthread)
	if(NOT(PTHREADS))
	   message (FATAL_ERROR "please install libpthread")
	else(NOT(PTHREADS))
	   set (extraLibs ${extraLibs} ${PTHREADS})
	endif(NOT(PTHREADS))

	find_library (SNDFILE sndfile)
	if(NOT(SNDFILE))
	   message (FATAL_ERROR "please install libsndfile")
	else(NOT(SNDFILE))
	   set (extraLibs ${extraLibs} ${SNDFILE})
	endif(NOT(SNDFILE))

	find_library (SAMPLERATE samplerate)
	if(NOT(SAMPLERATE))
	   message (FATAL_ERROR "please install libsamplerate")
	else(NOT(SAMPLERATE))
	   set (extraLibs ${extraLibs} ${SAMPLERATE})
	endif(NOT(SAMPLERATE))

	set (${objectName}_UIS ./widget.ui)

	include_directories (
		${CMAKE_SOURCE_DIR}
	        ${CMAKE_CURRENT_BINARY_DIR}
	        ${QT5Widgets_INCLUDES}
		${QT_QTCORE_INCLUDE_DIR}
	        ${QT_QTGUI_INCLUDE_DIR}
	        ${SNDFILES_INCLUDE_DIRS}
	        ${SAMPLERATE_INCLUDE_DIRS}
		.
		../
		../.. 
		../../radio-utils 
		../../../includes 
		../../../includes/filters 
		../../../includes/various 
		../../../includes/output
	)

	set ( ${objectName}_HDRS
		../rig-interface.h 
		./filereader.h 
		./filehulp.h 
		../../../includes/filters/fir-filters.h 
		../../../includes/various/sincos.h 
		../../../includes/various/utilities.h 
		../../../includes/swradio-constants.h
	)

	set ( ${objectName}_SRCS
		./filereader.cpp 
		./filehulp.cpp
		../../../src/filters/fir-filters.cpp 
		../../../src/various/sincos.cpp 
		../../../src/various/utilities.cpp 
	)

	set ( ${objectName}_MOCS
		../rig-interface.h 
		./filereader.h
		./filehulp.h
	)
	
	QT5_WRAP_CPP (MOCS ${${objectName}_MOCS})
	QT5_WRAP_UI(UIS ./widget.ui)

	add_library (${objectName}
	             SHARED ${${objectName}_SRCS}
	             ${UIS} ${SRCS} ${TRS} ${MOCS}
	)
	 
	target_link_libraries (${objectName}
	               Qt5::Widgets
	               ${extraLibs}
	)

	INSTALL(TARGETS ${objectName} LIBRARY DESTINATION ./linux-bin/input-plugins-sw)

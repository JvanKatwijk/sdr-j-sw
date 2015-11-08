
cmake_minimum_required( VERSION 2.8 )
#set ( CMAKE_BUILD_TYPE Release )
set (objectName device_dabstick)
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

	find_library (RTLSDR_DIR
	              NAMES rtlsdr rtl-sdr
	              PATH
	              /usr/local/lib
	              /usr/lib
	)
	if(NOT(RTLSDR_DIR))
	   message (FATAL_ERROR "please install librtlsdr")
	else(NOT(RTLSDR_DIR))
	   set (extraLibs ${extraLibs} ${RTLSDR_DIR})
	endif(NOT(RTLSDR_DIR))
	
	set (${objectName}_UIS ./widget.ui)

	include_directories (
		${CMAKE_SOURCE_DIR}
	        ${CMAKE_CURRENT_BINARY_DIR}
	        ${QT5Widgets_INCLUDES}
		${QT_QTCORE_INCLUDE_DIR}
	        ${QT_QTGUI_INCLUDE_DIR}
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
		./dabstick.h 
		./dongleselect.h 
		../../../includes/filters/fir-filters.h 
		../../../includes/various/sincos.h 
		../../../includes/various/utilities.h 
		../../../includes/swradio-constants.h
	)

	set ( ${objectName}_SRCS
		./dabstick.cpp 
		./dongleselect.cpp
		../../../src/filters/fir-filters.cpp 
		../../../src/various/sincos.cpp 
		../../../src/various/utilities.cpp 
	)

	set ( ${objectName}_MOCS
		../rig-interface.h 
		./dabstick.h
		./dongleselect.h
	)

	QT5_WRAP_UI(UIS ./widget.ui)
	QT5_WRAP_CPP(MOCS ${${objectName}_MOCS})

	add_library (${objectName}
	             SHARED ${${objectName}_SRCS}
	             ${UIS} ${RSCS} ${TRS} ${MOCS}
	)
	 
	target_link_libraries (${objectName}
	               Qt5::Widgets
	               ${extraLibs}
	               ${SAMPLERATE_LIBRARIES}
	)

	INSTALL(TARGETS ${objectName} LIBRARY DESTINATION ./linux-bin/input-plugins-sw)

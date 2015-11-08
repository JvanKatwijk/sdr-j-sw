cmake_minimum_required( VERSION 2.6 )
#set ( CMAKE_BUILD_TYPE Release )
set (objectName device_elektor)
ADD_DEFINITIONS (${QT_DEFINITIONS})
ADD_DEFINITIONS (-DQT_PLUGIN)
ADD_DEFINITIONS (-DQT_SHARED)
ADD_DEFINITIONS (-DQT_NO_DEBUG)
add_definitions ( -Wall )
set (CMAKE_INSTALL_PREFIX ./../../../../../../../)

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

	find_path (FTDI_INCLUDE_DIR NAMES libftdi1/ftdi.h)
	if (NOT(FTDI_INCLUDE_DIR))
	   message (FATAL_ERROR "please install libftdi-devel")
	endif (NOT(FTDI_INCLUDE_DIR)) 

	find_library (FTDI_LIBRARY NAMES libftdi ftdi ftdi1)
	message (STATUS ${FTDI_INCLUDE_DIR} ${FTDI_LIBRARY})
	if (NOT(FTDI_LIBRARY))
	   message (FATAL_ERROR "please install libftdi")
	else (NOT(FTDI_LIBRARY))
	   set (extraLibs ${extraLibs} ${FTDI_LIBRARY})
	endif (NOT(FTDI_LIBRARY)) 

	include(FindPackageHandleStandardArgs)

	set (${objectName}_UIS ./widget.ui)

	include_directories (
		${CMAKE_SOURCE_DIR}
	        ${CMAKE_CURRENT_BINARY_DIR}
	        ${QT5Widgets_INCLUDES}
		${QT_QTCORE_INCLUDE_DIR}
	        ${QT_QTGUI_INCLUDE_DIR}
	        ${FTDI_INCLUDE_DIR}
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
		./elektor.h 
		./band.h
		./frequencytables.h
		../pa-reader.h
		../../../includes/filters/fir-filters.h 
		../../../includes/filters/iir-filters.h 
		../../../includes/various/sincos.h 
		../../../includes/various/utilities.h 
		../../../includes/swradio-constants.h
	)

	set ( ${objectName}_SRCS
		./elektor.cpp 
		./band.cpp
		./frequencytables.cpp
		../pa-reader.cpp
		../../../src/filters/fir-filters.cpp 
		../../../src/filters/iir-filters.cpp 
		../../../src/various/sincos.cpp 
		../../../src/various/utilities.cpp 
	)

	set (${objectName}_UIS ./widget.ui)
	QT5_WRAP_UI(UIS ${${objectName}_UIS})

	set ( ${objectName}_MOCS
		../rig-interface.h 
		./elektor.h
		../pa-reader.h
	)
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

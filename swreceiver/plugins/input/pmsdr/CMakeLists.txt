cmake_minimum_required( VERSION 2.6 )
#set ( CMAKE_BUILD_TYPE Release )
set (objectName device_pmsdr)
ADD_DEFINITIONS (-DQT_PLUGIN)
ADD_DEFINITIONS (-DQT_SHARED)
ADD_DEFINITIONS (-DQT_NO_DEBUG)
set (CMAKE_INSTALL_PREFIX ./../../../../../../../)
add_definitions ( -Wall )

	find_package (Qt5Widgets REQUIRED)
	find_package (Qt5Declarative REQUIRED)
	find_package (Qt5Network REQUIRED)

	find_library (DYNAMIC_LOAD dl)
	if(NOT(DYNAMIC_LOAD))
	   message(FATAL_ERROR "please install -ldl")
	else(NOT(DYNAMIC_LOAD))
	   set(extraLibs ${DYNAMIC_LOAD})
	endif(NOT(DYNAMIC_LOAD))
#

	find_library (DYNAMIC_LOAD dl)
	if(NOT(DYNAMIC_LOAD))
	   message(FATAL_ERROR "please install -ldl")
	else(NOT(DYNAMIC_LOAD))
	   set(extraLibs ${DYNAMIC_LOAD})
	endif(NOT(DYNAMIC_LOAD))


	find_library (PTHREADS pthread)
	if(NOT(PTHREADS))
	   message (FATAL_ERROR "please install libpthread")
	else(NOT(PTHREADS))
	   set (extraLibs ${extraLibs} ${PTHREADS})
	endif(NOT(PTHREADS))

	find_library (LIBUSB
	           NAMES usb-1
	           PATHS
	           /usr/local/lib
	           /usr/lib
	)

	include_directories (
	   ${CMAKE_SOURCE_DIR}
	   ${CMAKE_CURRENT_BINARY_DIR}
	   ${QT5Widgets_INCLUDES}
	   ${QT_QTCORE_INCLUDE_DIR}
	   ${QT_QTGUI_INCLUDE_DIR}
	   .
	   ../
	   ../.. 
	   ../decoder-utils 
	   ../../radio-utils 
	   ../../../includes 
	   ../../../includes/filters 
	   ../../../includes/various 
	)

	set ( ${objectName}_HDRS
	   ../rig-interface.h 
	   ./pmsdr.h 
	   ./pmsdr_usb.h 
	   ./pmsdr_comm.h 
	   ./si570-handler.h 
	   ../pa-reader.h
	   ../../../includes/swradio-constants.h
	   ../../../includes/various/ringbuffer.h 
	)

	set ( ${objectName}_SRCS
	   ./pmsdr.cpp
	   ./pmsdr_usb.cpp 
	   ./pmsdr_comm.cpp 
	   ./si570-handler.cpp 
	   ../pa-reader.cpp
	)

	set (${objectName}_UIS ./widget.ui)
	QT5_WRAP_UI(UIS ${${objectName}_UIS})

	set ( ${objectName}_MOCS
	   ../rig-interface.h 
	   ./pmsdr.h
	   ./pmsdr_usb.h
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
	)


	INSTALL(TARGETS ${objectName} LIBRARY DESTINATION ./linux-bin/input-plugins-sw)

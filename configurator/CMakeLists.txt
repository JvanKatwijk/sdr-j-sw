cmake_minimum_required( VERSION 2.6 )
set ( CMAKE_BUILD_TYPE Release )
set (objectName configure-sw)
set ( CMAKE_INSTALL_PREFIX ../../../../ )
	add_definitions ( -Wall )

	find_package (Qt5Widgets REQUIRED)
	find_package (Qt5Declarative REQUIRED)

	include_directories (
	   ${CMAKE_SOURCE_DIR}
	   ${CMAKE_CURRENT_BINARY_DIR}
	   ${QT_QTCORE_INCLUDE_DIR}
	   ${QT_QTGUI_INCLUDE_DIR}
	   .
	)

	set (${objectName}_HDRS
	   ./configurator.h
	)

	set ( ${objectName}_SRCS
	   ./configurator.cpp
	   ./main.cpp
	)

	set ( ${objectName}_UIS
	   ./configurator.ui
	)

	QT5_WRAP_UI(UIS ${${objectName}_UIS})

	set ( ${objectName}_MOCS
	   ./configurator.h
	)

	QT5_WRAP_CPP(MOCS ${${objectName}_MOCS})

	add_executable (${objectName}
	                ${${objectName}_SRCS}
	                ${UIS}
	                ${SRCS} ${TRS} ${MOCS}
	)

	target_link_libraries (${objectName}
	                       Qt5::Widgets
	)

	INSTALL(TARGETS ${objectName}  DESTINATION ../../linux-bin)


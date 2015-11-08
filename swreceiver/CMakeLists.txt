cmake_minimum_required( VERSION 2.8 )
set ( CMAKE_BUILD_TYPE Release )
set (objectName sdr-j-swreceiver-7.1)
set ( CMAKE_INSTALL_PREFIX ../../../../ )
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
	find_library (QWT_LIBRARY qwt-qt5)
	if (QWT_LIBRARY)
#	Already in cache, be silent
	   set (QWT_FIND_QUIETLY TRUE)
	   set (extraLibs ${extraLibs} ${QWT_LIBRARY})
	endif (QWT_LIBRARY)

	find_path (QWT_INCLUDE_DIR
	           NAMES qwt.h
	           PATHS
	           /usr/local/include/qwt-qt5
	           /usr/local/include/qwt
	           /usr/local/include/qwt6
	           /usr/include/qwt6
	           /usr/include/qwt
	)

	message (STATUS ${QWT_LIBRARY} ${QWT_INCLUDE_DIR})
# handle the QUIETLY and REQUIRED arguments and set QWT_FOUND to TRUE if 
# all listed variables are TRUE
	include ( FindPackageHandleStandardArgs )
	find_package_handle_standard_args (Qwt DEFAULT_MSG QWT_LIBRARY QWT_INCLUDE_DIR )
	MARK_AS_ADVANCED (QWT_LIBRARIES QWT_INCLUDE_DIRS)
#
#  FFTW_INCLUDES    - where to find fftw3f.h
#  FFTW_LIBRARIES   - List of libraries when using FFTW.
#  FFTW_FOUND       - True if FFTW found.

	if (FFTW_INCLUDE_DIRS)
# Already in cache, be silent
	   set (FFTW_FIND_QUIETLY TRUE)
	endif (FFTW_INCLUDE_DIRS)

	find_path (FFTW_INCLUDE_DIR fftw3.h
              HINTS
                  ENV FFTW_ROOT
              PATHS 
              PATH_SUFFIXES
                  include
                  Include
	)
	list( APPEND FFTW_INCLUDE_DIRS ${FFTW_INCLUDE_DIR} )
	find_library (FFTW_LIBRARIES NAMES fftw3f
              HINTS ENV FFTW_ROOT
              PATHS
              PATH_SUFFIXES lib Lib
	)

	message (STATUS ${FFTW_LIBRARIES} ${FFTW_INCLUDE_DIRS})
# handle the QUIETLY and REQUIRED arguments and set FFTW_FOUND to TRUE if
# all listed variables are TRUE

	include (FindPackageHandleStandardArgs)
	find_package_handle_standard_args (FFTW DEFAULT_MSG FFTW_LIBRARIES FFTW_INCLUDE_DIRS)
	mark_as_advanced (FFTW_LIBRARIES FFTW_INCLUDE_DIRS)

	Find_Package(PkgConfig)
	pkg_check_modules(SAMPLERATE samplerate REQUIRED)
	link_directories(${SAMPLERATE})

	find_library (PORTAUDIO portaudio)
	if(NOT(PORTAUDIO))
	   message(FATAL_ERROR "please install portaudio V19")
	else(NOT(PORTAUDIO))
	   set (extraLibs  ${extraLibs} ${PORTAUDIO})
	endif(NOT(PORTAUDIO))

	SET(CMAKE_FIND_LIBRARY_SUFFIXES .so .a)
	find_library(USB1 usb-1.0 HINTS /usr/lib)
	if(NOT(USB1))
	   message(FATAL_ERROR "please install libusb-1.0")
	else(NOT(USB1))
	   set(extraLibs ${extraLibs} ${USB1})
	endif(NOT(USB1))

	find_library (SNDFILE sndfile)
	if(NOT(SNDFILE))
	   message (FATAL_ERROR "please install libsndfile")
	else(NOT(SNDFILE))
	   set (extraLibs ${extraLibs} ${SNDFILE})
	endif(NOT(SNDFILE))

	find_library (PTHREADS pthread)
	if(NOT(PTHREADS))
	   message (FATAL_ERROR "please install libpthread")
	else(NOT(PTHREADS))
	   set (extraLibs ${extraLibs} ${PTHREADS})
	endif(NOT(PTHREADS))

	include_directories (
	   ${CMAKE_SOURCE_DIR}
	   ${CMAKE_CURRENT_BINARY_DIR}
	   ${QT_QTCORE_INCLUDE_DIR}
	   ${QT_QTGUI_INCLUDE_DIR}
	   ${QWT_INCLUDE_DIR}
	   ${FFTW_INCLUDE_DIRS}
	   ${SAMPLERATE_INCLUDE_DIRS}
	   ${PORTAUDIO_INCLUDE_DIRS}
	   ${SNDFILES_INCLUDE_DIRS}
	   .
	   ./plugins 
	   ./plugins/decoders 
	   ./plugins/input 
	   ./includes 
	   ./includes/various 
	   ./includes/output 
	   ./includes/filters 
	   ./includes/scopes-qwt6 
	)

	set ( ${objectName}_HDRS
	   ./includes/swradio-constants.h
	   ./includes/gui.h
	   ./includes/output/pa-writer.h
	   ./includes/various/agchandler.h
	   ./includes/various/ringbuffer.h
	   ./includes/various/fft.h
	   ./includes/various/oscillator.h
	   ./includes/various/sincos.h
	   ./includes/various/utilities.h
	   ./includes/various/resampler.h
	   ./includes/various/attenuator.h
	   ./includes/various/s-meter.h
	   ./includes/various/deviceloader.h
	   ./includes/various/decoderloader.h
	   ./includes/filters/fft-filters.h
	   ./includes/filters/fir-filters.h
	   ./includes/filters/iir-filters.h
	   ./includes/scopes-qwt6/fft-scope.h
	   ./includes/scopes-qwt6/scope.h
	   ./includes/scopes-qwt6/hf-scope.h
	   ./plugins/input/rig-interface.h
           ./plugins/input/pa-reader.h
	   ./plugins/input/no-rig.h
	   ./plugins/decoders/decoder-interface.h 
	)

	set ( ${objectName}_SRCS
	   ./src/main.cpp
	   ./src/gui.cpp
	   ./src/various/deviceloader.cpp
	   ./src/various/decoderloader.cpp
	   ./src/output/pa-writer.cpp
	   ./src/various/agchandler.cpp
	   ./src/various/fft.cpp
	   ./src/various/oscillator.cpp
	   ./src/various/sincos.cpp
	   ./src/various/utilities.cpp
	   ./src/various/resampler.cpp
	   ./src/various/s-meter.cpp
	   ./src/various/attenuator.cpp
	   ./src/filters/fft-filters.cpp
	   ./src/filters/fir-filters.cpp
	   ./src/filters/iir-filters.cpp
	   ./src/scopes-qwt6/fft-scope.cpp
	   ./src/scopes-qwt6/hf-scope.cpp
	   ./src/scopes-qwt6/scope.cpp
	   ./plugins/input/pa-reader.cpp 
	   ./plugins/input/no-rig.cpp
	)

	set (${objectName}_UIS ./src/sdrgui.ui)
	QT5_WRAP_UI(UIS ${${objectName}_UIS})

	set ( ${objectName}_MOCS
	   ./includes/gui.h
	   ./includes/various/deviceloader.h
	   ./includes/various/decoderloader.h
	   ./includes/scopes-qwt6/fft-scope.h
	   ./includes/scopes-qwt6/scope.h
	   ./includes/scopes-qwt6/hf-scope.h
	   ./plugins/input/rig-interface.h
           ./plugins/input/pa-reader.h
	   ./plugins/decoders/decoder-interface.h 
	)
	QT5_WRAP_CPP(MOCS ${${objectName}_MOCS})


	add_executable (${objectName}
	                ${${objectName}_SRCS}
	                ${UIS} ${RSCS} ${TRS} ${MOCS} )

	target_link_libraries (${objectName} 
		               Qt5::Widgets
	                       ${FFTW_LIBRARIES}
	                       ${extraLibs}
	                       ${SAMPLERATE_LIBRARIES}
	                       ${QWT_LIBRARIES}
	)

	INSTALL(TARGETS ${objectName}  DESTINATION ./linux-bin)


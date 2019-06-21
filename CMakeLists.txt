#################### CMakeLists.txt (libopenshot-audio) ######################
# @brief CMake build file for libopenshot-audio (used to generate makefiles)
# @author Jonathan Thomas <jonathan@openshot.org>
#
# @section LICENSE
#
# Copyright (c) 2008-2019 OpenShot Studios, LLC
# <http://www.openshotstudios.com/>. This file is part of
# OpenShot Audio Library (libopenshot-audio), an open-source project dedicated
# to delivering high quality audio editing and playback solutions to the
# world. For more information visit <http://www.openshot.org/>.
#
# OpenShot Audio Library (libopenshot-audio) is free software: you can
# redistribute it and/or modify it under the terms of the GNU General Public
# License as published by the Free Software Foundation, either version 3 of the
# License, or (at your option) any later version.
#
# OpenShot Audio Library (libopenshot-audio) is distributed in the hope that it
# will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
# of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
# Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with OpenShot Audio Library. If not, see <http://www.gnu.org/licenses/>.
################################################################################

cmake_minimum_required(VERSION 3.1...3.14 FATAL_ERROR)

message("\
-----------------------------------------------------------------
          Welcome to the OpenShot Build System!

CMake will now check libopenshot-audio's build dependencies and
inform you of any missing files or other issues.

For more information, please visit <http://www.openshot.org/>.
-----------------------------------------------------------------")

################ ADD CMAKE MODULES ##################
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/Modules")

################ PROJECT VERSION ####################
set(PROJECT_VERSION_FULL "0.1.8-dev1")
set(PROJECT_SO_VERSION 6)

# Remove the dash and anything following, to get the #.#.# version for project()
STRING(REGEX REPLACE "\-.*$" "" VERSION_NUM "${PROJECT_VERSION_FULL}")

################### SETUP PROJECT ###################
# This will define the following variables
# PROJECT_NAME
# PROJECT_VERSION, libopenshot-audio_VERSION
# PROJECT_VERSION_MAJOR, libopenshot-audio_VERSION_MAJOR
# PROJECT_VERSION_MINOR, libopenshot-audio_VERSION_MINOR
# PROJECT_VERSION_PATCH, libopenshot-audio_VERSION_PATCH
PROJECT(libopenshot-audio LANGUAGES C CXX VERSION ${VERSION_NUM})

message("
Generating build files for OpenShot
  Building ${PROJECT_NAME} (version ${PROJECT_VERSION})
  SO/API/ABI Version: ${PROJECT_SO_VERSION}
")


# Define install paths according to system conventions
# XXX: This must be AFTER THE PROJECT() COMMAND w/ languages enabled,
#      in order to properly configure CMAKE_INSTALL_LIBDIR path
include(GNUInstallDirs)


# Juce requires either DEBUG or NDEBUG to be defined on MacOS.
# -DNDEBUG is set by cmake for all release configs, so add
# -DDEBUG for debug builds. We'll do this for all OSes, even
# though only MacOS requires it.
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG")
# Make sure we've picked some build type, default to debug
if(NOT DEFINED CMAKE_BUILD_TYPE OR CMAKE_BUILD_TYPE STREQUAL "")
	set(CMAKE_BUILD_TYPE "Debug")
endif()


# Enable stack-unwinding support in c objects on gcc-based platforms.
# Failing to do so will cause your program to be terminated when a png
# or a jpeg exception is thrown on linux or macosx.
IF (CMAKE_COMPILER_IS_GNUCC)
	SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fexceptions")
ENDIF(CMAKE_COMPILER_IS_GNUCC)

IF (WIN32)
	# Find the base directory of the ASIO SDK (if any)
	find_path(ASIO_SDK_DIR iasiodrv.h PATHS $ENV{ASIO_SDK_DIR} )

	IF (ASIO_SDK_DIR)
		MESSAGE("FOUND ASIO_SDK_DIR: ${ASIO_SDK_DIR}")
		ADD_DEFINITIONS(-DJUCE_ASIO=1)
		INCLUDE_DIRECTORIES(${ASIO_SDK_DIR})
	ELSE(ASIO_SDK_DIR)
		MESSAGE("ASIO_SDK_DIR NOT FOUND")
		ADD_DEFINITIONS(-DJUCE_ASIO=0)
	ENDIF (ASIO_SDK_DIR)

	ADD_DEFINITIONS(-DDONT_AUTOLINK_TO_JUCE_LIBRARY)

	SET(JUCE_PLATFORM_SPECIFIC_LIBRARIES
		advapi32.lib
		comdlg32.lib
		gdi32.lib
		GlU32.lib
		Imm32.dll
		kernel32.lib
		ole32.lib
		OpenGL32.lib
		rpcrt4.lib
		shell32.lib
		Shlwapi.dll
		user32.lib
		vfw32.lib
		version.lib
		winmm.lib
		wininet.lib
		ws2_32.lib
		)
endif()

if(UNIX AND APPLE)
	SET(JUCE_PLATFORM_SPECIFIC_DIR build/macosx/platform_specific_code)
	SET(JUCE_PLATFORM_SPECIFIC_LIBRARIES "-framework Carbon -framework Cocoa -framework CoreFoundation -framework CoreAudio -framework CoreMidi -framework IOKit -framework AGL -framework AudioToolbox -framework QuartzCore -lobjc -lz -framework Accelerate")
	SET(CMAKE_CXX_FLAGS " ${CMAKE_CXX_FLAGS} -flax-vector-conversions")
endif()

if(UNIX AND NOT APPLE) #Linux
	find_package(ALSA REQUIRED)
	include_directories(${ALSA_INCLUDE_DIR})
	ADD_DEFINITIONS(-DLINUX)
	SET(JUCE_PLATFORM_SPECIFIC_LIBRARIES ${ALSA_LIBRARIES})
endif()

# Default extension for source files
if(UNIX AND APPLE)
	SET(SOURCE_EXTENSION "mm")
else ()
    SET(SOURCE_EXTENSION "cpp")
endif()


# List of modules to build
set(JUCE_MODULES
	audio_basics
	audio_devices
	audio_formats
	core
	data_structures
	events )
# Convert to list of source files (extension based on OS)
foreach(j_module IN LISTS JUCE_MODULES)
	list(APPEND JUCE_SOURCES
		JuceLibraryCode/include_juce_${j_module}.${SOURCE_EXTENSION} )
endforeach()

ADD_LIBRARY(openshot-audio SHARED ${JUCE_SOURCES} )

# Include header directories
target_include_directories(openshot-audio PUBLIC
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/JuceLibraryCode>
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/JuceLibraryCode/modules>
		$<INSTALL_INTERFACE:include/libopenshot-audio>
		)

# Set SONAME and other library properties
set_target_properties(openshot-audio
		PROPERTIES
		VERSION ${PROJECT_VERSION}
		SOVERSION ${PROJECT_SO_VERSION}
		CXX_STANDARD 11
		CXX_STANDARD_REQUIRED YES
		CXX_EXTENSIONS OFF
		MACOSX_RPATH OFF
		INSTALL_NAME_DIR "${CMAKE_INSTALL_PREFIX}/lib")

# Find threading library
set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
find_package(Threads REQUIRED)
target_link_libraries(openshot-audio PUBLIC ${CMAKE_THREAD_LIBS_INIT})

# ZLIB
find_package(ZLIB REQUIRED)
include_directories(${ZLIB_INCLUDE_DIR})
target_link_libraries(openshot-audio PUBLIC ${ZLIB_LIBRARIES})

target_link_libraries(openshot-audio PUBLIC
		${CMAKE_DL_LIBS}
		${JUCE_PLATFORM_SPECIFIC_LIBRARIES} )

# PROCESS SUB-DIRECTORIES
add_subdirectory(src)


#################### INSTALLATION #####################

# Install Header Files
INSTALL(FILES
	${CMAKE_CURRENT_SOURCE_DIR}/JuceLibraryCode/JuceHeader.h
	${CMAKE_CURRENT_SOURCE_DIR}/JuceLibraryCode/AppConfig.h
	DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/libopenshot-audio )

INSTALL(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/JuceLibraryCode/modules/
		DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/libopenshot-audio
		FILES_MATCHING PATTERN "*.h" )

# Install library
INSTALL(TARGETS openshot-audio
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
	ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
	COMPONENT library )

################### DOCUMENTATION ###################
# Find Doxygen (used for documentation)
include(cmake/Modules/UseDoxygen.cmake)

# Install manpage
INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/doc/openshot-audio-test-sound.1
	DESTINATION ${CMAKE_INSTALL_MANDIR}/man1 )

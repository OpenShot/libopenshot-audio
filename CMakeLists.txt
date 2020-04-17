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

cmake_minimum_required(VERSION 3.2...3.17 FATAL_ERROR)

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
set(PROJECT_VERSION_FULL "0.2.0-dev2")
set(PROJECT_SO_VERSION 7)

### METADATA
set(PROJECT_AUTHOR "Jonathan Thomas and contributors")
set(PROJECT_AUTHOR_EMAIL "jonathan@openshot.org")
set(PROJECT_MAINTAINER "Jonathan Thomas")
set(PROJECT_MAINTAINER_EMAIL "jonathan@openshot.org")
set(PROJECT_URL "https://github.com/OpenShot/libopenshot/-audio")
set(PROJECT_SUMMARY "OpenShot audio library, based on JUCE")
set(PROJECT_DESCRIPTION [=[
    OpenShot Audio Library (libopenshot-audio) is a free,
    open-source project that enables high-quality editing
    and playback of audio, and is based on the amazing JUCE
    library.
]=])

# Remove the dash and anything following, to get the #.#.# version for project()
string(REGEX REPLACE "\-.*$" "" VERSION_NUM "${PROJECT_VERSION_FULL}")

################### SETUP PROJECT ###################
# This will define the following variables
# PROJECT_NAME
# PROJECT_VERSION, libopenshot-audio_VERSION
# PROJECT_VERSION_MAJOR, libopenshot-audio_VERSION_MAJOR
# PROJECT_VERSION_MINOR, libopenshot-audio_VERSION_MINOR
# PROJECT_VERSION_PATCH, libopenshot-audio_VERSION_PATCH
project(libopenshot-audio LANGUAGES C CXX VERSION ${VERSION_NUM})

# JuceHeader.h needs a hexadecimal version number for the project
if(CMAKE_VERSION VERSION_GREATER 3.13)
  math(EXPR PROJECT_VERSION_HEX
    "(${PROJECT_VERSION_MAJOR} << 16) + \
     (${PROJECT_VERSION_MINOR} << 8) + \
     (${PROJECT_VERSION_PATCH})" OUTPUT_FORMAT HEXADECIMAL )
else()
  # Compile and run a C++ program to generate the hex version
  set(HEX_COMPILE_DEFINITIONS
    -DVERSION_MAJOR=${PROJECT_VERSION_MAJOR}
    -DVERSION_MINOR=${PROJECT_VERSION_MINOR}
    -DVERSION_PATCH=${PROJECT_VERSION_PATCH}
  )
  try_run(HEX_VERSION_RUN HEX_VERSION_BUILD
    ${CMAKE_CURRENT_BINARY_DIR}/hex_version
    ${PROJECT_SOURCE_DIR}/src/hex_version.cpp
    COMPILE_DEFINITIONS ${HEX_COMPILE_DEFINITIONS}
    RUN_OUTPUT_VARIABLE HEX_VERSION_OUTPUT
  )
  if (NOT HEX_VERSION_BUILD)
    message(ERROR "Failed to compile hex-version utility!")
  elseif(HEX_VERSION_RUN STREQUAL FAILED_TO_RUN)
    message(ERROR "Could not execute hex-version utility!")
  else()
    set(PROJECT_VERSION_HEX ${HEX_VERSION_OUTPUT})
  endif()
endif()

message("
Generating build files for OpenShot with CMake ${CMAKE_VERSION}
  Building ${PROJECT_NAME} version ${PROJECT_VERSION_FULL} (${PROJECT_VERSION_HEX})
  SO/API/ABI Version: ${PROJECT_SO_VERSION}
")


# Define install paths according to system conventions
# XXX: This must be AFTER THE PROJECT() COMMAND w/ languages enabled,
#      in order to properly configure CMAKE_INSTALL_LIBDIR path
include(GNUInstallDirs)

# Collect and display summary of options/dependencies
include(FeatureSummary)

################ OPTIONS ##################
# Optional build settings for libopenshot-audio
option(ENABLE_NUGET "Build a NuGet package for the library (Win32 only)" OFF)

########## Configure headers ##############
configure_file(include/JuceHeader.h.in include/JuceHeader.h @ONLY)
configure_file(include/AppConfig.h.in include/AppConfig.h @ONLY)
# We'll want those installed later
install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/include/
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/libopenshot-audio )

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
if (CMAKE_COMPILER_IS_GNUCC)
	SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fexceptions")
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

add_library(openshot-audio SHARED ${JUCE_SOURCES} )

# Include header directories
target_include_directories(openshot-audio PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/JuceLibraryCode/modules>
	$<INSTALL_INTERFACE:include/libopenshot-audio> )

# Set SONAME and other library properties
set_target_properties(openshot-audio
		PROPERTIES
		VERSION ${PROJECT_VERSION}
		SOVERSION ${PROJECT_SO_VERSION}
		CXX_STANDARD 11
		CXX_STANDARD_REQUIRED YES
		CXX_EXTENSIONS OFF
		MACOSX_RPATH OFF
    INSTALL_NAME_DIR "${CMAKE_INSTALL_PREFIX}/lib"
)

# Threading library -- uses IMPORTED target Threads::Threads (since CMake 3.1)
set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package(Threads REQUIRED)
target_link_libraries(openshot-audio PUBLIC Threads::Threads)

# ALSA (Linux only)
if(UNIX AND NOT APPLE)
  find_package(ALSA REQUIRED)
  if (ALSA_FOUND AND NOT TARGET ALSA::ALSA) # CMake < 3.12
    add_library(ALSA::ALSA INTERFACE IMPORTED)
    set_target_properties(ALSA::ALSA PROPERTIES
      INTERFACE_INCLUDE_DIRECTORIES ${ALSA_INCLUDE_DIR}
      INTERFACE_LINK_LIBRARIES ${ALSA_LIBRARIES})
  endif()
  target_compile_definitions(openshot-audio PUBLIC LINUX)
  target_link_libraries(openshot-audio PUBLIC ALSA::ALSA)
endif()

# ZLIB -- uses IMPORTED target ZLIB::ZLIB which has existed since CMake 3.1
find_package(ZLIB REQUIRED)
target_link_libraries(openshot-audio PUBLIC ZLIB::ZLIB)

target_link_libraries(openshot-audio PUBLIC ${CMAKE_DL_LIBS})

# macOS specifics
if(UNIX AND APPLE)
  foreach(_framework
          Carbon Cocoa CoreFoundation CoreAudio CoreMidi
          IOKit AGL AudioToolbox QuartzCore Accelerate)
    target_link_libraries(openshot-audio PUBLIC "-framework ${_framework}")
  endforeach()
  target_link_libraries(openshot-audio PUBLIC objc)
  target_compile_options(openshot-audio PRIVATE -flax-vector-conversions)
endif()

# Windows specifics
if (WIN32)
  # Find the base directory of the ASIO SDK (if any)
  find_path(ASIO_SDK_DIR iasiodrv.h PATHS $ENV{ASIO_SDK_DIR} )

  if(ASIO_SDK_DIR)
    message(STATUS "FOUND ASIO_SDK_DIR: ${ASIO_SDK_DIR}")
    target_compile_definitions(openshot-audio PUBLIC JUCE_ASIO=1)
    target_include_directories(openshot-audio PUBLIC ${ASIO_SDK_DIR})
  else()
  message(STATUS "ASIO_SDK_DIR NOT FOUND")
endif()

  # Order here can be important!
  # For example, winmm.lib must come before kernel32.lib (if linked)
  # or older 32-bit windows versions will have linking issues for
  # certain entry points
  target_link_libraries(openshot-audio
    PUBLIC
      winmm.lib
      ws2_32.lib
      wininet.lib
      version.lib
      Shlwapi.dll
  )

  # Generate package definition file (if enabled)
  if(ENABLE_NUGET)
    configure_file(org.openshot.${PROJECT_NAME}.nuspec.in org.openshot.${PROJECT_NAME}.nuspec @ONLY)
  endif()

endif()

# PROCESS SUB-DIRECTORIES
add_subdirectory(src)


#################### INSTALLATION #####################

# Install Header Files
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/JuceLibraryCode/modules/
	DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/libopenshot-audio
	FILES_MATCHING PATTERN "*.h" )

# Install library
install(TARGETS openshot-audio
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
	ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} )

# Install manpage
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/doc/openshot-audio-test-sound.1
	DESTINATION ${CMAKE_INSTALL_MANDIR}/man1 )

########### DOCUMENTATION ##########
# We need Python to process the source for Doxygen...
find_package(PythonInterp 3)

if(PYTHONINTERP_FOUND)
    # Pre-process the sources to fix documentation formatting
    add_custom_target(process-source-files
        COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/doc/process_source_files.py ${CMAKE_CURRENT_SOURCE_DIR}/JuceLibraryCode/modules "docs"
        COMMENT "Formatting source code for documentation"
        VERBATIM )

    # Processed docs are removed on "make clean"
    set_property(DIRECTORY
        APPEND PROPERTY
        ADDITIONAL_MAKE_CLEAN_FILES "docs" )

    # Find Doxygen (used for documentation)
    include(cmake/Modules/UseDoxygen.cmake)

    # Make sure process-source comes before Doxygen targets,
    # assuming UseDoxygen found the tools and created them
    if(TARGET doc)
        add_dependencies(doc process-source-files)
    endif()
    if(TARGET doxygen)
        add_dependencies(doxygen process-source-files)
    endif()
endif()

# Doxygen was found
if (TARGET doc)
    message(STATUS "Doxygen found, documentation target enabled")
    message("\nTo compile documentation in doc/html, run: 'make doc'")

    # Install docs, if the user builds them with `make doc`
    install(CODE "MESSAGE(\"Checking for documentation files to install...\")")
    install(CODE "MESSAGE(\"(Compile with 'make doc' command, requires Python3 and Doxygen)\")")

    install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/doc/html/
        DESTINATION ${CMAKE_INSTALL_DOCDIR}/API
        MESSAGE_NEVER # Don't spew about file copies
        OPTIONAL )    # No error if the docs aren't found
endif()

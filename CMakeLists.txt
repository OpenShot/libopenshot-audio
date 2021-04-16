#################### CMakeLists.txt (libopenshot-audio) ######################
# @file
# @brief CMake build file for libopenshot-audio (used to generate makefiles)
# @author Jonathan Thomas <jonathan@openshot.org>
# @author FeRD (Frank Dana) <ferdnyc@gmail.com>
#
# Copyright (c) 2008-2020 OpenShot Studios, LLC
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

cmake_minimum_required(VERSION 3.1...3.18 FATAL_ERROR)

message("\
-----------------------------------------------------------------
          Welcome to the OpenShot Build System!

CMake will now check libopenshot-audio's build dependencies and
inform you of any missing files or other issues.

For more information, please visit <http://www.openshot.org/>.
-----------------------------------------------------------------")

################ ADD CMAKE MODULES ##################
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake/Modules")

################ PROJECT VERSION ####################
set(PROJECT_VERSION_FULL "0.2.0-dev4")
set(PROJECT_SO_VERSION 7)

# Remove the dash and anything following, to get the #.#.# version for project()
string(REGEX REPLACE "\-.*$" "" VERSION_NUM "${PROJECT_VERSION_FULL}")

################### SETUP PROJECT ###################
# This will define the following variables
# PROJECT_NAME
# PROJECT_VERSION, OpenShotAudio_VERSION
# PROJECT_VERSION_MAJOR, OpenShotAudio_VERSION_MAJOR
# PROJECT_VERSION_MINOR, OpenShotAudio_VERSION_MINOR
# PROJECT_VERSION_PATCH, OpenShotAudio_VERSION_PATCH
project(OpenShotAudio LANGUAGES C CXX VERSION ${VERSION_NUM})

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
# XXX: This must be AFTER THE project() COMMAND w/ languages enabled,
#      in order to properly configure CMAKE_INSTALL_LIBDIR path
include(GNUInstallDirs)
# Collect and display summary of options/dependencies
include(FeatureSummary)

# Alternative location for JUCE modules (debian has their own)
set(JUCE_MODULES_PATH "${CMAKE_CURRENT_SOURCE_DIR}/JuceLibraryCode/modules" CACHE PATH
    "Location of the JUCE source code 'modules' directory")
if(CMAKE_VERSION VERSION_GREATER 3.4 AND IS_ABSOLUTE ${JUCE_MODULES_PATH})
  # Display path relative to project root
  file(RELATIVE_PATH _juce_dir ${CMAKE_CURRENT_SOURCE_DIR} ${JUCE_MODULES_PATH})
else()
  set(_juce_dir ${JUCE_MODULES_PATH})
endif()
string(REGEX REPLACE "/modules$" "" _juce_dir "${_juce_dir}")
add_feature_info("JUCE path" TRUE "Using JUCE sources from: ${_juce_dir}")

###
### Configure headers
###

# Generate our custom headers from templates
configure_file(include/OpenShotAudio.h.in include/OpenShotAudio.h @ONLY)
if(UNIX)
  # Symlink to the old header name, for backwards compatibility
  file(CREATE_LINK
    OpenShotAudio.h
    "${CMAKE_CURRENT_BINARY_DIR}/include/JuceHeader.h"
    SYMBOLIC
  )
else()
  # We can't symlink it, so just make a copy
  configure_file(include/OpenShotAudio.h.in include/JuceHeader.h @ONLY)
endif()
configure_file(include/AppConfig.h.in include/AppConfig.h @ONLY)
list(APPEND _extra_headers
  ${CMAKE_CURRENT_BINARY_DIR}/include/OpenShotAudio.h
  ${CMAKE_CURRENT_BINARY_DIR}/include/JuceHeader.h
  ${CMAKE_CURRENT_BINARY_DIR}/include/AppConfig.h
)

# Juce requires either DEBUG or NDEBUG to be defined on MacOS.
# -DNDEBUG is set by cmake for all release configs, so add
# -DDEBUG for debug builds. We'll do this for all OSes, even
# though only MacOS requires it.
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG")
# Make sure we've picked some build type, default to release
if(NOT DEFINED CMAKE_BUILD_TYPE OR CMAKE_BUILD_TYPE STREQUAL "")
  set(CMAKE_BUILD_TYPE "Release")
endif()

# Default extension for source files
if(UNIX AND APPLE)
  set(SOURCE_EXTENSION "mm")
else()
  set(SOURCE_EXTENSION "cpp")
endif()

# List of modules to build
set(JUCE_MODULES
  juce_audio_basics
  juce_audio_devices
  juce_audio_formats
  juce_core
  juce_data_structures
  juce_events )
# Convert to list of source files (extension based on OS)
foreach(j_module IN LISTS JUCE_MODULES)
  list(APPEND JUCE_SOURCES
    JuceLibraryCode/include_${j_module}.${SOURCE_EXTENSION} )
endforeach()

add_library(openshot-audio SHARED ${JUCE_SOURCES})

# Include header directories
target_include_directories(openshot-audio PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
  $<BUILD_INTERFACE:${JUCE_MODULES_PATH}>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/libopenshot-audio/>
)

# Set SONAME and other library properties
set_target_properties(openshot-audio PROPERTIES
  VERSION ${PROJECT_VERSION}
  SOVERSION ${PROJECT_SO_VERSION}
  EXPORT_NAME Audio  # Exports as OpenShot::Audio target
)

# Require language features we use
if(CMAKE_VERSION VERSION_GREATER 3.8)
  target_compile_features(openshot-audio PUBLIC
    cxx_std_11
    cxx_range_for
    cxx_override
  )
else()
  set_target_properties(openshot-audio PROPERTIES
    CXX_STANDARD 11
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
  )
endif()

# Enable stack-unwinding support in c objects on gcc-based platforms.
# Failing to do so will cause your program to be terminated when a png
# or a jpeg exception is thrown on linux or macosx.
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  target_compile_options(openshot-audio PRIVATE -fexceptions)
endif()

# This ends up needing to be defined for consumers wishing to use
# JUCE classes in SWIG bindings, since some of the target languages
# (Ruby) have issues dealing with JUCE's use of isFinite()
target_compile_definitions(openshot-audio INTERFACE HAVE_ISFINITE=1)

if(WIN32)
  # Try to load ASIO SDK
  find_package(ASIO)
  if(TARGET ASIO::SDK)
    target_link_libraries(openshot-audio PRIVATE ASIO::SDK)
    set(NEED_ASIO TRUE)
  endif()
  # Order here can be important!
  # For example, winmm.lib must come before kernel32.lib (if linked)
  # or older 32-bit windows versions will have linking issues for
  # certain entry points
  target_link_libraries(openshot-audio PRIVATE
    winmm.lib
    ws2_32.lib
    wininet.lib
    version.lib
    Shlwapi.dll
  )
endif()

if(APPLE)
  set_target_properties(openshot-audio PROPERTIES
    INSTALL_NAME_DIR "${CMAKE_INSTALL_PREFIX}/lib"
    MACOSX_RPATH OFF
  )
  target_link_libraries(openshot-audio PRIVATE
    "-framework Carbon"
    "-framework Cocoa"
    "-framework CoreFoundation"
    "-framework CoreAudio"
    "-framework CoreMidi"
    "-framework IOKit"
    "-framework AGL"
    "-framework AudioToolbox"
    "-framework QuartzCore"
    "-framework Accelerate"
    "-lobjc"
  )
  target_compile_options(openshot-audio PRIVATE
    -flax-vector-conversions)
endif()

# ALSA (Linux only)
if(UNIX AND NOT APPLE)
  set(NEED_ALSA TRUE)
  find_package(ALSA REQUIRED)
  if (ALSA_FOUND AND NOT TARGET ALSA::ALSA) # CMake < 3.12
    add_library(ALSA::ALSA INTERFACE IMPORTED)
    set_target_properties(ALSA::ALSA PROPERTIES
      INTERFACE_INCLUDE_DIRECTORIES ${ALSA_INCLUDE_DIR}
      INTERFACE_LINK_LIBRARIES ${ALSA_LIBRARIES})
  endif()
  target_compile_definitions(openshot-audio PUBLIC LINUX)
  target_link_libraries(openshot-audio PUBLIC ALSA::ALSA)
else()
  # For EXPORTED Config
  set(NEED_ALSA FALSE)
endif()

# ZLIB -- uses IMPORTED target ZLIB::ZLIB which has existed since CMake 3.1
find_package(ZLIB REQUIRED)
target_link_libraries(openshot-audio PUBLIC ZLIB::ZLIB)

# Link with dynamic loader for platform
target_link_libraries(openshot-audio PUBLIC ${CMAKE_DL_LIBS})

# Create an alias so our EXPORT target name works internally, as well
add_library(OpenShot::Audio ALIAS openshot-audio)

# PROCESS SUB-DIRECTORIES
add_subdirectory(src)

###
###
### INSTALLATION
###
###

configure_file(
  "${PROJECT_SOURCE_DIR}/cmake/Modules/FindASIO.cmake"
  FindASIO.cmake @ONLY)

install(TARGETS openshot-audio
  EXPORT OpenShotAudioTargets
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/libopenshot-audio
)

# Install all headers
install(FILES
  ${_extra_headers}
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/libopenshot-audio
)

foreach(j_module IN LISTS JUCE_MODULES)
  install(CODE "message(STATUS \"Installing ${j_module} headers\")")
  install(DIRECTORY ${JUCE_MODULES_PATH}/${j_module}/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/libopenshot-audio/${j_module}
    FILES_MATCHING PATTERN "*.h" )
endforeach()

# Install manpage
install(FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/doc/openshot-audio-demo.1
  DESTINATION ${CMAKE_INSTALL_MANDIR}/man1
)

# Export configuration to build tree
export(EXPORT OpenShotAudioTargets
  FILE OpenShotAudioTargets.cmake
  NAMESPACE OpenShot::
)

# Package the exported targets for external consumers
include(CMakePackageConfigHelpers)

# (Borrowed from cppzmq/CMakeList.txt)
# GNUInstallDirs "DATADIR" wrong here; CMake search path wants "share".
set(CMAKECONFIG_INSTALL_DIR "share/cmake/${PROJECT_NAME}"
  CACHE STRING "install path for CMake configuration")

# Install EXPORTED configuration
install(
  EXPORT OpenShotAudioTargets
  FILE OpenShotAudioTargets.cmake
  NAMESPACE OpenShot::
  DESTINATION ${CMAKECONFIG_INSTALL_DIR}
)
configure_package_config_file(
  cmake/Config.cmake.in
  "${CMAKE_CURRENT_BINARY_DIR}/OpenShotAudioConfig.cmake"
  INSTALL_DESTINATION ${CMAKECONFIG_INSTALL_DIR}
)

# Write version file for eported config
write_basic_package_version_file(
  OpenShotAudioConfigVersion.cmake
  VERSION "${PROJECT_VERSION}"
  COMPATIBILITY AnyNewerVersion
)

# Export the configuration at install time
install(FILES
  "${CMAKE_CURRENT_BINARY_DIR}/OpenShotAudioConfig.cmake"
  "${CMAKE_CURRENT_BINARY_DIR}/OpenShotAudioConfigVersion.cmake"
  DESTINATION ${CMAKECONFIG_INSTALL_DIR}
)

# Include our FindASIO module, which will be needed on Windows
install(FILES
  "${PROJECT_SOURCE_DIR}/cmake/Modules/FindASIO.cmake"
  DESTINATION ${CMAKECONFIG_INSTALL_DIR}
)

###
### DOCUMENTATION
###
# We need Python to process the source for Doxygen...
find_package(PythonInterp 3)

if(PYTHONINTERP_FOUND)
  # Pre-process the sources to fix documentation formatting
  add_custom_target(process-source-files
    COMMAND
      ${PYTHON_EXECUTABLE}
      ${CMAKE_CURRENT_SOURCE_DIR}/doc/process_source_files.py
      ${JUCE_MODULES_PATH}
      "docs"
    COMMENT "Formatting source code for documentation"
    VERBATIM
  )
  add_feature_info("Docs preprocessing" TRUE "Format API docs (requires Python)")

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
if(TARGET doc)
  set(ENABLE_DOCS TRUE)
  message(STATUS "Doxygen found. To generate doc/html in build dir, use the 'doc' target")

  # Install docs, if the user builds them with `make doc`
  install(CODE "MESSAGE(\"Checking for documentation files to install...\")")
  install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/doc/html/
    DESTINATION ${CMAKE_INSTALL_DOCDIR}/API
    MESSAGE_NEVER # Don't spew about file copies
    OPTIONAL      # No error if the docs aren't found
  )
endif()

add_feature_info("API Documentation" ENABLE_DOCS "Building 'doc' target generates doc/html in build dir")

### Display feature summary
feature_summary(WHAT ALL
  INCLUDE_QUIET_PACKAGES
  FATAL_ON_MISSING_REQUIRED_PACKAGES
  DESCRIPTION "Displaying feature summary\n\nBuild configuration:"
)

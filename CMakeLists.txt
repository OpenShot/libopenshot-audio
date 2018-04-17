#################### CMakeLists.txt (libopenshot-audio) ######################
# @brief CMake build file for libopenshot-audio (used to generate makefiles)
# @author Jonathan Thomas <jonathan@openshot.org>
#
# @section LICENSE
#
# Copyright (c) 2008-2016 OpenShot Studios, LLC
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

cmake_minimum_required(VERSION 3.4)

################ ADD CMAKE MODULES ##################
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/Modules")

################ GET VERSION INFORMATION FROM VERSION.H ##################
MESSAGE("--------------------------------------------------------------")
MESSAGE("Determining Version Number (from Version.h file)")

#### Get the lines related to libopenshot version from the Version.h header
file(STRINGS ${CMAKE_CURRENT_SOURCE_DIR}/include/Version.h OPENSHOT_AUDIO_VERSION_LINES
  REGEX "#define[ ]+OPENSHOT_AUDIO_VERSION_.*[0-9]+;.*")

#### Set each line into it's own variable
list (GET OPENSHOT_AUDIO_VERSION_LINES 0 LINE_MAJOR)
list (GET OPENSHOT_AUDIO_VERSION_LINES 1 LINE_MINOR)
list (GET OPENSHOT_AUDIO_VERSION_LINES 2 LINE_BUILD)
list (GET OPENSHOT_AUDIO_VERSION_LINES 3 LINE_SO)

#### Get the version number out of each line
STRING(REGEX REPLACE "#define[ ]+OPENSHOT_AUDIO_VERSION_MAJOR.*([0-9])+;(.*)" "\\1" OPENSHOT_AUDIO_MAJOR_VERSION "${LINE_MAJOR}")
STRING(REGEX REPLACE "#define[ ]+OPENSHOT_AUDIO_VERSION_MINOR.*([0-9])+;(.*)" "\\1" OPENSHOT_AUDIO_MINOR_VERSION "${LINE_MINOR}")
STRING(REGEX REPLACE "#define[ ]+OPENSHOT_AUDIO_VERSION_BUILD.*([0-9])+;(.*)" "\\1" OPENSHOT_AUDIO_BUILD_VERSION "${LINE_BUILD}")
STRING(REGEX REPLACE "#define[ ]+OPENSHOT_AUDIO_VERSION_SO.*([0-9])+;(.*)" "\\1" OPENSHOT_AUDIO_SO_VERSION "${LINE_SO}")
MESSAGE("--> MAJOR Version: ${MAJOR_VERSION}")
MESSAGE("--> MINOR Version: ${MINOR_VERSION}")
MESSAGE("--> BUILD Version: ${BUILD_VERSION}")
MESSAGE("--> SO/API/ABI Version: ${SO_VERSION}")
MESSAGE("--> VERSION: ${PROJECT_VERSION}")
MESSAGE("")

################### SETUP PROJECT ###################
project(openshot-audio
		VERSION "${OPENSHOT_AUDIO_MAJOR_VERSION}.${OPENSHOT_AUDIO_MINOR_VERSION}.${OPENSHOT_AUDIO_BUILD_VERSION}.${OPENSHOT_AUDIO_SO_VERSION}")
MESSAGE("--------------------------------------------------------------")
MESSAGE("---- Generating build files for ${PROJECT_NAME} (${PROJECT_VERSION})")
MESSAGE("--------------------------------------------------------------")

if (NOT CMAKE_BUILD_TYPE)
	SET(CMAKE_BUILD_TYPE "Release")
endif()

ADD_LIBRARY(openshot-audio SHARED)

set_target_properties(openshot-audio PROPERTIES CXX_STANDARD 11)

# Enable stack-unwinding support in c objects on gcc-based platforms.
# Failing to do so will cause your program to be terminated when a png
# or a jpeg exception is thrown on linux or macosx.
IF (CMAKE_COMPILER_IS_GNUCC)
	SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fexceptions")
ENDIF(CMAKE_COMPILER_IS_GNUCC)

IF (WIN32)
	SET_PROPERTY(GLOBAL PROPERTY JUCE_WINDOWS "JUCE_WINDOWS")
	SET_PROPERTY(GLOBAL PROPERTY JUCE_MINGW "JUCE_MINGW")
    SET(EXTENSION "cpp")

	# Find the base directory of the AISO SDK (if any)
	find_path(AISO_SDK_DIR iasiodrv.h
			PATHS $ENV{AISO_SDK_DIR} )

	IF (AISO_SDK_DIR)
		MESSAGE("FOUND AISO_SDK_DIR: ${AISO_SDK_DIR}")
		target_compile_definitions(openshot-audio PUBLIC JUCE_ASIO=1)
		INCLUDE_DIRECTORIES(${AISO_SDK_DIR})
	ELSE(AISO_SDK_DIR)
		MESSAGE("AISO_SDK_DIR NOT FOUND")
		target_compile_definitions(openshot-audio PUBLIC JUCE_ASIO=0)
	ENDIF (AISO_SDK_DIR)

	target_compile_definitions(openshot-audio PUBLIC DONT_AUTOLINK_TO_JUCE_LIBRARY)

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
ELSE (WIN32)
	IF   (UNIX)
		IF   (APPLE)
            SET_PROPERTY(GLOBAL PROPERTY JUCE_MAC "JUCE_MAC")
            target_compile_definitions(openshot-audio PRIVATE NDEBUG)
            SET(EXTENSION "mm")

            SET(JUCE_PLATFORM_SPECIFIC_DIR build/macosx/platform_specific_code)
            SET(JUCE_PLATFORM_SPECIFIC_LIBRARIES "-framework Carbon -framework Cocoa -framework CoreFoundation -framework CoreAudio -framework CoreMidi -framework IOKit -framework AGL -framework AudioToolbox -framework QuartzCore -lobjc -framework Accelerate")

			target_compile_options(openshot-audio PRIVATE -flax-vector-conversions)
        ELSE (APPLE)
			SET_PROPERTY(GLOBAL PROPERTY JUCE_LINUX "JUCE_LINUX")
            SET(EXTENSION "cpp")

			FIND_PACKAGE(X11 REQUIRED)
			target_include_directories(openshot_audio PUBLIC ${X11_INCLUDE_DIR})
			target_include_directories(openshot_audio PUBLIC /usr/include/freetype2)
			target_compile_options(openshot-audio PRIVATE -flax-vector-conversions)
			SET(JUCE_PLATFORM_SPECIFIC_LIBRARIES ${X11_LIBRARIES} asound freetype Xinerama)
		ENDIF(APPLE)
	ENDIF(UNIX)
ENDIF(WIN32)

# Include header directories
target_include_directories(openshot-audio PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/JuceLibraryCode>
	$<INSTALL_INTERFACE:include/libopenshot-audio/JuceLibraryCode>
)

# List of modules to build (Extension based on OS)
FILE(GLOB JUCE_SOURCES
	JuceLibraryCode/modules/juce_core/juce_core.${EXTENSION}
	JuceLibraryCode/modules/juce_events/juce_events.${EXTENSION}
	JuceLibraryCode/modules/juce_audio_basics/juce_audio_basics.${EXTENSION}
	JuceLibraryCode/modules/juce_audio_devices/juce_audio_devices.${EXTENSION}
	JuceLibraryCode/modules/juce_audio_formats/juce_audio_formats.${EXTENSION}
)

target_sources(openshot-audio PRIVATE ${JUCE_SOURCES})

# Disable RPATH
SET(CMAKE_MACOSX_RPATH 0)

# Set SONAME and other library properties
set_target_properties(openshot-audio
			PROPERTIES
			VERSION ${PROJECT_VERSION}
			SOVERSION ${OPENSHOT_AUDIO_SO_VERSION}
			INSTALL_NAME_DIR "${CMAKE_INSTALL_PREFIX}/lib")

set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
find_package(Threads REQUIRED)
target_link_libraries(openshot-audio
	PRIVATE
		${CMAKE_DL_LIBS}
		${CMAKE_THREAD_LIBS_INIT}
	PUBLIC
		${JUCE_PLATFORM_SPECIFIC_LIBRARIES}
)

# PROCESS SUB-DIRECTORIES
add_subdirectory(src)

# Determine correct lib folder
set(LIB_INSTALL_DIR lib${LIB_SUFFIX})

# Install Files
# 👇 Exists a placemarker for FindOpenshot
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/JuceLibraryCode/JuceHeader.h DESTINATION include/libopenshot-audio)
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/JuceLibraryCode DESTINATION include/libopenshot-audio FILES_MATCHING PATTERN "*.h")
install(TARGETS openshot-audio EXPORT Openshot_Audio_Targets
	LIBRARY DESTINATION ${LIB_INSTALL_DIR}
	ARCHIVE DESTINATION ${LIB_INSTALL_DIR}
	RUNTIME DESTINATION ${LIB_INSTALL_DIR}
	INCLUDES DESTINATION include/libopenshot-audio
)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/libopenshot-audio/libopenshot-audioConfigVersion.cmake"
  VERSION ${Upstream_VERSION}
  COMPATIBILITY AnyNewerVersion
)

export(EXPORT Openshot_Audio_Targets
  FILE "${CMAKE_CURRENT_BINARY_DIR}/libopenshot-audio/libopenshot-audioTargets.cmake"
  NAMESPACE Openshot::
)

configure_file(cmake/libopenshot-audioConfig.cmake
  "${CMAKE_CURRENT_BINARY_DIR}/libopenshot-audio/libopenshot-audioConfig.cmake"
  COPYONLY
)

set(ConfigPackageLocation lib/cmake/libopenshot-audio)
install (EXPORT Openshot_Audio_Targets
	FILE libopenshot-audioTargets.cmake
	NAMESPACE Openshot::
	DESTINATION ${ConfigPackageLocation}
)

install(
	FILES
		cmake/libopenshot-audioConfig.cmake
		"${CMAKE_CURRENT_BINARY_DIR}/libopenshot-audio/libopenshot-audioConfigVersion.cmake"
	DESTINATION ${ConfigPackageLocation}
)

################### DOCUMENTATION ###################
# Find Doxygen (used for documentation)
include(cmake/Modules/UseDoxygen.cmake)

# Install Doxygen html documentation
file(GLOB_RECURSE doc_files ${CMAKE_CURRENT_BINARY_DIR}/doc/html/*.*)
INSTALL(FILES ${doc_files} DESTINATION share/doc/libopenshot-audio)

# Install manpage
INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/doc/openshot-audio-test-sound.1 DESTINATION share/man/man1)

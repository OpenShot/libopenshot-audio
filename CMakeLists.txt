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

cmake_minimum_required(VERSION 2.8)

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
STRING(REGEX REPLACE "#define[ ]+OPENSHOT_AUDIO_VERSION_MAJOR.*([0-9])+;(.*)" "\\1" MAJOR_VERSION "${LINE_MAJOR}")
STRING(REGEX REPLACE "#define[ ]+OPENSHOT_AUDIO_VERSION_MINOR.*([0-9])+;(.*)" "\\1" MINOR_VERSION "${LINE_MINOR}")
STRING(REGEX REPLACE "#define[ ]+OPENSHOT_AUDIO_VERSION_BUILD.*([0-9])+;(.*)" "\\1" BUILD_VERSION "${LINE_BUILD}")
STRING(REGEX REPLACE "#define[ ]+OPENSHOT_AUDIO_VERSION_SO.*([0-9])+;(.*)" "\\1" SO_VERSION "${LINE_SO}")
set(PROJECT_VERSION "${MAJOR_VERSION}.${MINOR_VERSION}.${BUILD_VERSION}")

MESSAGE("--> MAJOR Version: ${MAJOR_VERSION}")
MESSAGE("--> MINOR Version: ${MINOR_VERSION}")
MESSAGE("--> BUILD Version: ${BUILD_VERSION}")
MESSAGE("--> SO/API/ABI Version: ${SO_VERSION}")
MESSAGE("--> VERSION: ${PROJECT_VERSION}")
MESSAGE("")

################### SETUP PROJECT ###################
project(openshot-audio)
MESSAGE("--------------------------------------------------------------")
MESSAGE("---- Generating build files for ${PROJECT_NAME} (${PROJECT_VERSION})")
MESSAGE("--------------------------------------------------------------")


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
		ADD_DEFINITIONS(-DJUCE_ASIO=1)
		INCLUDE_DIRECTORIES(${AISO_SDK_DIR})
	ELSE(AISO_SDK_DIR)
		MESSAGE("AISO_SDK_DIR NOT FOUND")
		ADD_DEFINITIONS(-DJUCE_ASIO=0)
	ENDIF (AISO_SDK_DIR)
    
	ADD_DEFINITIONS(-DDONT_AUTOLINK_TO_JUCE_LIBRARY)
	SET(CMAKE_CXX_FLAGS " ${CMAKE_CXX_FLAGS} -std=c++0x")

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
            ADD_DEFINITIONS(-DNDEBUG)
            SET(EXTENSION "mm")
            
            SET(JUCE_PLATFORM_SPECIFIC_DIR build/macosx/platform_specific_code)
            SET(JUCE_PLATFORM_SPECIFIC_LIBRARIES "-framework Carbon -framework Cocoa -framework CoreFoundation -framework CoreAudio -framework CoreMidi -framework IOKit -framework AGL -framework AudioToolbox -framework QuartzCore -lobjc -framework Accelerate")
		
            SET(CMAKE_CXX_FLAGS " ${CMAKE_CXX_FLAGS} -flax-vector-conversions -std=c++0x")
		
        ELSE (APPLE)
		SET_PROPERTY(GLOBAL PROPERTY JUCE_LINUX "JUCE_LINUX")
		SET(EXTENSION "cpp")
            
		find_package(X11 REQUIRED)
		find_package(Freetype REQUIRED)
		find_package(ZLIB REQUIRED)
		find_package(ALSA REQUIRED)
		include_directories(${X11_INCLUDE_DIR} ${FREETYPE_INCLUDE_DIR} ${ZLIB_INCLUDE_DIR} ${ALSA_INCLUDE_DIR})
		ADD_DEFINITIONS(-DLINUX)
		SET(JUCE_PLATFORM_SPECIFIC_LIBRARIES ${X11_LIBRARIES} ${FREETYPE_LIBRARIES} ${ZLIB_LIBRARIES} ${ALSA_LIBRARIES})
	ENDIF(APPLE)
    ENDIF(UNIX)
ENDIF(WIN32)

# Include header directories
include_directories("JuceLibraryCode" "JuceLibraryCode/modules")

# List of modules to build (Extension based on OS)
FILE(GLOB JUCE_SOURCES
	JuceLibraryCode/include_juce_*.${EXTENSION})

# Disable RPATH
SET(CMAKE_MACOSX_RPATH 0)

ADD_LIBRARY(openshot-audio SHARED
	${JUCE_SOURCES}
)

# Set SONAME and other library properties
set_target_properties(openshot-audio
			PROPERTIES
			VERSION ${PROJECT_VERSION}
			SOVERSION ${SO_VERSION}
			INSTALL_NAME_DIR "${CMAKE_INSTALL_PREFIX}/lib")

set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
find_package(Threads REQUIRED)
TARGET_LINK_LIBRARIES(openshot-audio
	${CMAKE_DL_LIBS}
	${CMAKE_THREAD_LIBS_INIT}
	${JUCE_PLATFORM_SPECIFIC_LIBRARIES}
)

# PROCESS SUB-DIRECTORIES
add_subdirectory(src)

# Determine correct lib folder
set(LIB_INSTALL_DIR lib${LIB_SUFFIX}) 

# Install Files
INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/JuceLibraryCode/JuceHeader.h DESTINATION include/libopenshot-audio)
INSTALL(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/JuceLibraryCode DESTINATION include/libopenshot-audio FILES_MATCHING PATTERN "*.h")
INSTALL(TARGETS openshot-audio DESTINATION ${LIB_INSTALL_DIR})

################### DOCUMENTATION ###################
# Find Doxygen (used for documentation)
include(cmake/Modules/UseDoxygen.cmake)

# Install Doxygen html documentation
file(GLOB_RECURSE doc_files ${CMAKE_CURRENT_BINARY_DIR}/doc/html/*.*)
INSTALL(FILES ${doc_files} DESTINATION share/doc/libopenshot-audio)

# Install manpage
INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/doc/openshot-audio-test-sound.1 DESTINATION share/man/man1)

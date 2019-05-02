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

include(GNUInstallDirs)

# Determine file extension for source files
if(UNIX AND APPLE)
    set(EXTENSION "mm")
else()
    set(EXTENSION "cpp")
endif()

# List of modules to build (Extension based on OS)
file(GLOB JUCE_SOURCES include_juce_*.${EXTENSION})

# Create our target for building the library
add_library(OpenShotAudio SHARED ${JUCE_SOURCES})

# Set SONAME and other library properties
set_target_properties(OpenShotAudio PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${SO_VERSION}
    CXX_STANDARD 11
    CXX_STANDARD_REQUIRED YES
    MACOSX_RPATH 0
    PUBLIC_HEADER JuceHeader.h
    PUBLIC_HEADER AppConfig.h
    INSTALL_NAME_DIR ${CMAKE_INSTALL_LIBDIR}
)

# Include header directories
target_include_directories(OpenShotAudio PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/modules>
    $<INSTALL_INTERFACE:include/libopenshot-audio>
)

# Enable stack-unwinding support in c objects on gcc-based platforms.
# Failing to do so will cause your program to be terminated when a png
# or a jpeg exception is thrown on linux or macosx.
IF (CMAKE_COMPILER_IS_GNUCC)
    target_compile_options(OpenShotAudio PRIVATE "-fexceptions")
ENDIF(CMAKE_COMPILER_IS_GNUCC)

# Find common libraries
set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
find_package(Threads REQUIRED)
find_package(ZLIB REQUIRED)
target_link_libraries(OpenShotAudio ZLIB::ZLIB Threads::Threads)

if(WIN32)
    SET_PROPERTY(GLOBAL PROPERTY JUCE_WINDOWS "JUCE_WINDOWS")
    SET_PROPERTY(GLOBAL PROPERTY JUCE_MINGW "JUCE_MINGW")

    # Find the base directory of the AISO SDK (if any)
    find_path(ASIO_SDK_DIR iasiodrv.h
              PATHS $ENV{ASIO_SDK_DIR} )

    if(ASIO_SDK_DIR)
        MESSAGE(STATUS "FOUND ASIO_SDK_DIR: ${ASIO_SDK_DIR}")
        target_compile_definitions(OpenShotAudio PRIVATE JUCE_ASIO=1)
        target_include_directories(OpenShotAudio PRIVATE ${ASIO_SDK_DIR})
    else()
        MESSAGE(STATUS "ASIO_SDK_DIR NOT FOUND")
        target_compile_definitions(OpenShotAudio PRIVATE JUCE_ASIO=0)
    endif()

    target_compile_definitions(OpenShotAudio PUBLIC DONT_AUTOLINK_TO_JUCE_LIBRARY)

    target_link_libraries(OpenShotAudio
        advapi32.lib comdlg32.lib gdi32.lib GlU32.lib Imm32.dll
        kernel32.lib ole32.lib OpenGL32.lib rpcrt4.lib shell32.lib
        Shlwapi.dll user32.lib vfw32.lib version.lib winmm.lib
        wininet.lib ws2_32.lib)
endif()

if(UNIX AND APPLE)
    SET_PROPERTY(GLOBAL PROPERTY JUCE_MAC "JUCE_MAC")
    target_compile_definitions(OpenShotAudio PRIVATE NDEBUG)

    SET(JUCE_PLATFORM_SPECIFIC_DIR build/macosx/platform_specific_code)
    target_link_libraries(OpenShotAudio "-framework Carbon -framework Cocoa -framework CoreFoundation -framework CoreAudio -framework CoreMidi -framework IOKit -framework AGL -framework AudioToolbox -framework QuartzCore -lobjc -lz -framework Accelerate")
    target_compile_options(OpenShotAudio PRIVATE "-flax-vector-conversions")

endif()

if(UNIX AND NOT APPLE)
    SET_PROPERTY(GLOBAL PROPERTY JUCE_LINUX "JUCE_LINUX")
    target_compile_definitions(OpenShotAudio PRIVATE LINUX)

    find_package(ALSA REQUIRED)
    target_link_libraries(OpenShotAudio ALSA::ALSA)
endif()

# Link with the library that provides dlopen()/dlclose()
target_link_libraries(OpenShotAudio ${CMAKE_DL_LIBS})

##################### INSTALL #####################

install(TARGETS OpenShotAudio EXPORT OpenShotAudio
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/libopenshot-Audio
        COMPONENT Devel
)

# Header files (Devel component)
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/modules/
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/libopenshot-audio
        COMPONENT Devel
        FILES_MATCHING PATTERN "*.h")

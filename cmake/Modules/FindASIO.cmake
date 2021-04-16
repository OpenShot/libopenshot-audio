# vim: ts=2 sw=2
#[=======================================================================[.rst:
FindASIO
----------
Try to find the ASIO SDK, if installed (Windows only)

IMPORTED targets
^^^^^^^^^^^^^^^^

This module defines :prop_tgt:`IMPORTED` target ``ASIO::SDK``.


Result Variables
^^^^^^^^^^^^^^^^

This module defines the following variables:

::

  ASIO_FOUND         - The ASIO SDK headers have been located
  ASIO_INCLUDE_DIRS  - Include directory necessary for using ASIO APIs
  ASIO_SDK_DIR       - Root of the ASIO SDK distribution


TODO: Not currently defined (pending code for determining the SDK version)

::

  ASIO_VERSION      - The ASIO SDK version found

Copyright (c) 2020, FeRD (Frank Dana) <ferdnyc@gmail.com>

Redistribution and use is allowed according to the terms of the BSD license.
For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#]=======================================================================]
include(FindPackageHandleStandardArgs)

if(NOT WIN32)
  # ASIO is Win32 only
  if(NOT QUIET)
    message(STATUS "Skipping ASIO detection on unsupported platform")
  endif()
  return()
endif()

find_path(ASIO_COMMON_INCLUDE_DIR iasiodrv.h
  PATHS
    $ENV{ASIO_SDK_DIR}
    ${ASIO_ROOT}
    ${ASIO_SDK_DIR}
    ${ASIO_DIR}
    thirdparty
  PATH_SUFFIXES
    asiosdk*/common
    asio*/common
    asio*
    common
    )

if(ASIO_COMMON_INCLUDE_DIR)
  file(TO_CMAKE_PATH ${ASIO_COMMON_INCLUDE_DIR}/../ ASIO_SDK_DIR)

  # cache the vars.
  set(ASIO_SDK_DIR ${ASIO_SDK_DIR} CACHE PATH "The directory containing the ASIO SDK files" FORCE)
  set(ASIO_INCLUDE_DIRS ${ASIO_COMMON_INCLUDE_DIR} CACHE PATH "The include prefix necessary to use ASIO APIs" FORCE)

  # TODO: Determine this from the headers
  # set(ASIO_VERSION  CACHE STRING "The ${_component} version number.")

  mark_as_advanced(
    ASIO_INCLUDE_DIRS
    ASIO_SDK_DIR
    # ASIO_VERSION
    )
endif()

# Give a nice error message if some of the required vars are missing.
find_package_handle_standard_args(ASIO DEFAULT_MSG ASIO_SDK_DIR)

# Export target for configuration
if(ASIO_FOUND)
  if(NOT TARGET ASIO::SDK)
    message(STATUS "Creating IMPORTED target ASIO::SDK")

    add_library(ASIO::SDK INTERFACE IMPORTED)

    set_target_properties(ASIO::SDK PROPERTIES
      INTERFACE_INCLUDE_DIRECTORIES "${ASIO_COMMON_INCLUDE_DIR}")

    set_property(TARGET ASIO::SDK APPEND PROPERTY
      INTERFACE_COMPILE_DEFINITIONS "JUCE_ASIO=1")

    set_property(TARGET ASIO::SDK APPEND PROPERTY
      INTERFACE_COMPILE_DEFINITIONS "ASIO_SDK_DIR=${ASIO_SDK_DIR}")
  endif()
endif()

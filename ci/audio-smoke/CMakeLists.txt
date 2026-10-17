# SPDX-FileCopyrightText: 2026 OpenShot Studios, LLC
# SPDX-License-Identifier: LGPL-3.0-or-later

cmake_minimum_required(VERSION 3.16)
project(AudioSmokeTest LANGUAGES CXX)
find_package(OpenShotAudio REQUIRED)
option(REQUIRE_ARM64 "Require a native ARM64 test executable" OFF)
add_executable(audio-smoke main.cpp)
target_compile_features(audio-smoke PRIVATE cxx_std_17)
target_link_libraries(audio-smoke PRIVATE OpenShot::Audio)
if(REQUIRE_ARM64)
  target_compile_definitions(audio-smoke PRIVATE REQUIRE_ARM64=1)
endif()
enable_testing()
add_test(NAME audio-buffer-roundtrip COMMAND audio-smoke)

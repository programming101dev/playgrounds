cmake_minimum_required(VERSION 3.14)
project(p101-track-c-complex-components-tests C)
enable_testing()

if (NOT DEFINED P101_SOURCE_BUILD_DIR)
    message(FATAL_ERROR "P101_SOURCE_BUILD_DIR is required; configure the track with P101_BUILD_LEVEL=2")
endif ()
set(_P101_TRACK_BINARY "${P101_SOURCE_BUILD_DIR}/p101-track-c-complex-components")
if (NOT EXISTS "${_P101_TRACK_BINARY}")
    message(FATAL_ERROR "Track binary not found: ${_P101_TRACK_BINARY}. Build the track first.")
endif ()

add_test(NAME track-runs COMMAND "${_P101_TRACK_BINARY}")

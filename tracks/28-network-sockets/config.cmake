set(PROJECT_NAME "p101-track-network-sockets")
set(PROJECT_VERSION "1.0.0")
set(PROJECT_DESCRIPTION "Standalone p101 playground track: network-sockets")
set(PROJECT_LANGUAGE "C")

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

set(STANDARD_FLAGS
        -D_POSIX_C_SOURCE=200809L
        -D_XOPEN_SOURCE=700
        -Werror
)

set(DARWIN_STANDARD_FLAGS
        -D_DARWIN_C_SOURCE
)

set(LINUX_STANDARD_FLAGS)
set(BSD_STANDARD_FLAGS)

get_filename_component(P101_WORKSPACE_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../../.." ABSOLUTE)
file(GLOB _P101_TRACK_LIBRARY_REPOS LIST_DIRECTORIES true "${P101_WORKSPACE_ROOT}/libraries/lib_*")
list(SORT _P101_TRACK_LIBRARY_REPOS)
set(_P101_TRACK_INCLUDE_DIRS "")
set(_P101_TRACK_LINK_DIRS "")
foreach (_p101_library_repo IN LISTS _P101_TRACK_LIBRARY_REPOS)
    get_filename_component(_p101_library_name "${_p101_library_repo}" NAME)
    if (_p101_library_name MATCHES "^lib_(posix|posix_optional|posix_xsi|unix)$")
        continue()
    endif ()
    if (IS_DIRECTORY "${_p101_library_repo}/include")
        list(APPEND _P101_TRACK_INCLUDE_DIRS "${_p101_library_repo}/include")
    endif ()
    if (EXISTS "${_p101_library_repo}/.last-build-dir")
        file(READ "${_p101_library_repo}/.last-build-dir" _p101_last_build_dir)
        string(STRIP "${_p101_last_build_dir}" _p101_last_build_dir)
        if (IS_DIRECTORY "${_p101_library_repo}/${_p101_last_build_dir}")
            list(APPEND _P101_TRACK_LINK_DIRS "${_p101_library_repo}/${_p101_last_build_dir}")
        endif ()
    endif ()
    file(GLOB _p101_library_build_dirs LIST_DIRECTORIES true "${_p101_library_repo}/build*")
    list(SORT _p101_library_build_dirs)
    foreach (_p101_library_build_dir IN LISTS _p101_library_build_dirs)
        if (IS_DIRECTORY "${_p101_library_build_dir}")
            list(APPEND _P101_TRACK_LINK_DIRS "${_p101_library_build_dir}")
        endif ()
    endforeach ()
endforeach ()
list(REMOVE_DUPLICATES _P101_TRACK_INCLUDE_DIRS)
list(REMOVE_DUPLICATES _P101_TRACK_LINK_DIRS)
set(P101_PUBLIC_INCLUDE_DIRS "${_P101_TRACK_INCLUDE_DIRS}" CACHE STRING "Track p101 include dirs" FORCE)
set(P101_PUBLIC_LINK_DIRS "${_P101_TRACK_LINK_DIRS}" CACHE STRING "Track p101 link dirs" FORCE)

set(EXECUTABLE_TARGETS main)
set(LIBRARY_TARGETS "")
set(main_OUTPUT_NAME p101-track-network-sockets)

set(main_SOURCES
        src/main.c
)

set(main_HEADERS
        include/track_info.h
)

set(main_LINK_LIBRARIES
        p101_error
        p101_env
        p101_tool_event
        p101_c
        p101_io
        p101_filesystem
        p101_memory
        p101_process
        p101_thread
        p101_sync
        p101_ipc
        p101_network
        p101_terminal
        p101_time
        p101_identity
        p101_text
        p101_locale
        p101_math
        p101_search
        p101_dynamic_linking
        p101_diagnostics
        p101_database
        p101_cli
        p101_random
        p101_host
        p101_fsm
        p101_util
        p101_convert
        p101_c_facts
        m
)

set(PROJECT_NAME "p101-track-file-io")
set(PROJECT_VERSION "1.0.0")
set(PROJECT_DESCRIPTION "Standalone p101 playground track: file-io")
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
set(main_OUTPUT_NAME p101-track-file-io)

set(main_SOURCES
        src/main.c
)

set(main_HEADERS
        include/track_info.h
)

set(main_LINK_LIBRARIES
        p101_error
        p101_env
        p101_c
        p101_posix
        p101_posix_optional
        p101_posix_xsi
        p101_unix
        p101_fsm
        p101_util
        p101_convert
        p101_c_facts
        m
)

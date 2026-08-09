set(PROJECT_NAME "p101-tool-playground")
set(PROJECT_VERSION "1.0.0")
set(PROJECT_DESCRIPTION "Full-featured playground for the p101 runtime analysis tools")
set(PROJECT_LANGUAGE "C")

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# Common compiler flags
set(STANDARD_FLAGS
        -D_POSIX_C_SOURCE=200809L
        -D_XOPEN_SOURCE=700
        -Werror
)

set(DARWIN_STANDARD_FLAGS
        -D_DARWIN_C_SOURCE
)

set(LINUX_STANDARD_FLAGS
)

set(BSD_STANDARD_FLAGS
)

# Define targets
set(EXECUTABLE_TARGETS main)
set(LIBRARY_TARGETS "")
set(main_OUTPUT_NAME p101-tool-playground)

set(main_SOURCES
        src/cli.c
        src/lesson_demo.c
        src/main.c
        src/ownership_defect_demo.c
        src/playground.c
        src/playground_support.c
        src/resource_defect_demo.c
        src/runtime_demo.c
        src/scenario.c
)

set(main_HEADERS
        include/arguments.h
        include/cli.h
        include/constants.h
        include/errors.h
        include/playground.h
        include/scenario.h
)

set(main_LINK_LIBRARIES
        p101_error
        p101_env
        p101_c
        p101_cli
        p101_database
        p101_diagnostics
        p101_dynamic_linking
        p101_filesystem
        p101_host
        p101_identity
        p101_io
        p101_ipc
        p101_locale
        p101_math
        p101_memory
        p101_network
        p101_process
        p101_random
        p101_search
        p101_sync
        p101_terminal
        p101_text
        p101_thread
        p101_time
        p101_convert
        m
)

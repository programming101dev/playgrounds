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
        src/main.c
        src/playground.c
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
        p101_posix
        p101_posix_optional
        p101_posix_xsi
        p101_unix
        p101_convert
        m
)

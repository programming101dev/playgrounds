#ifndef P101_TOOL_PLAYGROUND_ARGUMENTS_H
#define P101_TOOL_PLAYGROUND_ARGUMENTS_H

#include <stdbool.h>

enum playground_scenario
{
    SCENARIO_TOUR = 0,
    SCENARIO_CLEAN_FILE,
    SCENARIO_REALLOC,
    SCENARIO_PIPE,
    SCENARIO_FORK,
    SCENARIO_FD_LEAK,
    SCENARIO_ALLOC_LEAK,
    SCENARIO_DOUBLE_CLOSE,
    SCENARIO_STRAY_CLOSE,
    SCENARIO_FAULT_LAB,
    SCENARIO_EARLY_RETURN_FD_LEAK,
    SCENARIO_EARLY_RETURN_ALLOC_LEAK,
    SCENARIO_PARTIAL_CLEANUP,
    SCENARIO_REALLOC_LEAK,
    SCENARIO_EXEC_INHERIT,
    SCENARIO_DOUBLE_FREE,
    SCENARIO_STRAY_FREE,
    SCENARIO_SIZEOF_POINTER,
    SCENARIO_IGNORE_READ_COUNT,
    SCENARIO_UNSAFE_LOG_SECRET,
    SCENARIO_LOG_INJECTION,
    SCENARIO_MISSING_STRUCTURED_LOG,
    SCENARIO_INPUT_VALIDATION,
    SCENARIO_COMMAND_INJECTION,
    SCENARIO_PREDICTABLE_TEMP_FILE,
    SCENARIO_SIGNED_CONVERSION,
    SCENARIO_TRUNCATION
};

struct arguments
{
    const char              *scenario_str;
    const char              *output_path;
    const char              *bytes_str;
    const char              *repeats_str;
    enum playground_scenario scenario;
    unsigned int             bytes;
    unsigned int             repeats;
    bool                     verbose;
};

#endif    // P101_TOOL_PLAYGROUND_ARGUMENTS_H

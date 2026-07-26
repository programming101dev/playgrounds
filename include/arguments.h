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
    SCENARIO_FAULT_LAB
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

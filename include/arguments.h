#ifndef P101_TOOL_PLAYGROUND_ARGUMENTS_H
#define P101_TOOL_PLAYGROUND_ARGUMENTS_H

#include <stdbool.h>

enum playground_scenario
{
#define P101_SCENARIO(identifier, name, behavior, description) identifier,
#include "playground_scenarios.def"
#undef P101_SCENARIO
    SCENARIO_COUNT
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
    bool                     show_help;
    bool                     show_scenario_manifest;
};

#endif    // P101_TOOL_PLAYGROUND_ARGUMENTS_H

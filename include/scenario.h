#ifndef P101_TOOL_PLAYGROUND_SCENARIO_H
#define P101_TOOL_PLAYGROUND_SCENARIO_H

#include "arguments.h"
#include <p101_env/env.h>
#include <p101_error/error.h>
#include <stdbool.h>
#include <stdio.h>

enum playground_scenario_behavior
{
    P101_SCENARIO_EXECUTABLE_CLEAN,
    P101_SCENARIO_EXECUTABLE_DEFECT,
    P101_SCENARIO_MODELED_DEFECT
};

enum playground_scenario          p101_tool_playground_scenario_from_name(const struct p101_env *env, const char *name, bool *ok);
const char                       *p101_tool_playground_scenario_name(enum playground_scenario scenario);
enum playground_scenario_behavior p101_tool_playground_scenario_behavior(enum playground_scenario scenario);
void                              p101_tool_playground_print_scenarios(const struct p101_env *env, struct p101_error *err, FILE *stream);

#endif    // P101_TOOL_PLAYGROUND_SCENARIO_H

#ifndef P101_TOOL_PLAYGROUND_SCENARIO_H
#define P101_TOOL_PLAYGROUND_SCENARIO_H

#include "arguments.h"
#include <p101_env/env.h>
#include <p101_error/error.h>
#include <stdbool.h>
#include <stdio.h>

enum playground_scenario p101_tool_playground_scenario_from_name(const struct p101_env *env, const char *name, bool *ok);
const char              *p101_tool_playground_scenario_name(enum playground_scenario scenario);
void                     p101_tool_playground_print_scenarios(const struct p101_env *env, struct p101_error *err, FILE *stream);
void                     p101_tool_playground_write_scenario_manifest(const struct p101_env *env, struct p101_error *err, FILE *stream);

#endif    // P101_TOOL_PLAYGROUND_SCENARIO_H

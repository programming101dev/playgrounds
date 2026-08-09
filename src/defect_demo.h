#ifndef P101_TOOL_PLAYGROUND_DEFECT_DEMO_H
#define P101_TOOL_PLAYGROUND_DEFECT_DEMO_H

#include "arguments.h"
#include <p101_env/env.h>
#include <p101_error/error.h>

int p101_tool_playground_run_resource_defect_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args);
int p101_tool_playground_run_ownership_defect_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args);

#endif    // P101_TOOL_PLAYGROUND_DEFECT_DEMO_H

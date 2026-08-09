#ifndef P101_TOOL_PLAYGROUND_LESSON_DEMO_H
#define P101_TOOL_PLAYGROUND_LESSON_DEMO_H

#include "arguments.h"
#include <p101_env/env.h>
#include <p101_error/error.h>

int p101_tool_playground_run_lesson_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args);
int p101_tool_playground_write_text_output(const struct p101_env *env, struct p101_error *err, const struct arguments *args, const char *text);

#endif    // P101_TOOL_PLAYGROUND_LESSON_DEMO_H

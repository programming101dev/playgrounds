#ifndef P101_TOOL_PLAYGROUND_SUPPORT_H
#define P101_TOOL_PLAYGROUND_SUPPORT_H

#include "arguments.h"
#include <p101_env/env.h>
#include <p101_error/error.h>
#include <stdbool.h>
#include <stddef.h>

int   run_orientation_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args);
int   run_clean_file(const struct p101_env *env, struct p101_error *err, const struct arguments *args);
int   run_c_memory_runtime_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args);
int   p101_tool_playground_support_write_demo_file(const struct p101_env *env, struct p101_error *err, const struct arguments *args, const char *label, bool leak_fd, bool leak_alloc);
char *p101_tool_playground_support_make_buffer(const struct p101_env *env, struct p101_error *err, size_t bytes, char fill);
void  p101_tool_playground_support_close_fd_preserving_error(const struct p101_env *env, struct p101_error *err, int *fd);

#endif    // P101_TOOL_PLAYGROUND_SUPPORT_H

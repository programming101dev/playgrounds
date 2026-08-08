/*
 * libFuzzer harness for p101-tool-playground's argument parser.
 */
#include "cli.h"
#include "constants.h"
#include <p101_c/p101_stdlib.h>
#include <p101_c/p101_string.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define FUZZ_MAX_ARGS 64

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    char              *buf;
    char              *argv[FUZZ_MAX_ARGS];
    int                argc;
    char              *p;
    struct p101_error *err;
    struct p101_env   *env;
    struct arguments   args;

    err = p101_error_create(false);
    env = p101_env_create(err, NULL);
    buf = (char *)p101_malloc(env, err, size + 1U);

    if(buf == NULL)
    {
        goto done;
    }

    p101_memcpy(env, buf, data, size);
    buf[size] = '\0';
    argv[0]   = (char *)"p101-tool-playground";
    argc      = 1;
    p         = buf;

    /*
     * Every NUL-terminated segment of the fuzz buffer is one argument, so an
     * empty segment reaches the parser as an empty-string argument and a
     * segment may carry whitespace.
     */
    while(p < buf + size && argc < FUZZ_MAX_ARGS - 1)
    {
        argv[argc] = p;
        argc++;
        p += p101_strlen(env, p) + 1U;
    }

    argv[argc] = NULL;

#ifdef __GLIBC__
    optind = 0;
#else
    {
        extern int optreset;
        optreset = 1;
        optind   = 1;
    }
#endif

    p101_memset(env, &args, 0, sizeof(args));
    args.scenario_str = DEFAULT_SCENARIO;
    args.output_path  = DEFAULT_OUTPUT_PATH;
    args.bytes        = DEFAULT_BYTES;
    args.repeats      = DEFAULT_REPEATS;

    p101_tool_playground_parse_arguments(env, err, argc, argv, &args);

    if(!args.show_help && p101_error_has_no_error(err))
    {
        p101_tool_playground_check_arguments(env, err, &args);
    }

    if(!args.show_help && p101_error_has_no_error(err))
    {
        p101_tool_playground_convert_arguments(env, err, &args);
    }

done:
    p101_free(env, buf);
    p101_env_destroy(env);
    p101_error_destroy(err);
    return 0;
}

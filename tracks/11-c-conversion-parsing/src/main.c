#include "track_info.h"
#include <p101_c/p101_stdio.h>
#include <p101_c/p101_stdlib.h>
#include <p101_env/env.h>
#include <p101_error/error.h>
#include <stddef.h>
#include <stdlib.h>

static const char *const wrapper_functions[] = {"p101_getopt",
                                                "p101_getopt_long",
                                                "p101_getopt_long_only",
                                                "p101_parse_char",
                                                "p101_parse_int",
                                                "p101_parse_int16_t",
                                                "p101_parse_int32_t",
                                                "p101_parse_int64_t",
                                                "p101_parse_int8_t",
                                                "p101_parse_long",
                                                "p101_parse_long_long",
                                                "p101_parse_negative_char",
                                                "p101_parse_negative_int",
                                                "p101_parse_negative_int16_t",
                                                "p101_parse_negative_int32_t",
                                                "p101_parse_negative_int64_t",
                                                "p101_parse_negative_int8_t",
                                                "p101_parse_negative_long",
                                                "p101_parse_negative_long_long",
                                                "p101_parse_negative_short",
                                                "p101_parse_positive_char",
                                                "p101_parse_positive_int",
                                                "p101_parse_positive_int16_t",
                                                "p101_parse_positive_int32_t",
                                                "p101_parse_positive_int64_t",
                                                "p101_parse_positive_int8_t",
                                                "p101_parse_positive_long",
                                                "p101_parse_positive_long_long",
                                                "p101_parse_positive_short",
                                                "p101_parse_short",
                                                "p101_parse_uint16_t",
                                                "p101_parse_uint32_t",
                                                "p101_parse_uint64_t",
                                                "p101_parse_uint8_t",
                                                "p101_parse_unsigned_char",
                                                "p101_parse_unsigned_int",
                                                "p101_parse_unsigned_long",
                                                "p101_parse_unsigned_long_long",
                                                "p101_parse_unsigned_short",
                                                "p101_imaxabs",
                                                "p101_imaxdiv",
                                                "p101_strtoimax",
                                                "p101_strtoumax",
                                                "p101_wcstoimax",
                                                "p101_wcstoumax"};

static const size_t wrapper_function_count = sizeof(wrapper_functions) / sizeof(wrapper_functions[0]);

static int write_line(const struct p101_env *env, struct p101_error *err, const char *line)
{
    int ret_val;

    ret_val = p101_fprintf(env, err, stdout, "%s\n", line);
    if(ret_val < 0 || p101_error_has_error(err))
    {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

static int write_wrapper_inventory(const struct p101_env *env, struct p101_error *err)
{
    int ret_val;

    ret_val = EXIT_SUCCESS;
    for(size_t i = 0; i < wrapper_function_count; i++)
    {
        if(p101_fprintf(env, err, stdout, "- %s\n", wrapper_functions[i]) < 0 || p101_error_has_error(err))
        {
            ret_val = EXIT_FAILURE;
            break;
        }
    }

    return ret_val;
}

int main(void)
{
    struct p101_error *err;
    struct p101_env   *env;
    int                ret_val;

    ret_val = EXIT_FAILURE;
    err     = p101_error_create(false);
    if(err == NULL)
    {
        return EXIT_FAILURE;
    }

    env = p101_env_create(err, NULL);
    if(env == NULL || p101_error_has_error(err))
    {
        goto done;
    }

    if(p101_fprintf(env, err, stdout, "Track %d: %s\n", P101_TRACK_INDEX, P101_TRACK_TITLE) < 0 || p101_error_has_error(err))
    {
        goto done;
    }
    if(write_line(env, err, P101_TRACK_PURPOSE) != EXIT_SUCCESS)
    {
        goto done;
    }
    if(p101_fprintf(env, err, stdout, "Wrapper count: %d\n", P101_TRACK_WRAPPER_COUNT) < 0 || p101_error_has_error(err))
    {
        goto done;
    }
    if(write_line(env, err, "Wrappers in this track:") != EXIT_SUCCESS)
    {
        goto done;
    }
    if(write_wrapper_inventory(env, err) != EXIT_SUCCESS)
    {
        goto done;
    }

    ret_val = EXIT_SUCCESS;

done:
    p101_env_destroy(env);
    p101_error_destroy(err);
    return ret_val;
}

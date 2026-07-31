#include "track_info.h"
#include <p101_c/p101_stdio.h>
#include <p101_c/p101_stdlib.h>
#include <p101_env/env.h>
#include <p101_error/error.h>
#include <stddef.h>
#include <stdlib.h>

static const char *const wrapper_functions[] = {"p101_atomic_flag_clear",
                                                "p101_atomic_flag_clear_explicit",
                                                "p101_atomic_flag_test_and_set",
                                                "p101_atomic_flag_test_and_set_explicit",
                                                "p101_atomic_signal_fence",
                                                "p101_atomic_thread_fence",
                                                "p101_atomic_uint_compare_exchange_strong",
                                                "p101_atomic_uint_compare_exchange_strong_explicit",
                                                "p101_atomic_uint_compare_exchange_weak",
                                                "p101_atomic_uint_compare_exchange_weak_explicit",
                                                "p101_atomic_uint_exchange",
                                                "p101_atomic_uint_exchange_explicit",
                                                "p101_atomic_uint_fetch_add",
                                                "p101_atomic_uint_fetch_add_explicit",
                                                "p101_atomic_uint_fetch_and",
                                                "p101_atomic_uint_fetch_and_explicit",
                                                "p101_atomic_uint_fetch_or",
                                                "p101_atomic_uint_fetch_or_explicit",
                                                "p101_atomic_uint_fetch_sub",
                                                "p101_atomic_uint_fetch_sub_explicit",
                                                "p101_atomic_uint_fetch_xor",
                                                "p101_atomic_uint_fetch_xor_explicit",
                                                "p101_atomic_uint_load",
                                                "p101_atomic_uint_load_explicit",
                                                "p101_atomic_uint_store",
                                                "p101_atomic_uint_store_explicit",
                                                "p101_longjmp",
                                                "p101_raise",
                                                "p101_signal",
                                                "p101_localeconv",
                                                "p101_setlocale",
                                                "p101_clock",
                                                "p101_difftime",
                                                "p101_mktime",
                                                "p101_strftime",
                                                "p101_time",
                                                "p101_timespec_get"};

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

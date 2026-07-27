#include "track_info.h"
#include <p101_c/p101_stdio.h>
#include <p101_c/p101_stdlib.h>
#include <p101_env/env.h>
#include <p101_error/error.h>
#include <stddef.h>
#include <stdlib.h>

static const char *const wrapper_functions[] = {
    "p101_ceil",
    "p101_ceilf",
    "p101_ceill",
    "p101_floor",
    "p101_floorf",
    "p101_floorl",
    "p101_fmod",
    "p101_fmodf",
    "p101_fmodl",
    "p101_frexp",
    "p101_frexpf",
    "p101_frexpl",
    "p101_ldexp",
    "p101_ldexpf",
    "p101_ldexpl",
    "p101_llrint",
    "p101_llrintf",
    "p101_llrintl",
    "p101_llround",
    "p101_llroundf",
    "p101_llroundl",
    "p101_lrint",
    "p101_lrintf",
    "p101_lrintl",
    "p101_lround",
    "p101_lroundf",
    "p101_lroundl",
    "p101_modf",
    "p101_modff",
    "p101_modfl",
    "p101_nearbyint",
    "p101_nearbyintf",
    "p101_nearbyintl",
    "p101_remainder",
    "p101_remainderf",
    "p101_remainderl",
    "p101_remquo",
    "p101_remquof",
    "p101_remquol",
    "p101_rint",
    "p101_rintf",
    "p101_rintl",
    "p101_round",
    "p101_roundf",
    "p101_roundl",
    "p101_scalbln",
    "p101_scalblnf",
    "p101_scalblnl",
    "p101_scalbn",
    "p101_scalbnf",
    "p101_scalbnl",
    "p101_trunc",
    "p101_truncf",
    "p101_truncl"
};

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

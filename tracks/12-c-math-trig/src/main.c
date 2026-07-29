#include "track_info.h"
#include <p101_c/p101_stdio.h>
#include <p101_c/p101_stdlib.h>
#include <p101_env/env.h>
#include <p101_error/error.h>
#include <stddef.h>
#include <stdlib.h>

static const char *const wrapper_functions[] = {"p101_acos",  "p101_acosf",  "p101_acosh",  "p101_acoshf", "p101_acoshl", "p101_acosl",  "p101_asin",   "p101_asinf", "p101_asinh", "p101_asinhf", "p101_asinhl", "p101_asinl", "p101_atan",
                                                "p101_atan2", "p101_atan2f", "p101_atan2l", "p101_atanf",  "p101_atanh",  "p101_atanhf", "p101_atanhl", "p101_atanl", "p101_cos",   "p101_cosf",   "p101_cosh",   "p101_coshf", "p101_coshl",
                                                "p101_cosl",  "p101_sin",    "p101_sinf",   "p101_sinh",   "p101_sinhf",  "p101_sinhl",  "p101_sinl",   "p101_tan",   "p101_tanf",  "p101_tanh",   "p101_tanhf",  "p101_tanhl", "p101_tanl"};

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

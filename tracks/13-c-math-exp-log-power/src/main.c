#include "track_info.h"
#include <p101_c/p101_stdio.h>
#include <p101_c/p101_stdlib.h>
#include <p101_env/env.h>
#include <p101_error/error.h>
#include <stddef.h>
#include <stdlib.h>

static const char *const wrapper_functions[] = {"p101_cbrt",  "p101_cbrtf", "p101_cbrtl",  "p101_erf",    "p101_erfc",   "p101_erfcf",  "p101_erfcl",  "p101_erff",   "p101_erfl",   "p101_exp",    "p101_exp2",    "p101_exp2f",
                                                "p101_exp2l", "p101_expf",  "p101_expl",   "p101_expm1",  "p101_expm1f", "p101_expm1l", "p101_hypot",  "p101_hypotf", "p101_hypotl", "p101_lgamma", "p101_lgammaf", "p101_lgammal",
                                                "p101_log",   "p101_log10", "p101_log10f", "p101_log10l", "p101_log1p",  "p101_log1pf", "p101_log1pl", "p101_log2",   "p101_log2f",  "p101_log2l",  "p101_logb",    "p101_logbf",
                                                "p101_logbl", "p101_logf",  "p101_logl",   "p101_pow",    "p101_powf",   "p101_powl",   "p101_sqrt",   "p101_sqrtf",  "p101_sqrtl",  "p101_tgamma", "p101_tgammaf", "p101_tgammal"};

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

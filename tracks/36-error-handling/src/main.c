#include "track_info.h"
#include <p101_c/p101_stdio.h>
#include <p101_c/p101_stdlib.h>
#include <p101_env/env.h>
#include <p101_error/error.h>
#include <stddef.h>
#include <stdlib.h>

static const char *const wrapper_functions[] = {"p101_check_equals_int",
                                                "p101_check_equals_intmax",
                                                "p101_check_equals_string",
                                                "p101_check_equals_uintmax",
                                                "p101_check_greater_than_double",
                                                "p101_check_greater_than_int",
                                                "p101_check_greater_than_intmax",
                                                "p101_check_greater_than_long_double",
                                                "p101_check_greater_than_uintmax",
                                                "p101_check_in_range_double",
                                                "p101_check_in_range_int",
                                                "p101_check_in_range_intmax",
                                                "p101_check_in_range_long_double",
                                                "p101_check_in_range_uintmax",
                                                "p101_check_less_than_double",
                                                "p101_check_less_than_int",
                                                "p101_check_less_than_intmax",
                                                "p101_check_less_than_long_double",
                                                "p101_check_less_than_uintmax",
                                                "p101_check_not_equals_int",
                                                "p101_check_not_equals_intmax",
                                                "p101_check_not_equals_string",
                                                "p101_check_not_equals_uintmax",
                                                "p101_check_not_null",
                                                "p101_check_null",
                                                "p101_errno_get_errno",
                                                "p101_error_check",
                                                "p101_error_copy",
                                                "p101_error_create",
                                                "p101_error_default_error_reporter",
                                                "p101_error_destroy",
                                                "p101_error_errno",
                                                "p101_error_get_code",
                                                "p101_error_get_errno",
                                                "p101_error_get_file_name",
                                                "p101_error_get_function_name",
                                                "p101_error_get_line_number",
                                                "p101_error_get_message",
                                                "p101_error_get_type",
                                                "p101_error_has_error",
                                                "p101_error_has_no_error",
                                                "p101_error_is_errno",
                                                "p101_error_is_error",
                                                "p101_error_is_reporting",
                                                "p101_error_move",
                                                "p101_error_reset",
                                                "p101_error_set_reporting",
                                                "p101_error_system",
                                                "p101_error_user",
                                                "p101_error_user_printf"};

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

static int run_error_lifecycle_demo(const struct p101_env *env, struct p101_error *err)
{
    enum
    {
        TEACHING_ERROR_CODE = 17
    };
    struct p101_error *local_err;
    int                ret_val;

    ret_val   = EXIT_FAILURE;
    local_err = p101_error_create(false);
    if(local_err == NULL)
    {
        return EXIT_FAILURE;
    }
    P101_ERROR_RAISE_USER(local_err, "expected teaching error", TEACHING_ERROR_CODE);
    if(!p101_error_is_error(local_err, P101_ERROR_USER, TEACHING_ERROR_CODE))
    {
        goto done;
    }
    if(p101_fprintf(env, err, stdout, "Observed error: %s\n", p101_error_get_message(local_err)) < 0 || p101_error_has_error(err))
    {
        goto done;
    }
    p101_error_reset(local_err);
    if(p101_error_has_no_error(local_err))
    {
        ret_val = EXIT_SUCCESS;
    }

done:
    p101_error_destroy(local_err);
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

    if(run_error_lifecycle_demo(env, err) != EXIT_SUCCESS)
    {
        goto done;
    }

    ret_val = EXIT_SUCCESS;

done:
    p101_env_destroy(env);
    p101_error_destroy(err);
    return ret_val;
}

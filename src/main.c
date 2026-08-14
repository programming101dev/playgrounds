#include "cli.h"
#include "constants.h"
#include "errors.h"
#include "playground.h"
#include "scenario.h"
#include <p101_c/p101_stdio.h>
#include <p101_c/p101_stdlib.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    bool        p101_bool_result_1;
    const char *p101_const_char_pointer_result_1;

    struct p101_error *err;
    struct p101_env   *env;
    struct arguments   args;
    int                ret_val;

    ret_val = EXIT_FAILURE;
    err     = p101_error_create(false);
    env     = p101_env_create(err, NULL);

    p101_tool_playground_arguments_set_defaults(env, &args);
    p101_tool_playground_parse_arguments(env, err, argc, argv, &args);

    p101_bool_result_1 = p101_error_has_no_error(err);
    if(args.show_help && p101_bool_result_1)
    {
        p101_tool_playground_usage(env, err, argv[0], EXIT_SUCCESS, NULL);
        ret_val = EXIT_SUCCESS;
        goto done;
    }

    if(args.show_scenario_manifest && p101_bool_result_1)
    {
        p101_tool_playground_write_scenario_manifest(env, err, stdout);
        p101_bool_result_1 = p101_error_has_no_error(err);
        if(p101_bool_result_1)
        {
            ret_val = EXIT_SUCCESS;
        }
        goto done;
    }

    p101_bool_result_1 = p101_error_has_error(err);
    if(p101_bool_result_1)
    {
        goto done;
    }

    if(args.verbose)
    {
        p101_env_set_tracer(env, p101_env_default_tracer);
    }

    p101_tool_playground_check_arguments(env, err, &args);

    p101_bool_result_1 = p101_error_has_error(err);
    if(p101_bool_result_1)
    {
        goto done;
    }

    p101_tool_playground_convert_arguments(env, err, &args);

    p101_bool_result_1 = p101_error_has_error(err);
    if(p101_bool_result_1)
    {
        goto done;
    }

    ret_val = p101_tool_playground_run(env, err, &args);

done:
    p101_bool_result_1 = p101_error_has_error(err);
    if(p101_bool_result_1)
    {
        p101_bool_result_1 = p101_error_is_error(err, P101_ERROR_USER, ERR_USAGE);
        if(p101_bool_result_1)
        {
            const char *msg;

            msg = p101_error_get_message(err);
            p101_tool_playground_usage(env, err, argv[0], EXIT_FAILURE, msg);
        }
        else
        {
            p101_const_char_pointer_result_1 = p101_error_get_message(err);
            p101_fprintf(env, err, stderr, "%s\n", p101_const_char_pointer_result_1);
        }
        ret_val = EXIT_FAILURE;
    }

    p101_env_destroy(env);
    p101_error_destroy(err);

    return ret_val;
}

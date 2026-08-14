#include "cli.h"
#include "constants.h"
#include "errors.h"
#include "scenario.h"
#include <p101_c/p101_ctype.h>
#include <p101_c/p101_stdio.h>
#include <p101_c/p101_string.h>
#include <p101_cli/p101_getopt.h>
#include <p101_cli/p101_stdlib.h>
#include <p101_cli/p101_unistd.h>
#include <p101_convert/integer.h>
#include <stdlib.h>

void p101_tool_playground_arguments_set_defaults(const struct p101_env *env, struct arguments *args)
{
    P101_TRACE_SCOPE(env);
    p101_memset(env, args, 0, sizeof(*args));
    args->scenario_str = DEFAULT_SCENARIO;
    args->output_path  = DEFAULT_OUTPUT_PATH;
    args->bytes        = DEFAULT_BYTES;
    args->repeats      = DEFAULT_REPEATS;
}

void p101_tool_playground_parse_arguments(const struct p101_env *env, struct p101_error *err, int argc, char *argv[], struct arguments *args)
{
    bool p101_bool_result_1;
    int  p101_int_result_1;

    P101_TRACE_SCOPE(env);
    opterr = 0;

    while(true)
    {
        int opt;

        opt                = p101_getopt(env, argc, argv, ":hvSs:o:b:r:");
        p101_bool_result_1 = p101_error_has_no_error(err);
        if(opt == -1 || !p101_bool_result_1)
        {
            break;
        }

        switch(opt)
        {
            case 'h':
            {
                args->show_help = true;
                break;
            }
            case 'v':
            {
                args->verbose = true;
                break;
            }
            case 'S':
            {
                args->show_scenario_manifest = true;
                break;
            }
            case 's':
            {
                args->scenario_str = optarg;
                break;
            }
            case 'o':
            {
                args->output_path = optarg;
                break;
            }
            case 'b':
            {
                args->bytes_str = optarg;
                break;
            }
            case 'r':
            {
                args->repeats_str = optarg;
                break;
            }
            case ':':
            {
                char msg[MSG_LEN];

                p101_snprintf(env, err, msg, sizeof(msg), "Option '-%c' requires an argument.", optopt ? optopt : '?');
                P101_ERROR_RAISE_USER(err, msg, ERR_USAGE);
                break;
            }
            case '?':
            {
                char msg[MSG_LEN];

                p101_int_result_1 = p101_isprint(env, optopt);
                if(p101_int_result_1)
                {
                    p101_snprintf(env, err, msg, sizeof(msg), "Unknown option '-%c'.", optopt);
                }
                else
                {
                    p101_snprintf(env, err, msg, sizeof(msg), "Unknown option character 0x%02X.", (unsigned)(unsigned char)optopt);
                }

                P101_ERROR_RAISE_USER(err, msg, ERR_USAGE);
                break;
            }
            default:
            {
                char msg[MSG_LEN];

                p101_int_result_1 = p101_isprint(env, opt);
                p101_snprintf(env, err, msg, sizeof(msg), "Internal error: unhandled option '-%c' returned by getopt.", p101_int_result_1 ? opt : '?');
                P101_ERROR_RAISE_USER(err, msg, ERR_USAGE);
                break;
            }
        }
    }

    p101_bool_result_1 = p101_error_has_no_error(err);
    if(p101_bool_result_1 && optind < argc)
    {
        P101_ERROR_RAISE_USER(err, "Unexpected positional argument.", ERR_USAGE);
    }
}

void p101_tool_playground_check_arguments(const struct p101_env *env, struct p101_error *err, const struct arguments *args)
{
    enum playground_scenario p101_enum_playground_scenario_result_1;

    bool ok;

    P101_TRACE_SCOPE(env);

    if(args->scenario_str == NULL || args->scenario_str[0] == '\0')
    {
        P101_ERROR_RAISE_USER(err, "The scenario must not be empty.", ERR_USAGE);
        goto done;
    }

    p101_enum_playground_scenario_result_1 = p101_tool_playground_scenario_from_name(env, args->scenario_str, &ok);
    (void)p101_enum_playground_scenario_result_1;
    if(!ok)
    {
        P101_ERROR_RAISE_USER(err, "Unknown scenario.", ERR_USAGE);
        goto done;
    }

    if(args->output_path == NULL || args->output_path[0] == '\0')
    {
        P101_ERROR_RAISE_USER(err, "The output path must not be empty.", ERR_USAGE);
        goto done;
    }

    if(args->bytes_str != NULL && args->bytes_str[0] == '\0')
    {
        P101_ERROR_RAISE_USER(err, "The byte count must not be empty.", ERR_USAGE);
        goto done;
    }

    if(args->repeats_str != NULL && args->repeats_str[0] == '\0')
    {
        P101_ERROR_RAISE_USER(err, "The repeat count must not be empty.", ERR_USAGE);
        goto done;
    }

done:
    return;
}

void p101_tool_playground_convert_arguments(const struct p101_env *env, struct p101_error *err, struct arguments *args)
{
    bool p101_bool_result_1;

    bool ok;

    P101_TRACE_SCOPE(env);
    args->scenario = p101_tool_playground_scenario_from_name(env, args->scenario_str, &ok);

    if(!ok)
    {
        P101_ERROR_RAISE_USER(err, "Unknown scenario.", ERR_USAGE);
        goto done;
    }

    if(args->bytes_str != NULL)
    {
        args->bytes = p101_parse_unsigned_int(env, err, args->bytes_str, DEFAULT_BYTES);

        p101_bool_result_1 = p101_error_has_error(err);
        if(p101_bool_result_1 || args->bytes == 0U || args->bytes > MAX_BYTES)
        {
            P101_ERROR_RAISE_USER(err, "The byte count must be between 1 and 4096.", ERR_USAGE);
            goto done;
        }
    }

    if(args->repeats_str != NULL)
    {
        args->repeats = p101_parse_unsigned_int(env, err, args->repeats_str, DEFAULT_REPEATS);

        p101_bool_result_1 = p101_error_has_error(err);
        if(p101_bool_result_1 || args->repeats == 0U || args->repeats > MAX_REPEATS)
        {
            P101_ERROR_RAISE_USER(err, "The repeat count must be between 1 and 32.", ERR_USAGE);
            goto done;
        }
    }

done:
    return;
}

void p101_tool_playground_usage(const struct p101_env *env, struct p101_error *err, const char *program_name, int exit_code, const char *message)
{
#ifndef P101_SUPPRESS_USAGE_TEXT
    FILE *stream;

    stream = (exit_code == EXIT_SUCCESS) ? stdout : stderr;

    if(message != NULL)
    {
        p101_fprintf(env, err, stream, "%s\n\n", message);
    }

    p101_fprintf(env, err, stream, "Usage: %s [-h] [-v] [-s <scenario>] [-o <output-path>] [-b <bytes>] [-r <repeats>]\n", program_name);
    p101_fputs(env, err, "\n", stream);
    p101_fputs(env, err, "A p101 observability playground for resource, call, fault, test, fuzz, and coverage tools.\n\n", stream);
    p101_fputs(env, err, "Options:\n", stream);
    p101_fputs(env, err, "  -h              Display this help message and exit\n", stream);
    p101_fputs(env, err, "  -v              Enable p101 tracing inside the playground itself\n", stream);
    p101_fputs(env, err, "  -s <scenario>   Scenario to run (default: tour)\n", stream);
    p101_fputs(env, err, "  -o <path>       Output file path for file scenarios\n", stream);
    p101_fputs(env, err, "  -b <bytes>      Bytes per allocation/write, 1..4096 (default: 64)\n", stream);
    p101_fputs(env, err, "  -r <repeats>    Repeated write/read operations, 1..32 (default: 2)\n\n", stream);
    p101_tool_playground_print_scenarios(env, err, stream);
#else
    (void)err;
    (void)exit_code;
    (void)program_name;
    (void)message;
#endif
}

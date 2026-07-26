#include "scenario.h"
#include "constants.h"
#include <p101_c/p101_stdio.h>
#include <p101_c/p101_string.h>
#include <stdlib.h>
#include <sys/wait.h>

enum playground_scenario p101_tool_playground_scenario_from_name(const struct p101_env *env, const char *name, bool *ok)
{
    enum playground_scenario scenario;

    *ok      = true;
    scenario = SCENARIO_TOUR;

    if(p101_strcmp(env, name, "tour") == 0)
    {
        scenario = SCENARIO_TOUR;
    }
    else if(p101_strcmp(env, name, "clean-file") == 0)
    {
        scenario = SCENARIO_CLEAN_FILE;
    }
    else if(p101_strcmp(env, name, "realloc") == 0)
    {
        scenario = SCENARIO_REALLOC;
    }
    else if(p101_strcmp(env, name, "pipe") == 0)
    {
        scenario = SCENARIO_PIPE;
    }
    else if(p101_strcmp(env, name, "fork") == 0)
    {
        scenario = SCENARIO_FORK;
    }
    else if(p101_strcmp(env, name, "fd-leak") == 0)
    {
        scenario = SCENARIO_FD_LEAK;
    }
    else if(p101_strcmp(env, name, "alloc-leak") == 0)
    {
        scenario = SCENARIO_ALLOC_LEAK;
    }
    else if(p101_strcmp(env, name, "double-close") == 0)
    {
        scenario = SCENARIO_DOUBLE_CLOSE;
    }
    else if(p101_strcmp(env, name, "stray-close") == 0)
    {
        scenario = SCENARIO_STRAY_CLOSE;
    }
    else if(p101_strcmp(env, name, "fault-lab") == 0)
    {
        scenario = SCENARIO_FAULT_LAB;
    }
    else if(p101_strcmp(env, name, "early-return-fd-leak") == 0)
    {
        scenario = SCENARIO_EARLY_RETURN_FD_LEAK;
    }
    else if(p101_strcmp(env, name, "early-return-alloc-leak") == 0)
    {
        scenario = SCENARIO_EARLY_RETURN_ALLOC_LEAK;
    }
    else if(p101_strcmp(env, name, "partial-cleanup") == 0)
    {
        scenario = SCENARIO_PARTIAL_CLEANUP;
    }
    else if(p101_strcmp(env, name, "realloc-leak") == 0)
    {
        scenario = SCENARIO_REALLOC_LEAK;
    }
    else if(p101_strcmp(env, name, "exec-inherit") == 0)
    {
        scenario = SCENARIO_EXEC_INHERIT;
    }
    else if(p101_strcmp(env, name, "double-free") == 0)
    {
        scenario = SCENARIO_DOUBLE_FREE;
    }
    else if(p101_strcmp(env, name, "stray-free") == 0)
    {
        scenario = SCENARIO_STRAY_FREE;
    }
    else if(p101_strcmp(env, name, "sizeof-pointer") == 0)
    {
        scenario = SCENARIO_SIZEOF_POINTER;
    }
    else if(p101_strcmp(env, name, "ignore-read-count") == 0)
    {
        scenario = SCENARIO_IGNORE_READ_COUNT;
    }
    else if(p101_strcmp(env, name, "unsafe-log-secret") == 0)
    {
        scenario = SCENARIO_UNSAFE_LOG_SECRET;
    }
    else if(p101_strcmp(env, name, "log-injection") == 0)
    {
        scenario = SCENARIO_LOG_INJECTION;
    }
    else if(p101_strcmp(env, name, "missing-structured-log") == 0)
    {
        scenario = SCENARIO_MISSING_STRUCTURED_LOG;
    }
    else if(p101_strcmp(env, name, "input-validation") == 0)
    {
        scenario = SCENARIO_INPUT_VALIDATION;
    }
    else if(p101_strcmp(env, name, "command-injection") == 0)
    {
        scenario = SCENARIO_COMMAND_INJECTION;
    }
    else if(p101_strcmp(env, name, "predictable-temp-file") == 0)
    {
        scenario = SCENARIO_PREDICTABLE_TEMP_FILE;
    }
    else if(p101_strcmp(env, name, "signed-conversion") == 0)
    {
        scenario = SCENARIO_SIGNED_CONVERSION;
    }
    else if(p101_strcmp(env, name, "truncation") == 0)
    {
        scenario = SCENARIO_TRUNCATION;
    }
    else
    {
        *ok = false;
    }

    return scenario;
}

const char *p101_tool_playground_scenario_name(enum playground_scenario scenario)
{
    const char *name;

#ifdef __clang__
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wcovered-switch-default"
#endif
    switch(scenario)
    {
        case SCENARIO_TOUR:
        {
            name = "tour";
            break;
        }
        case SCENARIO_CLEAN_FILE:
        {
            name = "clean-file";
            break;
        }
        case SCENARIO_REALLOC:
        {
            name = "realloc";
            break;
        }
        case SCENARIO_PIPE:
        {
            name = "pipe";
            break;
        }
        case SCENARIO_FORK:
        {
            name = "fork";
            break;
        }
        case SCENARIO_FD_LEAK:
        {
            name = "fd-leak";
            break;
        }
        case SCENARIO_ALLOC_LEAK:
        {
            name = "alloc-leak";
            break;
        }
        case SCENARIO_DOUBLE_CLOSE:
        {
            name = "double-close";
            break;
        }
        case SCENARIO_STRAY_CLOSE:
        {
            name = "stray-close";
            break;
        }
        case SCENARIO_FAULT_LAB:
        {
            name = "fault-lab";
            break;
        }
        case SCENARIO_EARLY_RETURN_FD_LEAK:
        {
            name = "early-return-fd-leak";
            break;
        }
        case SCENARIO_EARLY_RETURN_ALLOC_LEAK:
        {
            name = "early-return-alloc-leak";
            break;
        }
        case SCENARIO_PARTIAL_CLEANUP:
        {
            name = "partial-cleanup";
            break;
        }
        case SCENARIO_REALLOC_LEAK:
        {
            name = "realloc-leak";
            break;
        }
        case SCENARIO_EXEC_INHERIT:
        {
            name = "exec-inherit";
            break;
        }
        case SCENARIO_DOUBLE_FREE:
        {
            name = "double-free";
            break;
        }
        case SCENARIO_STRAY_FREE:
        {
            name = "stray-free";
            break;
        }
        case SCENARIO_SIZEOF_POINTER:
        {
            name = "sizeof-pointer";
            break;
        }
        case SCENARIO_IGNORE_READ_COUNT:
        {
            name = "ignore-read-count";
            break;
        }
        case SCENARIO_UNSAFE_LOG_SECRET:
        {
            name = "unsafe-log-secret";
            break;
        }
        case SCENARIO_LOG_INJECTION:
        {
            name = "log-injection";
            break;
        }
        case SCENARIO_MISSING_STRUCTURED_LOG:
        {
            name = "missing-structured-log";
            break;
        }
        case SCENARIO_INPUT_VALIDATION:
        {
            name = "input-validation";
            break;
        }
        case SCENARIO_COMMAND_INJECTION:
        {
            name = "command-injection";
            break;
        }
        case SCENARIO_PREDICTABLE_TEMP_FILE:
        {
            name = "predictable-temp-file";
            break;
        }
        case SCENARIO_SIGNED_CONVERSION:
        {
            name = "signed-conversion";
            break;
        }
        case SCENARIO_TRUNCATION:
        {
            name = "truncation";
            break;
        }
        default:
        {
            name = "unknown";
            break;
        }
    }
#ifdef __clang__
    #pragma clang diagnostic pop
#endif

    return name;
}

void p101_tool_playground_print_scenarios(const struct p101_env *env, struct p101_error *err, FILE *stream)
{
    p101_fputs(env, err, "Scenarios:\n", stream);
    p101_fputs(env, err, "  tour          Clean default: file + realloc + pipe + fork\n", stream);
    p101_fputs(env, err, "  clean-file    Open/write/close/free cleanly\n", stream);
    p101_fputs(env, err, "  realloc       Allocate, grow, and free a block\n", stream);
    p101_fputs(env, err, "  pipe          Create a pipe and close both descriptors\n", stream);
    p101_fputs(env, err, "  fork          Fork a child that uses heap + pipe resources\n", stream);
    p101_fputs(env, err, "  fd-leak       Intentionally leak one descriptor\n", stream);
    p101_fputs(env, err, "  alloc-leak    Intentionally leak one allocation\n", stream);
    p101_fputs(env, err, "  double-close  Intentionally close one descriptor twice\n", stream);
    p101_fputs(env, err, "  stray-close              Intentionally close an unopened positive descriptor\n", stream);
    p101_fputs(env, err, "  fault-lab                Clean normally, leaky under injected p101 failures\n", stream);
    p101_fputs(env, err, "  early-return-fd-leak     Return before descriptor cleanup\n", stream);
    p101_fputs(env, err, "  early-return-alloc-leak  Return before allocation cleanup\n", stream);
    p101_fputs(env, err, "  partial-cleanup          Acquire several resources and clean up only some\n", stream);
    p101_fputs(env, err, "  realloc-leak             Grow an allocation and forget to free the result\n", stream);
    p101_fputs(env, err, "  exec-inherit             Leave a descriptor open across an exec boundary\n", stream);
    p101_fputs(env, err, "  double-free              Free the same allocation twice\n", stream);
    p101_fputs(env, err, "  stray-free               Free a pointer this function does not own\n", stream);
    p101_fputs(env, err, "  sizeof-pointer           Use sizeof(pointer) where the data size was needed\n", stream);
    p101_fputs(env, err, "  ignore-read-count        Write a whole buffer instead of the bytes read\n", stream);
    p101_fputs(env, err, "  unsafe-log-secret        Write a secret directly into an application log\n", stream);
    p101_fputs(env, err, "  log-injection            Allow untrusted text to forge a second log record\n", stream);
    p101_fputs(env, err, "  missing-structured-log   Emit vague text instead of structured event fields\n", stream);
    p101_fputs(env, err, "  input-validation         Accept an unsafe path-shaped input value\n", stream);
    p101_fputs(env, err, "  command-injection        Build a shell command from untrusted text\n", stream);
    p101_fputs(env, err, "  predictable-temp-file    Use a predictable filename in /tmp\n", stream);
    p101_fputs(env, err, "  signed-conversion        Convert a negative count to an unsigned size\n", stream);
    p101_fputs(env, err, "  truncation               Store a large value in a too-small integer type\n", stream);
}

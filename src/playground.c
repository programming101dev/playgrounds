#include "playground.h"
#include "constants.h"
#include "defect_demo.h"
#include "errors.h"
#include "lesson_demo.h"
#include "playground_support.h"
#include "scenario.h"
#include <fcntl.h>
#include <limits.h>
#include <p101_c/p101_stdio.h>
#include <p101_c/p101_stdlib.h>
#include <p101_c/p101_string.h>
#include <p101_cli/p101_getopt.h>
#include <p101_cli/p101_stdlib.h>
#include <p101_cli/p101_unistd.h>
#include <p101_filesystem/p101_dirent.h>
#include <p101_filesystem/p101_fnmatch.h>
#include <p101_filesystem/p101_ftw.h>
#include <p101_filesystem/p101_glob.h>
#include <p101_filesystem/p101_libgen.h>
#include <p101_filesystem/p101_stdio.h>
#include <p101_filesystem/p101_stdlib.h>
#include <p101_filesystem/p101_unistd.h>
#include <p101_filesystem/sys/p101_stat.h>
#include <p101_filesystem/sys/p101_statvfs.h>
#include <p101_host/p101_stdlib.h>
#include <p101_host/p101_unistd.h>
#include <p101_host/sys/p101_utsname.h>
#include <p101_io/p101_aio.h>
#include <p101_io/p101_fcntl.h>
#include <p101_io/p101_poll.h>
#include <p101_io/p101_stdio.h>
#include <p101_io/p101_unistd.h>
#include <p101_io/sys/p101_select.h>
#include <p101_io/sys/p101_uio.h>
#include <p101_ipc/p101_unistd.h>
#include <p101_ipc/sys/p101_ipc.h>
#include <p101_ipc/sys/p101_mman.h>
#include <p101_ipc/sys/p101_msg.h>
#include <p101_ipc/sys/p101_sem.h>
#include <p101_ipc/sys/p101_shm.h>
#include <p101_ipc/sys/p101_stat.h>
#include <p101_memory/p101_stdlib.h>
#include <p101_memory/sys/p101_mman.h>
#include <p101_process/p101_sched.h>
#include <p101_process/p101_setjmp.h>
#include <p101_process/p101_signal.h>
#include <p101_process/p101_spawn.h>
#include <p101_process/p101_stdio.h>
#include <p101_process/p101_stdlib.h>
#include <p101_process/p101_unistd.h>
#include <p101_process/sys/p101_resource.h>
#include <p101_process/sys/p101_times.h>
#include <p101_process/sys/p101_wait.h>
#include <p101_random/p101_stdlib.h>
#include <p101_terminal/p101_stdlib.h>
#include <p101_terminal/p101_termios.h>
#include <p101_terminal/p101_unistd.h>
#include <p101_text/p101_ctype.h>
#include <p101_text/p101_regex.h>
#include <p101_text/p101_stdlib.h>
#include <p101_text/p101_string.h>
#include <p101_text/p101_strings.h>
#include <p101_text/p101_unistd.h>
#include <p101_text/p101_wchar.h>
#include <p101_text/p101_wctype.h>
#include <p101_text/p101_wordexp.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <wchar.h>

#ifndef PATH_MAX
    #define PATH_MAX 4096
#endif

static int run_realloc_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args);
static int run_pipe_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args);
static int run_fork_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args);

int p101_tool_playground_run(const struct p101_env *env, struct p101_error *err, const struct arguments *args)
{
    const char *p101_const_char_pointer_result_1;

    int ret_val;

    P101_TRACE_SCOPE(env);
    p101_const_char_pointer_result_1 = p101_tool_playground_scenario_name(args->scenario);
    p101_printf(env, err, "p101-tool-playground: scenario=%s output=%s bytes=%u repeats=%u\n", p101_const_char_pointer_result_1, args->output_path, args->bytes, args->repeats);

#ifdef __clang__
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wcovered-switch-default"
#endif
    switch(args->scenario)
    {
        case SCENARIO_TOUR:
        {
            bool p101_bool_result_1;

            ret_val            = run_clean_file(env, err, args);
            p101_bool_result_1 = p101_error_has_no_error(err);
            if(p101_bool_result_1)
            {
                ret_val = run_realloc_demo(env, err, args);
            }
            p101_bool_result_1 = p101_error_has_no_error(err);
            if(p101_bool_result_1)
            {
                ret_val = run_pipe_demo(env, err, args);
            }
            p101_bool_result_1 = p101_error_has_no_error(err);
            if(p101_bool_result_1)
            {
                ret_val = run_fork_demo(env, err, args);
            }
            break;
        }
        case SCENARIO_ORIENTATION:
        {
            ret_val = run_orientation_demo(env, err, args);
            break;
        }
        case SCENARIO_CLEAN_FILE:
        {
            ret_val = run_clean_file(env, err, args);
            break;
        }
        case SCENARIO_C_MEMORY_RUNTIME:
        {
            ret_val = run_c_memory_runtime_demo(env, err, args);
            break;
        }
        case SCENARIO_REALLOC:
        {
            ret_val = run_realloc_demo(env, err, args);
            break;
        }
        case SCENARIO_PIPE:
        {
            ret_val = run_pipe_demo(env, err, args);
            break;
        }
        case SCENARIO_FORK:
        {
            ret_val = run_fork_demo(env, err, args);
            break;
        }
        case SCENARIO_FD_LEAK:
        case SCENARIO_ALLOC_LEAK:
        case SCENARIO_DOUBLE_CLOSE:
        case SCENARIO_STRAY_CLOSE:
        case SCENARIO_FAULT_LAB:
        case SCENARIO_EARLY_RETURN_FD_LEAK:
        case SCENARIO_EARLY_RETURN_ALLOC_LEAK:
        {
            ret_val = p101_tool_playground_run_resource_defect_demo(env, err, args);
            break;
        }
        case SCENARIO_PARTIAL_CLEANUP:
        case SCENARIO_REALLOC_LEAK:
        case SCENARIO_EXEC_INHERIT:
        case SCENARIO_DOUBLE_FREE:
        case SCENARIO_STRAY_FREE:
        case SCENARIO_SIZEOF_POINTER:
        case SCENARIO_IGNORE_READ_COUNT:
        {
            ret_val = p101_tool_playground_run_ownership_defect_demo(env, err, args);
            break;
        }
        case SCENARIO_UNSAFE_LOG_SECRET:
        case SCENARIO_LOG_INJECTION:
        case SCENARIO_MISSING_STRUCTURED_LOG:
        case SCENARIO_INPUT_VALIDATION:
        case SCENARIO_COMMAND_INJECTION:
        case SCENARIO_PREDICTABLE_TEMP_FILE:
        case SCENARIO_SIGNED_CONVERSION:
        case SCENARIO_TRUNCATION:
        case SCENARIO_USE_AFTER_FREE:
        case SCENARIO_REALLOC_FAILURE:
        case SCENARIO_OUT_OF_BOUNDS_WRITE:
        case SCENARIO_OUT_OF_BOUNDS_READ:
        case SCENARIO_BUFFER_OVERFLOW:
        case SCENARIO_UNINITIALIZED_READ:
        case SCENARIO_NULL_DEREFERENCE:
        case SCENARIO_INTEGER_OVERFLOW:
        case SCENARIO_PATH_TRAVERSAL:
        case SCENARIO_FORMAT_STRING:
        case SCENARIO_STALE_SECRET:
        case SCENARIO_RESOURCE_EXHAUSTION:
        case SCENARIO_TOCTOU:
        case SCENARIO_DATA_RACE:
        case SCENARIO_STRING_NOT_TERMINATED:
        case SCENARIO_PARTIAL_WRITE:
        case SCENARIO_INTERRUPTED_SYSCALL:
        case SCENARIO_UNSAFE_FILE_MODE:
        case SCENARIO_SYMLINK_FOLLOW:
        case SCENARIO_TRUSTED_ENVIRONMENT:
        case SCENARIO_UNCHECKED_PARSE:
        case SCENARIO_MISSING_AUTHORIZATION:
        case SCENARIO_CLEANUP_ORDER:
        case SCENARIO_THREAD_ARGUMENT_LIFETIME:
        case SCENARIO_SHORT_READ:
        case SCENARIO_READ_EOF_HANDLING:
        case SCENARIO_PARSER_FUZZ:
        {
            ret_val = p101_tool_playground_run_lesson_demo(env, err, args);
            break;
        }
        case SCENARIO_COUNT:
        {
            P101_ERROR_RAISE_USER(err, "The scenario-count sentinel is not executable.", ERR_USAGE);
            ret_val = EXIT_FAILURE;
            break;
        }
        default:
        {
            P101_ERROR_RAISE_USER(err, "The scenario value is invalid.", ERR_USAGE);
            ret_val = EXIT_FAILURE;
            break;
        }
    }
#ifdef __clang__
    #pragma clang diagnostic pop
#endif

    return ret_val;
}

static int run_realloc_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args)
{
    bool  p101_bool_result_1;
    void *p101_void_pointer_result_1;

    char *buffer;
    char *grown;
    int   ret_val;

    P101_TRACE_SCOPE(env);
    ret_val = EXIT_FAILURE;
    buffer  = p101_tool_playground_support_make_buffer(env, err, args->bytes, 'r');

    p101_bool_result_1 = p101_error_has_error(err);
    if(buffer == NULL || p101_bool_result_1)
    {
        goto done;
    }

    p101_void_pointer_result_1 = p101_realloc(env, err, buffer, (size_t)args->bytes + (size_t)args->bytes);
    grown                      = (char *)p101_void_pointer_result_1;
    p101_bool_result_1         = p101_error_has_error(err);
    if(grown == NULL || p101_bool_result_1)
    {
        p101_free(env, buffer);
        buffer = NULL;
        goto done;
    }

    buffer = grown;
    p101_memset(env, buffer + args->bytes, 'R', args->bytes);
    p101_printf(env, err, "realloc: grew one block from %u to %u bytes\n", args->bytes, args->bytes + args->bytes);
    ret_val = EXIT_SUCCESS;

done:
    p101_free(env, buffer);
    return ret_val;
}

static int run_pipe_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args)
{
    bool p101_bool_result_1;

    int        fds[2];
    const char message[] = "pipe-demo";
    char       buffer[READ_BUF_LEN];
    int        ret_val;

    P101_TRACE_SCOPE(env);
    fds[0]  = -1;
    fds[1]  = -1;
    ret_val = EXIT_FAILURE;
    p101_pipe(env, err, fds);

    p101_bool_result_1 = p101_error_has_error(err);
    if(p101_bool_result_1)
    {
        goto done;
    }

    for(unsigned int i = 0; i < args->repeats; i++)
    {
        p101_bool_result_1 = p101_error_has_no_error(err);
        if(!p101_bool_result_1)
        {
            break;
        }

        p101_write(env, err, fds[1], message, sizeof(message));
        p101_read(env, err, fds[0], buffer, sizeof(message));
    }

    p101_bool_result_1 = p101_error_has_no_error(err);
    if(p101_bool_result_1)
    {
        p101_printf(env, err, "pipe: wrote/read %u message%s\n", args->repeats, args->repeats == 1U ? "" : "s");
        ret_val = EXIT_SUCCESS;
    }

done:
    if(fds[0] != -1)
    {
        p101_close(env, err, fds[0]);
    }

    if(fds[1] != -1)
    {
        p101_close(env, err, fds[1]);
    }

    return ret_val;
}

static int run_fork_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args)
{
    bool p101_bool_result_1;

    int        fds[2];
    const char message[] = "child-resource-demo";
    char       buffer[READ_BUF_LEN];
    pid_t      pid;
    int        status;
    int        ret_val;

    P101_TRACE_SCOPE(env);
    (void)args;
    fds[0]  = -1;
    fds[1]  = -1;
    status  = 0;
    ret_val = EXIT_FAILURE;
    p101_pipe(env, err, fds);

    p101_bool_result_1 = p101_error_has_error(err);
    if(p101_bool_result_1)
    {
        goto done;
    }
    /*
     * A child that returns through main inherits the parent's stdio buffers.
     * Flush the teaching output before fork so both processes cannot flush the
     * same pre-fork bytes.
     */
    p101_fflush(env, err, stdout);
    p101_bool_result_1 = p101_error_has_error(err);
    if(p101_bool_result_1)
    {
        goto done;
    }

    pid = p101_fork(env, err);

    p101_bool_result_1 = p101_error_has_error(err);
    if(p101_bool_result_1)
    {
        goto done;
    }

    if(pid == 0)
    {
        char *child_buffer;

        child_buffer = p101_tool_playground_support_make_buffer(env, err, DEFAULT_BYTES, 'c');
        p101_close(env, err, fds[0]);
        fds[0] = -1;
        p101_write(env, err, fds[1], message, sizeof(message));
        p101_close(env, err, fds[1]);
        fds[1] = -1;
        p101_free(env, child_buffer);
        ret_val = EXIT_SUCCESS;

        p101_bool_result_1 = p101_error_has_error(err);
        if(p101_bool_result_1)
        {
            ret_val = EXEC_FAILURE;
        }
    }
    else
    {
        p101_close(env, err, fds[1]);
        fds[1] = -1;
        p101_read(env, err, fds[0], buffer, sizeof(message));
        p101_close(env, err, fds[0]);
        fds[0] = -1;
        p101_waitpid(env, err, pid, &status, 0);

        p101_bool_result_1 = p101_error_has_no_error(err);
        if(p101_bool_result_1 && WIFEXITED(status) && WEXITSTATUS(status) == EXIT_SUCCESS)
        {
            p101_printf(env, err, "fork: child allocated, wrote through a pipe, and cleaned up\n");
            ret_val = EXIT_SUCCESS;
        }
    }

done:
    if(fds[0] != -1)
    {
        p101_close(env, err, fds[0]);
    }

    if(fds[1] != -1)
    {
        p101_close(env, err, fds[1]);
    }

    return ret_val;
}

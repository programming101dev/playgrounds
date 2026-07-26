#include "playground.h"
#include "constants.h"
#include "scenario.h"
#include <fcntl.h>
#include <p101_c/p101_stdio.h>
#include <p101_c/p101_stdlib.h>
#include <p101_c/p101_string.h>
#include <p101_posix/p101_fcntl.h>
#include <p101_posix/p101_unistd.h>
#include <p101_posix/sys/p101_wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>

static int   run_clean_file(const struct p101_env *env, struct p101_error *err, const struct arguments *args);
static int   run_realloc_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args);
static int   run_pipe_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args);
static int   run_fork_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args);
static int   run_fd_leak_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args);
static int   run_alloc_leak_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args);
static int   run_double_close_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args);
static int   run_stray_close_demo(const struct p101_env *env, struct p101_error *err);
static int   run_fault_lab(const struct p101_env *env, struct p101_error *err, const struct arguments *args);
static int   run_early_return_fd_leak_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args);
static int   run_early_return_alloc_leak_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args);
static int   run_partial_cleanup_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args);
static int   run_realloc_leak_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args);
static int   run_exec_inherit_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args);
static int   run_double_free_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args);
static int   run_stray_free_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args);
static int   run_sizeof_pointer_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args);
static int   run_ignore_read_count_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args);
static int   run_unsafe_log_secret_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args);
static int   run_log_injection_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args);
static int   run_missing_structured_log_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args);
static int   run_input_validation_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args);
static int   run_command_injection_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args);
static int   run_predictable_temp_file_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args);
static int   run_signed_conversion_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args);
static int   run_truncation_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args);
static int   run_use_after_free_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args);
static int   run_realloc_failure_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args);
static int   run_out_of_bounds_write_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args);
static int   run_out_of_bounds_read_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args);
static int   run_buffer_overflow_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args);
static int   run_uninitialized_read_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args);
static int   run_null_dereference_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args);
static int   run_integer_overflow_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args);
static int   run_path_traversal_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args);
static int   run_format_string_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args);
static int   run_stale_secret_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args);
static int   run_resource_exhaustion_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args);
static int   run_toctou_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args);
static int   run_data_race_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args);
static int   run_string_not_terminated_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args);
static int   run_partial_write_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args);
static int   run_interrupted_syscall_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args);
static int   run_unsafe_file_mode_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args);
static int   run_symlink_follow_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args);
static int   run_trusted_environment_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args);
static int   run_unchecked_parse_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args);
static int   run_missing_authorization_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args);
static int   run_cleanup_order_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args);
static int   run_thread_argument_lifetime_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args);
static int   run_parser_fuzz_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args);
static int   write_demo_file(const struct p101_env *env, struct p101_error *err, const struct arguments *args, const char *label, bool leak_fd, bool leak_alloc);
static int   write_text_output(const struct p101_env *env, struct p101_error *err, const struct arguments *args, const char *text);
static char *make_buffer(const struct p101_env *env, struct p101_error *err, size_t bytes, char fill);

int p101_tool_playground_run(const struct p101_env *env, struct p101_error *err, const struct arguments *args)
{
    int ret_val;

    P101_TRACE(env);
    p101_printf(env, err, "p101-tool-playground: scenario=%s output=%s bytes=%u repeats=%u\n", p101_tool_playground_scenario_name(args->scenario), args->output_path, args->bytes, args->repeats);

#ifdef __clang__
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wcovered-switch-default"
#endif
    switch(args->scenario)
    {
        case SCENARIO_TOUR:
        {
            ret_val = run_clean_file(env, err, args);
            if(p101_error_has_no_error(err))
            {
                ret_val = run_realloc_demo(env, err, args);
            }
            if(p101_error_has_no_error(err))
            {
                ret_val = run_pipe_demo(env, err, args);
            }
            if(p101_error_has_no_error(err))
            {
                ret_val = run_fork_demo(env, err, args);
            }
            break;
        }
        case SCENARIO_CLEAN_FILE:
        {
            ret_val = run_clean_file(env, err, args);
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
        {
            ret_val = run_fd_leak_demo(env, err, args);
            break;
        }
        case SCENARIO_ALLOC_LEAK:
        {
            ret_val = run_alloc_leak_demo(env, err, args);
            break;
        }
        case SCENARIO_DOUBLE_CLOSE:
        {
            ret_val = run_double_close_demo(env, err, args);
            break;
        }
        case SCENARIO_STRAY_CLOSE:
        {
            ret_val = run_stray_close_demo(env, err);
            break;
        }
        case SCENARIO_FAULT_LAB:
        {
            ret_val = run_fault_lab(env, err, args);
            break;
        }
        case SCENARIO_EARLY_RETURN_FD_LEAK:
        {
            ret_val = run_early_return_fd_leak_demo(env, err, args);
            break;
        }
        case SCENARIO_EARLY_RETURN_ALLOC_LEAK:
        {
            ret_val = run_early_return_alloc_leak_demo(env, err, args);
            break;
        }
        case SCENARIO_PARTIAL_CLEANUP:
        {
            ret_val = run_partial_cleanup_demo(env, err, args);
            break;
        }
        case SCENARIO_REALLOC_LEAK:
        {
            ret_val = run_realloc_leak_demo(env, err, args);
            break;
        }
        case SCENARIO_EXEC_INHERIT:
        {
            ret_val = run_exec_inherit_demo(env, err, args);
            break;
        }
        case SCENARIO_DOUBLE_FREE:
        {
            ret_val = run_double_free_demo(env, err, args);
            break;
        }
        case SCENARIO_STRAY_FREE:
        {
            ret_val = run_stray_free_demo(env, err, args);
            break;
        }
        case SCENARIO_SIZEOF_POINTER:
        {
            ret_val = run_sizeof_pointer_demo(env, err, args);
            break;
        }
        case SCENARIO_IGNORE_READ_COUNT:
        {
            ret_val = run_ignore_read_count_demo(env, err, args);
            break;
        }
        case SCENARIO_UNSAFE_LOG_SECRET:
        {
            ret_val = run_unsafe_log_secret_demo(env, err, args);
            break;
        }
        case SCENARIO_LOG_INJECTION:
        {
            ret_val = run_log_injection_demo(env, err, args);
            break;
        }
        case SCENARIO_MISSING_STRUCTURED_LOG:
        {
            ret_val = run_missing_structured_log_demo(env, err, args);
            break;
        }
        case SCENARIO_INPUT_VALIDATION:
        {
            ret_val = run_input_validation_demo(env, err, args);
            break;
        }
        case SCENARIO_COMMAND_INJECTION:
        {
            ret_val = run_command_injection_demo(env, err, args);
            break;
        }
        case SCENARIO_PREDICTABLE_TEMP_FILE:
        {
            ret_val = run_predictable_temp_file_demo(env, err, args);
            break;
        }
        case SCENARIO_SIGNED_CONVERSION:
        {
            ret_val = run_signed_conversion_demo(env, err, args);
            break;
        }
        case SCENARIO_TRUNCATION:
        {
            ret_val = run_truncation_demo(env, err, args);
            break;
        }
        case SCENARIO_USE_AFTER_FREE:
        {
            ret_val = run_use_after_free_demo(env, err, args);
            break;
        }
        case SCENARIO_REALLOC_FAILURE:
        {
            ret_val = run_realloc_failure_demo(env, err, args);
            break;
        }
        case SCENARIO_OUT_OF_BOUNDS_WRITE:
        {
            ret_val = run_out_of_bounds_write_demo(env, err, args);
            break;
        }
        case SCENARIO_OUT_OF_BOUNDS_READ:
        {
            ret_val = run_out_of_bounds_read_demo(env, err, args);
            break;
        }
        case SCENARIO_BUFFER_OVERFLOW:
        {
            ret_val = run_buffer_overflow_demo(env, err, args);
            break;
        }
        case SCENARIO_UNINITIALIZED_READ:
        {
            ret_val = run_uninitialized_read_demo(env, err, args);
            break;
        }
        case SCENARIO_NULL_DEREFERENCE:
        {
            ret_val = run_null_dereference_demo(env, err, args);
            break;
        }
        case SCENARIO_INTEGER_OVERFLOW:
        {
            ret_val = run_integer_overflow_demo(env, err, args);
            break;
        }
        case SCENARIO_PATH_TRAVERSAL:
        {
            ret_val = run_path_traversal_demo(env, err, args);
            break;
        }
        case SCENARIO_FORMAT_STRING:
        {
            ret_val = run_format_string_demo(env, err, args);
            break;
        }
        case SCENARIO_STALE_SECRET:
        {
            ret_val = run_stale_secret_demo(env, err, args);
            break;
        }
        case SCENARIO_RESOURCE_EXHAUSTION:
        {
            ret_val = run_resource_exhaustion_demo(env, err, args);
            break;
        }
        case SCENARIO_TOCTOU:
        {
            ret_val = run_toctou_demo(env, err, args);
            break;
        }
        case SCENARIO_DATA_RACE:
        {
            ret_val = run_data_race_demo(env, err, args);
            break;
        }
        case SCENARIO_STRING_NOT_TERMINATED:
        {
            ret_val = run_string_not_terminated_demo(env, err, args);
            break;
        }
        case SCENARIO_PARTIAL_WRITE:
        {
            ret_val = run_partial_write_demo(env, err, args);
            break;
        }
        case SCENARIO_INTERRUPTED_SYSCALL:
        {
            ret_val = run_interrupted_syscall_demo(env, err, args);
            break;
        }
        case SCENARIO_UNSAFE_FILE_MODE:
        {
            ret_val = run_unsafe_file_mode_demo(env, err, args);
            break;
        }
        case SCENARIO_SYMLINK_FOLLOW:
        {
            ret_val = run_symlink_follow_demo(env, err, args);
            break;
        }
        case SCENARIO_TRUSTED_ENVIRONMENT:
        {
            ret_val = run_trusted_environment_demo(env, err, args);
            break;
        }
        case SCENARIO_UNCHECKED_PARSE:
        {
            ret_val = run_unchecked_parse_demo(env, err, args);
            break;
        }
        case SCENARIO_MISSING_AUTHORIZATION:
        {
            ret_val = run_missing_authorization_demo(env, err, args);
            break;
        }
        case SCENARIO_CLEANUP_ORDER:
        {
            ret_val = run_cleanup_order_demo(env, err, args);
            break;
        }
        case SCENARIO_THREAD_ARGUMENT_LIFETIME:
        {
            ret_val = run_thread_argument_lifetime_demo(env, err, args);
            break;
        }
        case SCENARIO_PARSER_FUZZ:
        {
            ret_val = run_parser_fuzz_demo(env, err, args);
            break;
        }
        default:
        {
            ret_val = EXIT_FAILURE;
            break;
        }
    }
#ifdef __clang__
    #pragma clang diagnostic pop
#endif

    return ret_val;
}

static int run_clean_file(const struct p101_env *env, struct p101_error *err, const struct arguments *args)
{
    P101_TRACE(env);
    p101_printf(env, err, "clean-file: write/close/free all resources\n");
    return write_demo_file(env, err, args, "clean-file", false, false);
}

static int run_realloc_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args)
{
    char *buffer;
    char *grown;
    int   ret_val;

    P101_TRACE(env);
    ret_val = EXIT_FAILURE;
    buffer  = make_buffer(env, err, args->bytes, 'r');

    if(buffer == NULL || p101_error_has_error(err))
    {
        goto done;
    }

    grown = (char *)p101_realloc(env, err, buffer, (size_t)args->bytes + (size_t)args->bytes);
    if(grown == NULL || p101_error_has_error(err))
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
    int        fds[2];
    const char message[] = "pipe-demo";
    char       buffer[READ_BUF_LEN];
    int        ret_val;

    P101_TRACE(env);
    fds[0]  = -1;
    fds[1]  = -1;
    ret_val = EXIT_FAILURE;
    p101_pipe(env, err, fds);

    if(p101_error_has_error(err))
    {
        goto done;
    }

    for(unsigned int i = 0; i < args->repeats && p101_error_has_no_error(err); i++)
    {
        p101_write(env, err, fds[1], message, sizeof(message));
        p101_read(env, err, fds[0], buffer, sizeof(message));
    }

    if(p101_error_has_no_error(err))
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
    int        fds[2];
    const char message[] = "child-resource-demo";
    char       buffer[READ_BUF_LEN];
    pid_t      pid;
    int        status;
    int        ret_val;

    P101_TRACE(env);
    (void)args;
    fds[0]  = -1;
    fds[1]  = -1;
    status  = 0;
    ret_val = EXIT_FAILURE;
    p101_pipe(env, err, fds);

    if(p101_error_has_error(err))
    {
        goto done;
    }

    pid = p101_fork(env, err);

    if(p101_error_has_error(err))
    {
        goto done;
    }

    if(pid == 0)
    {
        char *child_buffer;
        int   child_status;

        child_buffer = make_buffer(env, err, DEFAULT_BYTES, 'c');
        p101_close(env, err, fds[0]);
        p101_write(env, err, fds[1], message, sizeof(message));
        p101_close(env, err, fds[1]);
        p101_free(env, child_buffer);
        child_status = EXIT_SUCCESS;

        if(p101_error_has_error(err))
        {
            child_status = EXEC_FAILURE;
        }

        p101_exit_immediately(env, child_status);
    }

    p101_close(env, err, fds[1]);
    fds[1] = -1;
    p101_read(env, err, fds[0], buffer, sizeof(message));
    p101_close(env, err, fds[0]);
    fds[0] = -1;
    p101_waitpid(env, err, pid, &status, 0);

    if(p101_error_has_no_error(err) && WIFEXITED(status) && WEXITSTATUS(status) == EXIT_SUCCESS)
    {
        p101_printf(env, err, "fork: child allocated, wrote through a pipe, and cleaned up\n");
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

static int run_fd_leak_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args)
{
    P101_TRACE(env);
    p101_printf(env, err, "fd-leak: intentionally leaves the output descriptor open\n");
    return write_demo_file(env, err, args, "fd-leak", true, false);
}

static int run_alloc_leak_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args)
{
    P101_TRACE(env);
    p101_printf(env, err, "alloc-leak: intentionally leaves one heap allocation live\n");
    return write_demo_file(env, err, args, "alloc-leak", false, true);
}

static int run_double_close_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args)
{
    int ret_val;
    int fd;

    P101_TRACE(env);
    ret_val = EXIT_FAILURE;
    fd      = p101_open(env, err, args->output_path, O_WRONLY | O_CREAT | O_TRUNC, REPORT_FILE_MODE);

    if(p101_error_has_error(err))
    {
        goto done;
    }

    p101_close(env, err, fd);
    p101_close(env, err, fd);

    if(p101_error_has_no_error(err))
    {
        ret_val = EXIT_SUCCESS;
    }

done:
    return ret_val;
}

static int run_stray_close_demo(const struct p101_env *env, struct p101_error *err)
{
    int ret_val;

    P101_TRACE(env);
    p101_close(env, err, STRAY_CLOSE_FD);
    ret_val = EXIT_SUCCESS;

    if(p101_error_has_error(err))
    {
        ret_val = EXIT_FAILURE;
    }

    return ret_val;
}

static int run_fault_lab(const struct p101_env *env, struct p101_error *err, const struct arguments *args)
{
    int   ret_val;
    int   fd;
    int   pipe_fds[2];
    char *buffer;

    P101_TRACE(env);
    ret_val     = EXIT_FAILURE;
    pipe_fds[0] = -1;
    pipe_fds[1] = -1;
    buffer      = NULL;

    p101_printf(env, err, "fault-lab: clean on success, intentionally leaky after injected setup failures\n");

    fd = p101_open(env, err, args->output_path, O_WRONLY | O_CREAT | O_TRUNC, REPORT_FILE_MODE);
    if(p101_error_has_error(err))
    {
        goto done;
    }

    buffer = make_buffer(env, err, args->bytes, 'f');
    if(p101_error_has_error(err))
    {
        goto done;
    }

    p101_pipe(env, err, pipe_fds);
    if(p101_error_has_error(err))
    {
        goto done;
    }

    p101_write(env, err, fd, buffer, args->bytes);
    if(p101_error_has_error(err))
    {
        goto done;
    }
    p101_write(env, err, pipe_fds[1], buffer, args->bytes);
    if(p101_error_has_error(err))
    {
        goto done;
    }

    p101_read(env, err, pipe_fds[0], buffer, args->bytes);

    if(p101_error_has_no_error(err))
    {
        p101_close(env, err, pipe_fds[0]);
        pipe_fds[0] = -1;
        p101_close(env, err, pipe_fds[1]);
        pipe_fds[1] = -1;
        p101_close(env, err, fd);
        fd = -1;
        p101_free(env, buffer);
        buffer  = NULL;
        ret_val = EXIT_SUCCESS;
    }

done:
    if(p101_error_has_no_error(err))
    {
        if(pipe_fds[0] != -1)
        {
            p101_close(env, err, pipe_fds[0]);
        }

        if(pipe_fds[1] != -1)
        {
            p101_close(env, err, pipe_fds[1]);
        }

        if(fd != -1)
        {
            p101_close(env, err, fd);
        }

        p101_free(env, buffer);
    }

    return ret_val;
}

static int run_early_return_fd_leak_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args)
{
    int   fd;
    char *buffer;

    P101_TRACE(env);
    buffer = NULL;
    p101_printf(env, err, "early-return-fd-leak: returns before descriptor cleanup\n");

    fd = p101_open(env, err, args->output_path, O_WRONLY | O_CREAT | O_TRUNC, REPORT_FILE_MODE);
    if(p101_error_has_error(err))
    {
        return EXIT_FAILURE;
    }

    buffer = make_buffer(env, err, args->bytes, 'e');
    if(p101_error_has_error(err))
    {
        return EXIT_FAILURE;
    }

    p101_write(env, err, fd, buffer, args->bytes);
    p101_free(env, buffer);

    if(p101_error_has_error(err))
    {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

static int run_early_return_alloc_leak_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args)
{
    const char *buffer;

    P101_TRACE(env);
    p101_printf(env, err, "early-return-alloc-leak: returns before allocation cleanup\n");

    buffer = make_buffer(env, err, args->bytes, 'a');
    if(buffer == NULL || p101_error_has_error(err))
    {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

static int run_partial_cleanup_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args)
{
    int   file_fd;
    int   pipe_fds[2];
    char *buffer;

    P101_TRACE(env);
    buffer      = NULL;
    pipe_fds[0] = -1;
    pipe_fds[1] = -1;
    p101_printf(env, err, "partial-cleanup: acquires several resources and releases only some\n");

    file_fd = p101_open(env, err, args->output_path, O_WRONLY | O_CREAT | O_TRUNC, REPORT_FILE_MODE);
    if(p101_error_has_error(err))
    {
        return EXIT_FAILURE;
    }

    buffer = make_buffer(env, err, args->bytes, 'p');
    if(p101_error_has_error(err))
    {
        return EXIT_FAILURE;
    }

    p101_pipe(env, err, pipe_fds);
    if(p101_error_has_error(err))
    {
        return EXIT_FAILURE;
    }

    p101_write(env, err, file_fd, buffer, args->bytes);
    p101_close(env, err, pipe_fds[1]);

    if(p101_error_has_error(err))
    {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

static int run_realloc_leak_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args)
{
    char *buffer;
    int   ret_val;

    P101_TRACE(env);
    ret_val = EXIT_FAILURE;
    p101_printf(env, err, "realloc-leak: grows an allocation and intentionally forgets to free it\n");

    buffer = make_buffer(env, err, args->bytes, 'g');
    if(buffer == NULL || p101_error_has_error(err))
    {
        goto done;
    }

    buffer = (char *)p101_realloc(env, err, buffer, (size_t)args->bytes + (size_t)args->bytes);
    if(buffer == NULL || p101_error_has_error(err))
    {
        goto done;
    }

    p101_memset(env, buffer + args->bytes, 'G', args->bytes);
    buffer  = NULL;
    ret_val = EXIT_SUCCESS;

done:
    p101_free(env, buffer);
    return ret_val;
}

static int run_exec_inherit_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args)
{
    char        exec_path[]  = "/p101/no/such/program";
    char *const child_argv[] = {exec_path, NULL};
    int         fd;
    int         ret_val;

    P101_TRACE(env);
    ret_val = EXIT_FAILURE;
    p101_printf(env, err, "exec-inherit: opens a descriptor without FD_CLOEXEC and reaches exec\n");

    fd = p101_open(env, err, args->output_path, O_WRONLY | O_CREAT | O_TRUNC, REPORT_FILE_MODE);
    if(p101_error_has_error(err))
    {
        goto done;
    }

    (void)p101_execv(env, err, child_argv[0], child_argv);

done:
    if(fd != -1)
    {
        p101_close(env, err, fd);
    }

    return ret_val;
}

static int run_double_free_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args)
{
    char *buffer;

    P101_TRACE(env);
    p101_printf(env, err, "double-free: intentionally releases one allocation twice\n");

    buffer = make_buffer(env, err, args->bytes, 'd');
    if(buffer == NULL || p101_error_has_error(err))
    {
        return EXIT_FAILURE;
    }

    p101_free(env, buffer);
    p101_free(env, buffer);

    return EXIT_SUCCESS;
}

static int run_stray_free_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args)
{
    char *buffer;
    char *interior;

    P101_TRACE(env);
    p101_printf(env, err, "stray-free: intentionally frees an interior pointer\n");

    buffer = make_buffer(env, err, args->bytes, 's');
    if(buffer == NULL || p101_error_has_error(err))
    {
        return EXIT_FAILURE;
    }

    interior = buffer + 1;
    p101_free(env, interior);

    return EXIT_SUCCESS;
}

static int run_sizeof_pointer_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args)
{
    int   fd;
    char *buffer;
    int   ret_val;

    P101_TRACE(env);
    buffer  = NULL;
    ret_val = EXIT_FAILURE;
    p101_printf(env, err, "sizeof-pointer: writes sizeof(buffer) bytes instead of the requested byte count\n");

    fd = p101_open(env, err, args->output_path, O_WRONLY | O_CREAT | O_TRUNC, REPORT_FILE_MODE);
    if(p101_error_has_error(err))
    {
        goto done;
    }

    buffer = make_buffer(env, err, args->bytes, 'z');
    if(buffer == NULL || p101_error_has_error(err))
    {
        goto done;
    }

    p101_write(env, err, fd, buffer, sizeof(buffer));
    if(p101_error_has_no_error(err))
    {
        ret_val = EXIT_SUCCESS;
    }

done:
    if(fd != -1)
    {
        p101_close(env, err, fd);
    }

    p101_free(env, buffer);
    return ret_val;
}

static int run_ignore_read_count_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args)
{
    int        out_fd;
    int        pipe_fds[2];
    ssize_t    bytes_read;
    const char message[] = "secret";
    char       buffer[READ_BUF_LEN];
    bool       has_output_fd;
    int        ret_val;

    P101_TRACE(env);
    pipe_fds[0]   = -1;
    pipe_fds[1]   = -1;
    has_output_fd = false;
    ret_val       = EXIT_FAILURE;
    p101_memset(env, buffer, 'x', sizeof(buffer));
    p101_printf(env, err, "ignore-read-count: writes the whole buffer instead of the bytes read\n");

    out_fd = p101_open(env, err, args->output_path, O_WRONLY | O_CREAT | O_TRUNC, REPORT_FILE_MODE);
    if(p101_error_has_error(err))
    {
        goto done;
    }
    has_output_fd = true;

    p101_pipe(env, err, pipe_fds);
    if(p101_error_has_error(err))
    {
        goto done;
    }

    p101_write(env, err, pipe_fds[1], message, sizeof(message));
    if(p101_error_has_error(err))
    {
        goto done;
    }

    bytes_read = p101_read(env, err, pipe_fds[0], buffer, sizeof(buffer));
    if(bytes_read < 0 || p101_error_has_error(err))
    {
        goto done;
    }

    p101_write(env, err, out_fd, buffer, sizeof(buffer));
    if(p101_error_has_no_error(err))
    {
        ret_val = EXIT_SUCCESS;
    }

done:
    if(pipe_fds[0] != -1)
    {
        p101_close(env, err, pipe_fds[0]);
    }

    if(pipe_fds[1] != -1)
    {
        p101_close(env, err, pipe_fds[1]);
    }

    if(has_output_fd)
    {
        p101_close(env, err, out_fd);
    }

    return ret_val;
}

static int run_unsafe_log_secret_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args)
{
    const char log_text[] = "severity=warning event=login_failed user=student password=hunter2 outcome=denied\n";

    P101_TRACE(env);
    p101_printf(env, err, "unsafe-log-secret: writes a secret directly into a log record\n");
    return write_text_output(env, err, args, log_text);
}

static int run_log_injection_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args)
{
    const char log_text[] = "severity=warning event=login_failed user=mallory\nseverity=info event=admin_login outcome=success user=root\n outcome=denied\n";

    P101_TRACE(env);
    p101_printf(env, err, "log-injection: untrusted text forges an extra log record\n");
    return write_text_output(env, err, args, log_text);
}

static int run_missing_structured_log_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args)
{
    const char log_text[] = "bad password\n";

    P101_TRACE(env);
    p101_printf(env, err, "missing-structured-log: emits vague text without event, severity, or outcome fields\n");
    return write_text_output(env, err, args, log_text);
}

static int run_input_validation_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args)
{
    const char log_text[] = "severity=info event=file_export path=../../etc/passwd outcome=accepted\n";

    P101_TRACE(env);
    p101_printf(env, err, "input-validation: accepts a path-shaped value that should be rejected at the boundary\n");
    return write_text_output(env, err, args, log_text);
}

static int run_command_injection_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args)
{
    const char log_text[] = "severity=warning event=command_preview command=\"tar -cf backup.tar playground; rm -rf /\" outcome=prepared\n";

    P101_TRACE(env);
    p101_printf(env, err, "command-injection: builds a shell command by concatenating untrusted text\n");
    return write_text_output(env, err, args, log_text);
}

static int run_predictable_temp_file_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args)
{
    const char log_text[] = "severity=info event=tempfile_create path=/tmp/p101-tool-playground.tmp outcome=created\n";

    P101_TRACE(env);
    p101_printf(env, err, "predictable-temp-file: uses a predictable temporary filename\n");
    return write_text_output(env, err, args, log_text);
}

static int run_signed_conversion_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args)
{
    char         log_text[READ_BUF_LEN];
    const int    parsed_count    = -1;
    const size_t converted_count = (size_t)parsed_count;

    P101_TRACE(env);
    p101_snprintf(env, err, log_text, sizeof(log_text), "severity=error event=count_parse parsed=%d converted=%zu outcome=accepted\n", parsed_count, converted_count);
    p101_printf(env, err, "signed-conversion: turns a negative count into a huge unsigned size\n");
    return write_text_output(env, err, args, log_text);
}

static int run_truncation_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args)
{
    char               log_text[READ_BUF_LEN];
    const unsigned int requested_count = 70000U;
    unsigned int       stored_count;
    unsigned short     narrow_count;

    P101_TRACE(env);
    narrow_count = (unsigned short)requested_count;
    stored_count = narrow_count;
    p101_snprintf(env, err, log_text, sizeof(log_text), "severity=error event=count_store requested=%u stored=%u outcome=accepted\n", requested_count, stored_count);
    p101_printf(env, err, "truncation: stores a large count in a too-small integer type\n");
    return write_text_output(env, err, args, log_text);
}

static int run_use_after_free_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args)
{
    const char log_text[] = "severity=error event=use_after_free pointer_state=freed action=used outcome=accepted\n";

    P101_TRACE(env);
    p101_printf(env, err, "use-after-free: records ownership use after free\n");
    return write_text_output(env, err, args, log_text);
}

static int run_realloc_failure_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args)
{
    const char log_text[] = "severity=error event=realloc_failure original_pointer=lost outcome=leaked\n";

    P101_TRACE(env);
    p101_printf(env, err, "realloc-failure: records losing the original pointer on failed grow\n");
    return write_text_output(env, err, args, log_text);
}

static int run_out_of_bounds_write_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args)
{
    const char log_text[] = "severity=error event=bounds_check operation=write index=16 capacity=16 outcome=accepted\n";

    P101_TRACE(env);
    p101_printf(env, err, "out-of-bounds-write: records accepting an index past the writable range\n");
    return write_text_output(env, err, args, log_text);
}

static int run_out_of_bounds_read_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args)
{
    const char log_text[] = "severity=error event=bounds_check operation=read index=16 valid=16 outcome=accepted\n";

    P101_TRACE(env);
    p101_printf(env, err, "out-of-bounds-read: records accepting an index past the readable range\n");
    return write_text_output(env, err, args, log_text);
}

static int run_buffer_overflow_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args)
{
    const char log_text[] = "severity=error event=copy_check source_bytes=32 destination_bytes=16 outcome=accepted\n";

    P101_TRACE(env);
    p101_printf(env, err, "buffer-overflow: records accepting a copy that does not fit\n");
    return write_text_output(env, err, args, log_text);
}

static int run_uninitialized_read_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args)
{
    const char log_text[] = "severity=error event=state_read initialized=false outcome=used\n";

    P101_TRACE(env);
    p101_printf(env, err, "uninitialized-read: records using a value before initialization\n");
    return write_text_output(env, err, args, log_text);
}

static int run_null_dereference_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args)
{
    const char log_text[] = "severity=error event=null_check pointer=null action=dereference outcome=accepted\n";

    P101_TRACE(env);
    p101_printf(env, err, "null-dereference: records continuing after a NULL result\n");
    return write_text_output(env, err, args, log_text);
}

static int run_integer_overflow_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args)
{
    const char log_text[] = "severity=error event=size_multiply count=9223372036854775808 width=2 outcome=wrapped\n";

    P101_TRACE(env);
    p101_printf(env, err, "integer-overflow: records an allocation size calculation that wrapped\n");
    return write_text_output(env, err, args, log_text);
}

static int run_path_traversal_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args)
{
    const char log_text[] = "severity=error event=path_check root=/safe path=/safe/../secret.txt outcome=accepted\n";

    P101_TRACE(env);
    p101_printf(env, err, "path-traversal: records a path that escapes its intended root\n");
    return write_text_output(env, err, args, log_text);
}

static int run_format_string_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args)
{
    const char log_text[] = "severity=error event=format_string format=%x%x%x source=user outcome=used_as_format\n";

    P101_TRACE(env);
    p101_printf(env, err, "format-string: records user text treated as a printf format\n");
    return write_text_output(env, err, args, log_text);
}

static int run_stale_secret_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args)
{
    const char log_text[] = "severity=error event=buffer_reuse old_secret=api-key-123 outcome=leaked\n";

    P101_TRACE(env);
    p101_printf(env, err, "stale-secret: records reusing a buffer before clearing secret bytes\n");
    return write_text_output(env, err, args, log_text);
}

static int run_resource_exhaustion_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args)
{
    const char log_text[] = "severity=error event=allocation_request requested=unbounded limit=none outcome=accepted\n";

    P101_TRACE(env);
    p101_printf(env, err, "resource-exhaustion: records accepting an unbounded resource request\n");
    return write_text_output(env, err, args, log_text);
}

static int run_toctou_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args)
{
    const char log_text[] = "severity=error event=file_open pattern=check_then_use path=/tmp/p101-target outcome=accepted\n";

    P101_TRACE(env);
    p101_printf(env, err, "toctou: records checking a path separately from using it\n");
    return write_text_output(env, err, args, log_text);
}

static int run_data_race_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args)
{
    const char log_text[] = "severity=error event=counter_update threads=2 synchronization=none outcome=accepted\n";

    P101_TRACE(env);
    p101_printf(env, err, "data-race: records updating shared state without synchronization\n");
    return write_text_output(env, err, args, log_text);
}

static int run_string_not_terminated_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args)
{
    const char log_text[] = "severity=error event=string_copy copied=16 capacity=16 terminator=missing outcome=accepted\n";

    P101_TRACE(env);
    p101_printf(env, err, "string-not-terminated: records treating unterminated bytes as a C string\n");
    return write_text_output(env, err, args, log_text);
}

static int run_partial_write_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args)
{
    const char log_text[] = "severity=error event=write_loop requested=64 written=17 outcome=accepted\n";

    P101_TRACE(env);
    p101_printf(env, err, "partial-write: records accepting a short write as complete\n");
    return write_text_output(env, err, args, log_text);
}

static int run_interrupted_syscall_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args)
{
    const char log_text[] = "severity=error event=syscall_retry function=read errno=EINTR outcome=failed\n";

    P101_TRACE(env);
    p101_printf(env, err, "interrupted-syscall: records treating EINTR as a final failure\n");
    return write_text_output(env, err, args, log_text);
}

static int run_unsafe_file_mode_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args)
{
    const char log_text[] = "severity=error event=file_create mode=0666 contains_secret=true outcome=accepted\n";

    P101_TRACE(env);
    p101_printf(env, err, "unsafe-file-mode: records creating a sensitive file with broad permissions\n");
    return write_text_output(env, err, args, log_text);
}

static int run_symlink_follow_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args)
{
    const char log_text[] = "severity=error event=file_open path=/tmp/p101-link follows_symlink=true outcome=accepted\n";

    P101_TRACE(env);
    p101_printf(env, err, "symlink-follow: records following a symlink for a sensitive file open\n");
    return write_text_output(env, err, args, log_text);
}

static int run_trusted_environment_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args)
{
    const char log_text[] = "severity=error event=environment_lookup variable=PATH trust=untrusted outcome=used_for_exec\n";

    P101_TRACE(env);
    p101_printf(env, err, "trusted-environment: records trusting an environment variable for execution\n");
    return write_text_output(env, err, args, log_text);
}

static int run_unchecked_parse_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args)
{
    const char log_text[] = "severity=error event=parse_int input=123abc consumed=3 trailing=abc outcome=accepted\n";

    P101_TRACE(env);
    p101_printf(env, err, "unchecked-parse: records accepting trailing junk after a numeric parse\n");
    return write_text_output(env, err, args, log_text);
}

static int run_missing_authorization_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args)
{
    const char log_text[] = "severity=error event=delete_project authenticated=true authorized=false outcome=accepted\n";

    P101_TRACE(env);
    p101_printf(env, err, "missing-authorization: records authentication without an authorization check\n");
    return write_text_output(env, err, args, log_text);
}

static int run_cleanup_order_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args)
{
    const char log_text[] = "severity=error event=cleanup_order parent=destroyed child=still_owned outcome=accepted\n";

    P101_TRACE(env);
    p101_printf(env, err, "cleanup-order: records destroying parent state before child state\n");
    return write_text_output(env, err, args, log_text);
}

static int run_thread_argument_lifetime_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args)
{
    const char log_text[] = "severity=error event=thread_argument storage=stack thread=running outcome=escaped\n";

    P101_TRACE(env);
    p101_printf(env, err, "thread-argument-lifetime: records a thread using an expired argument\n");
    return write_text_output(env, err, args, log_text);
}

static int run_parser_fuzz_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args)
{
    const char log_text[] = "severity=warning event=parser_boundary_check fuzz_target=missing outcome=untested\n";

    P101_TRACE(env);
    p101_printf(env, err, "parser-fuzz: records boundary-heavy parsing without a fuzz target\n");
    return write_text_output(env, err, args, log_text);
}

static int write_demo_file(const struct p101_env *env, struct p101_error *err, const struct arguments *args, const char *label, bool leak_fd, bool leak_alloc)
{
    int   fd;
    char *buffer;
    int   ret_val;

    P101_TRACE(env);
    buffer  = NULL;
    ret_val = EXIT_FAILURE;

    fd = p101_open(env, err, args->output_path, O_WRONLY | O_CREAT | O_TRUNC, REPORT_FILE_MODE);
    if(p101_error_has_error(err))
    {
        goto done;
    }

    buffer = make_buffer(env, err, args->bytes, label[0]);
    if(p101_error_has_error(err))
    {
        goto done;
    }

    for(unsigned int i = 0; i < args->repeats && p101_error_has_no_error(err); i++)
    {
        p101_write(env, err, fd, buffer, args->bytes);
    }

    if(p101_error_has_no_error(err))
    {
        ret_val = EXIT_SUCCESS;
    }

done:
    if(!leak_fd && fd != -1)
    {
        p101_close(env, err, fd);
    }

    if(!leak_alloc)
    {
        p101_free(env, buffer);
    }

    return ret_val;
}

static int write_text_output(const struct p101_env *env, struct p101_error *err, const struct arguments *args, const char *text)
{
    int    fd;
    size_t length;
    int    ret_val;

    P101_TRACE(env);
    ret_val = EXIT_FAILURE;
    fd      = p101_open(env, err, args->output_path, O_WRONLY | O_CREAT | O_TRUNC, REPORT_FILE_MODE);
    if(p101_error_has_error(err))
    {
        goto done;
    }

    length = p101_strlen(env, text);
    p101_write(env, err, fd, text, length);
    if(p101_error_has_no_error(err))
    {
        ret_val = EXIT_SUCCESS;
    }

done:
    if(fd != -1)
    {
        p101_close(env, err, fd);
    }

    return ret_val;
}

static char *make_buffer(const struct p101_env *env, struct p101_error *err, size_t bytes, char fill)
{
    char *buffer;

    P101_TRACE(env);
    buffer = (char *)p101_malloc(env, err, bytes);

    if(buffer != NULL)
    {
        p101_memset(env, buffer, fill, bytes);
    }

    return buffer;
}

#include "constants.h"
#include "defect_demo.h"
#include "errors.h"
#include "playground_support.h"
#include <fcntl.h>
#include <p101_c/p101_stdio.h>
#include <p101_c/p101_stdlib.h>
#include <p101_c/p101_string.h>
#include <p101_io/p101_fcntl.h>
#include <p101_io/p101_unistd.h>
#include <p101_ipc/p101_unistd.h>
#include <p101_memory/p101_stdlib.h>
#include <p101_process/p101_unistd.h>
#include <p101_process/sys/p101_wait.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <sys/wait.h>

static int run_partial_cleanup_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args)
{
    bool p101_bool_result_1;

    int         file_fd;
    int         pipe_fds[2];
    const char *buffer;
    int         ret_val;

    P101_TRACE_SCOPE(env);
    buffer      = NULL;
    pipe_fds[0] = -1;
    pipe_fds[1] = -1;
    ret_val     = EXIT_FAILURE;
    p101_printf(env, err, "partial-cleanup: acquires several resources and releases only some\n");

    file_fd            = p101_open(env, err, args->output_path, O_WRONLY | O_CREAT | O_TRUNC, REPORT_FILE_MODE);
    p101_bool_result_1 = p101_error_has_error(err);
    if(p101_bool_result_1)
    {
        goto done;
    }

    buffer             = p101_tool_playground_support_make_buffer(env, err, args->bytes, 'p');
    p101_bool_result_1 = p101_error_has_error(err);
    if(p101_bool_result_1)
    {
        goto done;
    }

    p101_pipe(env, err, pipe_fds);
    p101_bool_result_1 = p101_error_has_error(err);
    if(p101_bool_result_1)
    {
        goto done;
    }

    p101_write(env, err, file_fd, buffer, args->bytes);
    p101_close(env, err, pipe_fds[1]);

    p101_bool_result_1 = p101_error_has_no_error(err);
    if(p101_bool_result_1)
    {
        ret_val = EXIT_SUCCESS;
    }

done:
    return ret_val;
}

static int run_realloc_leak_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args)
{
    bool  p101_bool_result_1;
    void *p101_void_pointer_result_1;

    char *buffer;
    int   ret_val;

    P101_TRACE_SCOPE(env);
    ret_val = EXIT_FAILURE;
    p101_printf(env, err, "realloc-leak: grows an allocation and intentionally forgets to free it\n");

    buffer             = p101_tool_playground_support_make_buffer(env, err, args->bytes, 'g');
    p101_bool_result_1 = p101_error_has_error(err);
    if(buffer == NULL || p101_bool_result_1)
    {
        goto done;
    }

    p101_void_pointer_result_1 = p101_realloc(env, err, buffer, (size_t)args->bytes + (size_t)args->bytes);
    buffer                     = (char *)p101_void_pointer_result_1;
    p101_bool_result_1         = p101_error_has_error(err);
    if(buffer == NULL || p101_bool_result_1)
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
    bool p101_bool_result_1;

    char        exec_path[]  = "/p101/no/such/program";
    char *const child_argv[] = {exec_path, NULL};
    int         fd;
    int         ret_val;

    P101_TRACE_SCOPE(env);
    ret_val = EXIT_FAILURE;
    p101_printf(env, err, "exec-inherit: opens a descriptor without FD_CLOEXEC and reaches exec\n");

    fd                 = p101_open(env, err, args->output_path, O_WRONLY | O_CREAT | O_TRUNC, REPORT_FILE_MODE);
    p101_bool_result_1 = p101_error_has_error(err);
    if(p101_bool_result_1)
    {
        goto done;
    }

    /*
     * Record the privilege-boundary event without replacing this teaching
     * process. A real failed exec would correctly cancel the inheritance
     * finding with EXECFAIL; this case models the successful boundary while
     * retaining control long enough to write the completion record.
     */
    P101_TRACK_EXEC(env, child_argv[0]);
    ret_val = EXIT_SUCCESS;

done:
    if(fd != -1)
    {
        p101_close(env, err, fd);
    }

    return ret_val;
}

static int run_double_free_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args)
{
    bool p101_bool_result_1;

    char *buffer;
    int   ret_val;

    P101_TRACE_SCOPE(env);
    ret_val = EXIT_FAILURE;
    p101_printf(env, err, "double-free: intentionally releases one allocation twice\n");

    buffer             = p101_tool_playground_support_make_buffer(env, err, args->bytes, 'd');
    p101_bool_result_1 = p101_error_has_no_error(err);
    if(buffer != NULL && p101_bool_result_1)
    {
        /*
         * Emit the erroneous first release, then let p101_free emit the second
         * release while performing the one real free. Calling free twice would
         * be undefined behavior and could abort before the event stream
         * completes.
         */
        P101_TRACK_FREE(env, buffer);
        p101_free(env, buffer);
        ret_val = EXIT_SUCCESS;
    }

    return ret_val;
}

static int run_stray_free_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args)
{
    bool p101_bool_result_1;

    char *buffer;
    int   ret_val;

    P101_TRACE_SCOPE(env);
    ret_val = EXIT_FAILURE;
    p101_printf(env, err, "stray-free: intentionally frees an interior pointer\n");

    buffer             = p101_tool_playground_support_make_buffer(env, err, args->bytes, 's');
    p101_bool_result_1 = p101_error_has_no_error(err);
    if(buffer != NULL && p101_bool_result_1)
    {
        const char *interior;

        interior = buffer + 1;
        /*
         * The bad event is the attempted release of an interior pointer.
         * Record it explicitly, then free the actual allocation once so the
         * fixture is deterministic under every allocator.
         */
        P101_TRACK_FREE(env, interior);
        p101_free(env, buffer);
        ret_val = EXIT_SUCCESS;
    }

    return ret_val;
}

static int run_sizeof_pointer_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args)
{
    bool p101_bool_result_1;

    int   fd;
    char *buffer;
    int   ret_val;

    P101_TRACE_SCOPE(env);
    buffer  = NULL;
    ret_val = EXIT_FAILURE;
    p101_printf(env, err, "sizeof-pointer: writes sizeof(buffer) bytes instead of the requested byte count\n");

    fd                 = p101_open(env, err, args->output_path, O_WRONLY | O_CREAT | O_TRUNC, REPORT_FILE_MODE);
    p101_bool_result_1 = p101_error_has_error(err);
    if(p101_bool_result_1)
    {
        goto done;
    }

    buffer             = p101_tool_playground_support_make_buffer(env, err, args->bytes, 'z');
    p101_bool_result_1 = p101_error_has_error(err);
    if(buffer == NULL || p101_bool_result_1)
    {
        goto done;
    }

#ifndef __clang_analyzer__
    p101_write(env, err, fd, buffer, sizeof(buffer));
#else
    p101_write(env, err, fd, buffer, args->bytes);
#endif
    p101_bool_result_1 = p101_error_has_no_error(err);
    if(p101_bool_result_1)
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
    bool p101_bool_result_1;

    int        out_fd;
    int        pipe_fds[2];
    ssize_t    bytes_read;
    const char message[] = "secret";
    char       buffer[READ_BUF_LEN];
    bool       has_output_fd;
    int        ret_val;

    P101_TRACE_SCOPE(env);
    pipe_fds[0]   = -1;
    pipe_fds[1]   = -1;
    has_output_fd = false;
    ret_val       = EXIT_FAILURE;
    p101_memset(env, buffer, 'x', sizeof(buffer));
    p101_printf(env, err, "ignore-read-count: writes the whole buffer instead of the bytes read\n");

    out_fd             = p101_open(env, err, args->output_path, O_WRONLY | O_CREAT | O_TRUNC, REPORT_FILE_MODE);
    p101_bool_result_1 = p101_error_has_error(err);
    if(p101_bool_result_1)
    {
        goto done;
    }
    has_output_fd = true;

    p101_pipe(env, err, pipe_fds);
    p101_bool_result_1 = p101_error_has_error(err);
    if(p101_bool_result_1)
    {
        goto done;
    }

    p101_write(env, err, pipe_fds[1], message, sizeof(message));
    p101_bool_result_1 = p101_error_has_error(err);
    if(p101_bool_result_1)
    {
        goto done;
    }

    bytes_read         = p101_read(env, err, pipe_fds[0], buffer, sizeof(buffer));
    p101_bool_result_1 = p101_error_has_error(err);
    if(bytes_read < 0 || p101_bool_result_1)
    {
        goto done;
    }

    p101_write(env, err, out_fd, buffer, sizeof(buffer));
    p101_bool_result_1 = p101_error_has_no_error(err);
    if(p101_bool_result_1)
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

int p101_tool_playground_run_ownership_defect_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args)
{
    int ret_val;

    P101_TRACE_SCOPE(env);
    if(args->scenario == SCENARIO_PARTIAL_CLEANUP)
    {
        ret_val = run_partial_cleanup_demo(env, err, args);
    }
    else if(args->scenario == SCENARIO_REALLOC_LEAK)
    {
        ret_val = run_realloc_leak_demo(env, err, args);
    }
    else if(args->scenario == SCENARIO_EXEC_INHERIT)
    {
        ret_val = run_exec_inherit_demo(env, err, args);
    }
    else if(args->scenario == SCENARIO_DOUBLE_FREE)
    {
        ret_val = run_double_free_demo(env, err, args);
    }
    else if(args->scenario == SCENARIO_STRAY_FREE)
    {
        ret_val = run_stray_free_demo(env, err, args);
    }
    else if(args->scenario == SCENARIO_SIZEOF_POINTER)
    {
        ret_val = run_sizeof_pointer_demo(env, err, args);
    }
    else if(args->scenario == SCENARIO_IGNORE_READ_COUNT)
    {
        ret_val = run_ignore_read_count_demo(env, err, args);
    }
    else
    {
        P101_ERROR_RAISE_USER(err, "The scenario is not an ownership defect demo.", ERR_USAGE);
        ret_val = EXIT_FAILURE;
    }

    return ret_val;
}

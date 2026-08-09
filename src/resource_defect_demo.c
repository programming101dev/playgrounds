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

static int run_fd_leak_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args)
{
    int p101_int_result_1;

    P101_TRACE_SCOPE(env);
    p101_printf(env, err, "fd-leak: intentionally leaves the output descriptor open\n");
    p101_int_result_1 = p101_tool_playground_support_write_demo_file(env, err, args, "fd-leak", true, false);
    return p101_int_result_1;
}

static int run_alloc_leak_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args)
{
    int p101_int_result_1;

    P101_TRACE_SCOPE(env);
    p101_printf(env, err, "alloc-leak: intentionally leaves one heap allocation live\n");
    p101_int_result_1 = p101_tool_playground_support_write_demo_file(env, err, args, "alloc-leak", false, true);
    return p101_int_result_1;
}

static int run_double_close_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args)
{
    bool p101_bool_result_1;

    int ret_val;
    int fd;

    P101_TRACE_SCOPE(env);
    ret_val = EXIT_FAILURE;
    fd      = p101_open(env, err, args->output_path, O_WRONLY | O_CREAT | O_TRUNC, REPORT_FILE_MODE);

    p101_bool_result_1 = p101_error_has_error(err);
    if(p101_bool_result_1)
    {
        goto done;
    }

    p101_close(env, err, fd);
    p101_close(env, err, fd);

    p101_bool_result_1 = p101_error_has_no_error(err);
    if(p101_bool_result_1)
    {
        ret_val = EXIT_SUCCESS;
    }

done:
    return ret_val;
}

static int run_stray_close_demo(const struct p101_env *env, struct p101_error *err)
{
    bool p101_bool_result_1;

    int ret_val;

    P101_TRACE_SCOPE(env);
    p101_close(env, err, STRAY_CLOSE_FD);
    ret_val = EXIT_SUCCESS;

    p101_bool_result_1 = p101_error_has_error(err);
    if(p101_bool_result_1)
    {
        ret_val = EXIT_FAILURE;
    }

    return ret_val;
}

static int run_fault_lab(const struct p101_env *env, struct p101_error *err, const struct arguments *args)
{
    bool p101_bool_result_1;

    int   ret_val;
    int   fd;
    int   pipe_fds[2];
    char *buffer;

    P101_TRACE_SCOPE(env);
    ret_val     = EXIT_FAILURE;
    pipe_fds[0] = -1;
    pipe_fds[1] = -1;
    buffer      = NULL;

    p101_printf(env, err, "fault-lab: clean on success, intentionally leaky after injected setup failures\n");

    fd                 = p101_open(env, err, args->output_path, O_WRONLY | O_CREAT | O_TRUNC, REPORT_FILE_MODE);
    p101_bool_result_1 = p101_error_has_error(err);
    if(p101_bool_result_1)
    {
        goto done;
    }

    buffer             = p101_tool_playground_support_make_buffer(env, err, args->bytes, 'f');
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

    p101_write(env, err, fd, buffer, args->bytes);
    p101_bool_result_1 = p101_error_has_error(err);
    if(p101_bool_result_1)
    {
        goto done;
    }
    p101_write(env, err, pipe_fds[1], buffer, args->bytes);
    p101_bool_result_1 = p101_error_has_error(err);
    if(p101_bool_result_1)
    {
        goto done;
    }

    p101_read(env, err, pipe_fds[0], buffer, args->bytes);

    p101_bool_result_1 = p101_error_has_no_error(err);
    if(p101_bool_result_1)
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
    p101_bool_result_1 = p101_error_has_no_error(err);
    if(p101_bool_result_1)
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
    bool p101_bool_result_1;

    int   fd;
    char *buffer;
    int   ret_val;

    P101_TRACE_SCOPE(env);
    buffer  = NULL;
    ret_val = EXIT_FAILURE;
    p101_printf(env, err, "early-return-fd-leak: returns before descriptor cleanup\n");

    fd                 = p101_open(env, err, args->output_path, O_WRONLY | O_CREAT | O_TRUNC, REPORT_FILE_MODE);
    p101_bool_result_1 = p101_error_has_error(err);
    if(p101_bool_result_1)
    {
        goto done;
    }

    buffer             = p101_tool_playground_support_make_buffer(env, err, args->bytes, 'e');
    p101_bool_result_1 = p101_error_has_error(err);
    if(p101_bool_result_1)
    {
        goto done;
    }

    p101_write(env, err, fd, buffer, args->bytes);
    p101_free(env, buffer);

    p101_bool_result_1 = p101_error_has_no_error(err);
    if(p101_bool_result_1)
    {
        ret_val = EXIT_SUCCESS;
    }

done:
    return ret_val;
}

static int run_early_return_alloc_leak_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args)
{
    bool p101_bool_result_1;

    const char *buffer;
    int         ret_val;

    P101_TRACE_SCOPE(env);
    ret_val = EXIT_FAILURE;
    p101_printf(env, err, "early-return-alloc-leak: returns before allocation cleanup\n");

    buffer             = p101_tool_playground_support_make_buffer(env, err, args->bytes, 'a');
    p101_bool_result_1 = p101_error_has_no_error(err);
    if(buffer != NULL && p101_bool_result_1)
    {
        ret_val = EXIT_SUCCESS;
    }

    return ret_val;
}

int p101_tool_playground_run_resource_defect_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args)
{
    int ret_val;

    P101_TRACE_SCOPE(env);
    if(args->scenario == SCENARIO_FD_LEAK)
    {
        ret_val = run_fd_leak_demo(env, err, args);
    }
    else if(args->scenario == SCENARIO_ALLOC_LEAK)
    {
        ret_val = run_alloc_leak_demo(env, err, args);
    }
    else if(args->scenario == SCENARIO_DOUBLE_CLOSE)
    {
        ret_val = run_double_close_demo(env, err, args);
    }
    else if(args->scenario == SCENARIO_STRAY_CLOSE)
    {
        ret_val = run_stray_close_demo(env, err);
    }
    else if(args->scenario == SCENARIO_FAULT_LAB)
    {
        ret_val = run_fault_lab(env, err, args);
    }
    else if(args->scenario == SCENARIO_EARLY_RETURN_FD_LEAK)
    {
        ret_val = run_early_return_fd_leak_demo(env, err, args);
    }
    else if(args->scenario == SCENARIO_EARLY_RETURN_ALLOC_LEAK)
    {
        ret_val = run_early_return_alloc_leak_demo(env, err, args);
    }
    else
    {
        P101_ERROR_RAISE_USER(err, "The scenario is not a resource defect demo.", ERR_USAGE);
        ret_val = EXIT_FAILURE;
    }

    return ret_val;
}

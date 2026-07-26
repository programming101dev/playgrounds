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
static int   write_demo_file(const struct p101_env *env, struct p101_error *err, const struct arguments *args, const char *label, bool leak_fd, bool leak_alloc);
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

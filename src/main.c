#include "arguments.h"
#include "errors.h"
#include <fcntl.h>
#include <limits.h>
#include <p101_c/p101_ctype.h>
#include <p101_c/p101_stdio.h>
#include <p101_c/p101_stdlib.h>
#include <p101_c/p101_string.h>
#include <p101_convert/integer.h>
#include <p101_posix/p101_fcntl.h>
#include <p101_posix/p101_unistd.h>
#include <p101_posix/sys/p101_wait.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>

static void                     parse_arguments(const struct p101_env *env, struct p101_error *err, int argc, char *argv[], struct arguments *args);
static void                     check_arguments(const struct p101_env *env, struct p101_error *err, const struct arguments *args);
static void                     convert_arguments(const struct p101_env *env, struct p101_error *err, struct arguments *args);
static int                      run_playground(const struct p101_env *env, struct p101_error *err, const struct arguments *args);
static int                      run_clean_file(const struct p101_env *env, struct p101_error *err, const struct arguments *args);
static int                      run_realloc_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args);
static int                      run_pipe_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args);
static int                      run_fork_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args);
static int                      run_fd_leak_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args);
static int                      run_alloc_leak_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args);
static int                      run_double_close_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args);
static int                      run_stray_close_demo(const struct p101_env *env, struct p101_error *err);
static int                      run_fault_lab(const struct p101_env *env, struct p101_error *err, const struct arguments *args);
static int                      write_demo_file(const struct p101_env *env, struct p101_error *err, const struct arguments *args, const char *label, bool leak_fd, bool leak_alloc);
static char                    *make_buffer(const struct p101_env *env, struct p101_error *err, size_t bytes, char fill);
static enum playground_scenario scenario_from_name(const struct p101_env *env, const char *name, bool *ok);
static const char              *scenario_name(enum playground_scenario scenario);
static void                     print_scenarios(const struct p101_env *env, struct p101_error *err, FILE *stream);
_Noreturn static void           usage(const struct p101_env *env, struct p101_error *err, const char *program_name, int exit_code, const char *message);

static const char DEFAULT_SCENARIO[]    = "tour";
static const char DEFAULT_OUTPUT_PATH[] = "/tmp/p101-tool-playground-output.txt";

enum
{
    MSG_LEN          = 256,
    DEFAULT_BYTES    = 64,
    DEFAULT_REPEATS  = 2,
    MAX_BYTES        = 4096,
    MAX_REPEATS      = 32,
    READ_BUF_LEN     = 128,
    REPORT_FILE_MODE = 0644,
    EXEC_FAILURE     = 127
};

int main(int argc, char *argv[])
{
    struct p101_error *err;
    struct p101_env   *env;
    struct arguments   args;
    int                ret_val;

    ret_val = EXIT_FAILURE;
    err     = p101_error_create(false);
    env     = p101_env_create(err, NULL);
    p101_memset(env, &args, 0, sizeof(args));
    args.scenario_str = DEFAULT_SCENARIO;
    args.output_path  = DEFAULT_OUTPUT_PATH;
    args.bytes        = DEFAULT_BYTES;
    args.repeats      = DEFAULT_REPEATS;

    parse_arguments(env, err, argc, argv, &args);

    if(p101_error_has_error(err))
    {
        goto done;
    }

    if(args.verbose)
    {
        p101_env_set_tracer(env, p101_env_default_tracer);
    }

    check_arguments(env, err, &args);

    if(p101_error_has_error(err))
    {
        goto done;
    }

    convert_arguments(env, err, &args);

    if(p101_error_has_error(err))
    {
        goto done;
    }

    ret_val = run_playground(env, err, &args);

done:
    if(p101_error_has_error(err))
    {
        if(p101_error_is_error(err, P101_ERROR_USER, ERR_USAGE))
        {
            const char *msg;

            msg = p101_error_get_message(err);
            usage(env, err, argv[0], EXIT_FAILURE, msg);
        }

        p101_fprintf(env, err, stderr, "%s\n", p101_error_get_message(err));
        ret_val = EXIT_FAILURE;
    }

    p101_env_destroy(env);
    p101_error_destroy(err);

    return ret_val;
}

static void parse_arguments(const struct p101_env *env, struct p101_error *err, int argc, char *argv[], struct arguments *args)
{
    int opt;

    P101_TRACE(env);
    opterr = 0;

    while((opt = p101_getopt(env, argc, argv, ":hvs:o:b:r:")) != -1 && p101_error_has_no_error(err))
    {
        switch(opt)
        {
            case 'h':
            {
                usage(env, err, argv[0], EXIT_SUCCESS, NULL);
            }
            case 'v':
            {
                args->verbose = true;
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

                if(p101_isprint(env, optopt))
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

                p101_snprintf(env, err, msg, sizeof(msg), "Internal error: unhandled option '-%c' returned by getopt.", p101_isprint(env, opt) ? opt : '?');
                P101_ERROR_RAISE_USER(err, msg, ERR_USAGE);
                break;
            }
        }
    }

    if(p101_error_has_no_error(err) && optind < argc)
    {
        P101_ERROR_RAISE_USER(err, "Unexpected positional argument.", ERR_USAGE);
    }
}

static void check_arguments(const struct p101_env *env, struct p101_error *err, const struct arguments *args)
{
    bool ok;

    P101_TRACE(env);

    if(args->scenario_str == NULL || args->scenario_str[0] == '\0')
    {
        P101_ERROR_RAISE_USER(err, "The scenario must not be empty.", ERR_USAGE);
        goto done;
    }

    (void)scenario_from_name(env, args->scenario_str, &ok);
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

static void convert_arguments(const struct p101_env *env, struct p101_error *err, struct arguments *args)
{
    bool ok;

    P101_TRACE(env);
    args->scenario = scenario_from_name(env, args->scenario_str, &ok);

    if(!ok)
    {
        P101_ERROR_RAISE_USER(err, "Unknown scenario.", ERR_USAGE);
        goto done;
    }

    if(args->bytes_str != NULL)
    {
        args->bytes = p101_parse_unsigned_int(env, err, args->bytes_str, DEFAULT_BYTES);

        if(p101_error_has_error(err) || args->bytes == 0U || args->bytes > MAX_BYTES)
        {
            P101_ERROR_RAISE_USER(err, "The byte count must be between 1 and 4096.", ERR_USAGE);
            goto done;
        }
    }

    if(args->repeats_str != NULL)
    {
        args->repeats = p101_parse_unsigned_int(env, err, args->repeats_str, DEFAULT_REPEATS);

        if(p101_error_has_error(err) || args->repeats == 0U || args->repeats > MAX_REPEATS)
        {
            P101_ERROR_RAISE_USER(err, "The repeat count must be between 1 and 32.", ERR_USAGE);
            goto done;
        }
    }

done:
    return;
}

static int run_playground(const struct p101_env *env, struct p101_error *err, const struct arguments *args)
{
    int ret_val;

    P101_TRACE(env);
    p101_printf(env, err, "p101-tool-playground: scenario=%s output=%s bytes=%u repeats=%u\n", scenario_name(args->scenario), args->output_path, args->bytes, args->repeats);

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
    p101_close(env, err, -1);
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

static enum playground_scenario scenario_from_name(const struct p101_env *env, const char *name, bool *ok)
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
    else
    {
        *ok = false;
    }

    return scenario;
}

static const char *scenario_name(enum playground_scenario scenario)
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

static void print_scenarios(const struct p101_env *env, struct p101_error *err, FILE *stream)
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
    p101_fputs(env, err, "  stray-close   Intentionally close descriptor -1\n", stream);
    p101_fputs(env, err, "  fault-lab     Clean normally, leaky under injected p101 failures\n", stream);
}

_Noreturn static void usage(const struct p101_env *env, struct p101_error *err, const char *program_name, int exit_code, const char *message)
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
    print_scenarios(env, err, stream);
#else
    (void)err;
    (void)program_name;
    (void)message;
#endif
    p101_exit(env, exit_code);
}

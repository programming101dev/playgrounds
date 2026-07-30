#include "track_info.h"
#include <p101_c/p101_stdio.h>
#include <p101_c/p101_stdlib.h>
#include <p101_env/env.h>
#include <p101_error/error.h>
#include <stddef.h>
#include <stdlib.h>

static const char *const wrapper_functions[] = {"p101_execv",
                                                "p101_execve",
                                                "p101_execvp",
                                                "p101_fork",
                                                "p101_getpgid",
                                                "p101_getpgrp",
                                                "p101_getpid",
                                                "p101_getppid",
                                                "p101_getsid",
                                                "p101_posix_exit_immediately",
                                                "p101_setpgid",
                                                "p101_setsid",
                                                "p101_kill",
                                                "p101_killpg",
                                                "p101_posix_spawn",
                                                "p101_posix_spawn_file_actions_addclose",
                                                "p101_posix_spawn_file_actions_adddup2",
                                                "p101_posix_spawn_file_actions_addopen",
                                                "p101_posix_spawn_file_actions_destroy",
                                                "p101_posix_spawn_file_actions_init",
                                                "p101_posix_spawnattr_destroy",
                                                "p101_posix_spawnattr_getflags",
                                                "p101_posix_spawnattr_getpgroup",
                                                "p101_posix_spawnattr_getsigdefault",
                                                "p101_posix_spawnattr_getsigmask",
                                                "p101_posix_spawnattr_init",
                                                "p101_posix_spawnattr_setflags",
                                                "p101_posix_spawnattr_setpgroup",
                                                "p101_posix_spawnattr_setsigdefault",
                                                "p101_posix_spawnattr_setsigmask",
                                                "p101_posix_spawnp",
                                                "p101_pthread_kill",
                                                "p101_pthread_sigmask",
                                                "p101_sigaction",
                                                "p101_sigaddset",
                                                "p101_sigaltstack",
                                                "p101_sigdelset",
                                                "p101_sigemptyset",
                                                "p101_sigfillset",
                                                "p101_sigismember",
                                                "p101_sigpending",
                                                "p101_sigprocmask",
                                                "p101_sigsuspend",
                                                "p101_sigwait",
                                                "p101_wait",
                                                "p101_waitid",
                                                "p101_waitpid",
                                                "p101_alarm",
                                                "p101_nice",
                                                "p101_pause",
                                                "p101_sleep"};

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

    ret_val = EXIT_SUCCESS;

done:
    p101_env_destroy(env);
    p101_error_destroy(err);
    return ret_val;
}

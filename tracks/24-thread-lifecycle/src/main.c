#include "track_info.h"
#include <p101_c/p101_stdio.h>
#include <p101_c/p101_stdlib.h>
#include <p101_env/env.h>
#include <p101_error/error.h>
#include <stddef.h>
#include <stdlib.h>

static const char *const wrapper_functions[] = {"p101_pthread_attr_destroy",
                                                "p101_pthread_attr_getdetachstate",
                                                "p101_pthread_attr_getguardsize",
                                                "p101_pthread_attr_getinheritsched",
                                                "p101_pthread_attr_getschedparam",
                                                "p101_pthread_attr_getschedpolicy",
                                                "p101_pthread_attr_getscope",
                                                "p101_pthread_attr_getstack",
                                                "p101_pthread_attr_getstacksize",
                                                "p101_pthread_attr_init",
                                                "p101_pthread_attr_setdetachstate",
                                                "p101_pthread_attr_setguardsize",
                                                "p101_pthread_attr_setinheritsched",
                                                "p101_pthread_attr_setschedparam",
                                                "p101_pthread_attr_setschedpolicy",
                                                "p101_pthread_attr_setscope",
                                                "p101_pthread_attr_setstack",
                                                "p101_pthread_attr_setstacksize",
                                                "p101_pthread_atfork",
                                                "p101_pthread_create",
                                                "p101_pthread_detach",
                                                "p101_pthread_equal",
                                                "p101_pthread_exit",
                                                "p101_pthread_getschedparam",
                                                "p101_pthread_join",
                                                "p101_pthread_kill",
                                                "p101_pthread_self",
                                                "p101_pthread_setschedparam",
                                                "p101_pthread_sigmask",
                                                "p101_sem_close",
                                                "p101_sem_open",
                                                "p101_sem_post",
                                                "p101_sem_trywait",
                                                "p101_sem_unlink",
                                                "p101_sem_wait"};

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

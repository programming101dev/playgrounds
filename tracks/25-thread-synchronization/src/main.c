#include "track_info.h"
#include <p101_c/p101_stdio.h>
#include <p101_c/p101_stdlib.h>
#include <p101_env/env.h>
#include <p101_error/error.h>
#include <stddef.h>
#include <stdlib.h>

static const char *const wrapper_functions[] = {
    "p101_pthread_cond_broadcast",
    "p101_pthread_cond_destroy",
    "p101_pthread_cond_init",
    "p101_pthread_cond_signal",
    "p101_pthread_cond_timedwait",
    "p101_pthread_cond_wait",
    "p101_pthread_condattr_destroy",
    "p101_pthread_condattr_getpshared",
    "p101_pthread_condattr_init",
    "p101_pthread_condattr_setpshared",
    "p101_pthread_mutex_destroy",
    "p101_pthread_mutex_getprioceiling",
    "p101_pthread_mutex_init",
    "p101_pthread_mutex_lock",
    "p101_pthread_mutex_setprioceiling",
    "p101_pthread_mutex_trylock",
    "p101_pthread_mutex_unlock",
    "p101_pthread_mutexattr_destroy",
    "p101_pthread_mutexattr_getprioceiling",
    "p101_pthread_mutexattr_getprotocol",
    "p101_pthread_mutexattr_getpshared",
    "p101_pthread_mutexattr_gettype",
    "p101_pthread_mutexattr_init",
    "p101_pthread_mutexattr_setprioceiling",
    "p101_pthread_mutexattr_setprotocol",
    "p101_pthread_mutexattr_setpshared",
    "p101_pthread_mutexattr_settype",
    "p101_pthread_rwlock_destroy",
    "p101_pthread_rwlock_init",
    "p101_pthread_rwlock_rdlock",
    "p101_pthread_rwlock_tryrdlock",
    "p101_pthread_rwlock_trywrlock",
    "p101_pthread_rwlock_unlock",
    "p101_pthread_rwlock_wrlock",
    "p101_pthread_rwlockattr_destroy",
    "p101_pthread_rwlockattr_getpshared",
    "p101_pthread_rwlockattr_init",
    "p101_pthread_rwlockattr_setpshared"
};

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

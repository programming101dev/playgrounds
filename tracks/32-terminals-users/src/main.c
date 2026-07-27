#include "track_info.h"
#include <p101_c/p101_stdio.h>
#include <p101_c/p101_stdlib.h>
#include <p101_env/env.h>
#include <p101_error/error.h>
#include <stddef.h>
#include <stdlib.h>

static const char *const wrapper_functions[] = {
    "p101_endusershell",
    "p101_getegid",
    "p101_geteuid",
    "p101_getgid",
    "p101_getgroups",
    "p101_getlogin_r",
    "p101_getuid",
    "p101_getusershell",
    "p101_setegid",
    "p101_seteuid",
    "p101_setgid",
    "p101_setregid",
    "p101_setreuid",
    "p101_setuid",
    "p101_setusershell",
    "p101_cfgetispeed",
    "p101_cfgetospeed",
    "p101_cfmakeraw",
    "p101_cfsetispeed",
    "p101_cfsetospeed",
    "p101_cfsetspeed",
    "p101_endttyent",
    "p101_endutxent",
    "p101_getgrgid_r",
    "p101_getgrnam_r",
    "p101_getpwnam_r",
    "p101_getpwuid_r",
    "p101_getttyent",
    "p101_getttynam",
    "p101_getutxent",
    "p101_getutxid",
    "p101_getutxline",
    "p101_isatty",
    "p101_pututxline",
    "p101_setttyent",
    "p101_setutxent",
    "p101_tcdrain",
    "p101_tcflow",
    "p101_tcflush",
    "p101_tcgetattr",
    "p101_tcgetpgrp",
    "p101_tcgetsid",
    "p101_tcsendbreak",
    "p101_tcsetattr",
    "p101_tcsetpgrp",
    "p101_ttyname_r"
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

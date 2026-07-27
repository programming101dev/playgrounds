#include "track_info.h"
#include <p101_c/p101_stdio.h>
#include <p101_c/p101_stdlib.h>
#include <p101_env/env.h>
#include <p101_error/error.h>
#include <stddef.h>
#include <stdlib.h>

static const char *const wrapper_functions[] = {
    "p101_b64_ntop",
    "p101_b64_pton",
    "p101_dn_comp",
    "p101_dn_expand",
    "p101_dn_skipname",
    "p101_ns_get16",
    "p101_ns_get32",
    "p101_ns_initparse",
    "p101_ns_msg_getflag",
    "p101_ns_name_compress",
    "p101_ns_name_ntol",
    "p101_ns_name_ntop",
    "p101_ns_name_pack",
    "p101_ns_name_pton",
    "p101_ns_name_rollback",
    "p101_ns_name_skip",
    "p101_ns_name_uncompress",
    "p101_ns_name_unpack",
    "p101_ns_parserr",
    "p101_ns_put16",
    "p101_ns_put32",
    "p101_ns_skiprr",
    "p101_res_dnok",
    "p101_res_hnok",
    "p101_res_mailok",
    "p101_res_nclose",
    "p101_res_ninit",
    "p101_res_nmkquery",
    "p101_res_nquery",
    "p101_res_nquerydomain",
    "p101_res_nsearch",
    "p101_res_nsend",
    "p101_res_ownok"
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

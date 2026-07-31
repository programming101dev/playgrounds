#include "track_info.h"
#include <inttypes.h>
#include <p101_c/p101_stdio.h>
#include <p101_c/p101_stdlib.h>
#include <p101_env/env.h>
#include <p101_error/error.h>
#include <p101_util/endian.h>
#include <stddef.h>
#include <stdlib.h>

static const char *const wrapper_functions[] = {"p101_be16toh",          "p101_be32toh",           "p101_be64toh", "p101_bswap16", "p101_bswap32",          "p101_bswap64", "p101_htobe16", "p101_htobe32",
                                                "p101_htobe64",          "p101_htole16",           "p101_htole32", "p101_htole64", "p101_is_little_endian", "p101_le16toh", "p101_le32toh", "p101_le64toh",
                                                "p101_tool_run_capture", "p101_tool_run_redirect", "p101_memchr",  "p101_memcmp",  "p101_memcpy",           "p101_memmove", "p101_memset",  "p101_memccpy"};

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

static int write_endian_demo(const struct p101_env *env, struct p101_error *err)
{
    const uint32_t host_value = UINT32_C(0x12345678);
    uint32_t       wire_value;
    uint32_t       round_trip;

    wire_value = p101_htobe32(env, host_value);
    round_trip = p101_be32toh(env, wire_value);
    if(p101_fprintf(env, err, stdout, "Endian round trip: 0x%08" PRIx32 " -> 0x%08" PRIx32 " -> 0x%08" PRIx32 "\n", host_value, wire_value, round_trip) < 0 || p101_error_has_error(err))
    {
        return EXIT_FAILURE;
    }

    return (round_trip == host_value) ? EXIT_SUCCESS : EXIT_FAILURE;
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

    if(write_endian_demo(env, err) != EXIT_SUCCESS)
    {
        goto done;
    }

    ret_val = EXIT_SUCCESS;

done:
    p101_env_destroy(env);
    p101_error_destroy(err);
    return ret_val;
}

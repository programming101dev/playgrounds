#include "track_info.h"
#include <p101_c/p101_stdio.h>
#include <p101_c/p101_stdlib.h>
#include <p101_env/env.h>
#include <p101_error/error.h>
#include <stddef.h>
#include <stdlib.h>

static const char *const wrapper_functions[] = {
    "p101_strchr",       "p101_strcmp",      "p101_strcoll",    "p101_strcspn",     "p101_strerror",   "p101_strlen",      "p101_strncat",     "p101_strncmp",         "p101_strncpy",    "p101_strpbrk",    "p101_strrchr",    "p101_strspn",
    "p101_strstr",       "p101_strxfrm",     "p101_a64l",       "p101_ffs",         "p101_isalnum_l",  "p101_isalpha_l",   "p101_isblank_l",   "p101_iscntrl_l",       "p101_isdigit_l",  "p101_isgraph_l",  "p101_islower_l",  "p101_isprint_l",
    "p101_ispunct_l",    "p101_isspace_l",   "p101_isupper_l",  "p101_iswalnum_l",  "p101_iswalpha_l", "p101_iswblank_l",  "p101_iswcntrl_l",  "p101_iswctype_l",      "p101_iswdigit_l", "p101_iswgraph_l", "p101_iswlower_l", "p101_iswprint_l",
    "p101_iswpunct_l",   "p101_iswspace_l",  "p101_iswupper_l", "p101_iswxdigit_l", "p101_isxdigit_l", "p101_l64a",        "p101_mbsnrtowcs",  "p101_open_wmemstream", "p101_rpmatch",    "p101_stpcpy",     "p101_stpncpy",    "p101_strcasecmp",
    "p101_strcasecmp_l", "p101_strcasestr",  "p101_strchrnul",  "p101_strcoll_l",   "p101_strdup",     "p101_strerror_r",  "p101_strncasecmp", "p101_strncasecmp_l",   "p101_strndup",    "p101_strnlen",    "p101_strsep",     "p101_strsignal",
    "p101_strtok_r",     "p101_strxfrm_l",   "p101_swab",       "p101_tolower_l",   "p101_toupper_l",  "p101_towctrans_l", "p101_towlower_l",  "p101_towupper_l",      "p101_wcpcpy",     "p101_wcpncpy",    "p101_wcscasecmp", "p101_wcscoll_l",
    "p101_wcsdup",       "p101_wcsncasecmp", "p101_wcsnlen",    "p101_wcsnrtombs",  "p101_wcswidth",   "p101_wcsxfrm_l",   "p101_wctrans_l",   "p101_wctype_l",        "p101_wcwidth",    "p101_wordexp",    "p101_wordfree"};

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

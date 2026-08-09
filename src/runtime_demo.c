#include "constants.h"
#include "errors.h"
#include "lesson_demo.h"
#include "playground_support.h"
#include <fcntl.h>
#include <limits.h>
#include <p101_c/p101_stdio.h>
#include <p101_c/p101_stdlib.h>
#include <p101_c/p101_string.h>
#include <p101_cli/p101_stdlib.h>
#include <p101_cli/p101_unistd.h>
#include <p101_filesystem/p101_dirent.h>
#include <p101_filesystem/p101_fnmatch.h>
#include <p101_filesystem/p101_ftw.h>
#include <p101_filesystem/p101_glob.h>
#include <p101_filesystem/p101_libgen.h>
#include <p101_filesystem/p101_stdio.h>
#include <p101_filesystem/p101_stdlib.h>
#include <p101_filesystem/p101_unistd.h>
#include <p101_filesystem/sys/p101_stat.h>
#include <p101_filesystem/sys/p101_statvfs.h>
#include <p101_host/p101_stdlib.h>
#include <p101_host/p101_unistd.h>
#include <p101_host/sys/p101_utsname.h>
#include <p101_io/p101_aio.h>
#include <p101_io/p101_fcntl.h>
#include <p101_io/p101_poll.h>
#include <p101_io/p101_stdio.h>
#include <p101_io/p101_unistd.h>
#include <p101_io/sys/p101_select.h>
#include <p101_io/sys/p101_uio.h>
#include <p101_ipc/p101_unistd.h>
#include <p101_ipc/sys/p101_ipc.h>
#include <p101_ipc/sys/p101_mman.h>
#include <p101_ipc/sys/p101_msg.h>
#include <p101_ipc/sys/p101_sem.h>
#include <p101_ipc/sys/p101_shm.h>
#include <p101_ipc/sys/p101_stat.h>
#include <p101_memory/p101_stdlib.h>
#include <p101_memory/sys/p101_mman.h>
#include <p101_process/p101_sched.h>
#include <p101_process/p101_setjmp.h>
#include <p101_process/p101_signal.h>
#include <p101_process/p101_spawn.h>
#include <p101_process/p101_stdio.h>
#include <p101_process/p101_stdlib.h>
#include <p101_process/p101_unistd.h>
#include <p101_process/sys/p101_resource.h>
#include <p101_process/sys/p101_times.h>
#include <p101_process/sys/p101_wait.h>
#include <p101_random/p101_stdlib.h>
#include <p101_terminal/p101_stdlib.h>
#include <p101_terminal/p101_termios.h>
#include <p101_terminal/p101_unistd.h>
#include <p101_text/p101_ctype.h>
#include <p101_text/p101_regex.h>
#include <p101_text/p101_stdlib.h>
#include <p101_text/p101_string.h>
#include <p101_text/p101_strings.h>
#include <p101_text/p101_unistd.h>
#include <p101_text/p101_wchar.h>
#include <p101_text/p101_wctype.h>
#include <p101_text/p101_wordexp.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>

#ifndef PATH_MAX
    #define PATH_MAX 4096
#endif

enum c_memory_runtime_demo_constants
{
    ORIENTATION_MESSAGE_BYTES       = 64,
    C_MEMORY_INITIAL_BYTES          = 32,
    C_MEMORY_ZEROED_ITEMS           = 4,
    C_MEMORY_ZEROED_ITEM_BYTES      = 8,
    C_MEMORY_GROWN_BYTES            = 64,
    C_MEMORY_VALUE_COUNT            = 5,
    C_MEMORY_SORT_VALUE_LOW         = 1,
    C_MEMORY_SORT_VALUE_MID_LOW     = 2,
    C_MEMORY_SEARCH_KEY             = 3,
    C_MEMORY_SORT_VALUE_MID_HIGH    = 4,
    C_MEMORY_SORT_VALUE_HIGH        = 5,
    C_MEMORY_ABS_VALUE              = 7,
    C_MEMORY_DIV_NUMERATOR          = 17,
    C_MEMORY_DIV_DENOMINATOR        = 5,
    C_MEMORY_EXPECTED_QUOTIENT      = 3,
    C_MEMORY_EXPECTED_REMAINDER     = 2,
    C_MEMORY_WIDE_TEXT_CAPACITY     = 16,
    C_MEMORY_PARSE_BASE             = 10,
    C_MEMORY_RANDOM_STATE_BYTES     = 256,
    C_MEMORY_RANDOM_BYTES           = 16,
    C_MEMORY_PUTENV_BYTES           = 32,
    C_MEMORY_ALIGNMENT_MULTIPLIER   = 2,
    C_MEMORY_ALIGNED_BYTES          = 64,
    C_MEMORY_L64A_VALUE             = 1234,
    C_MEMORY_ARC4RANDOM_UPPER_BOUND = 10,
    C_MEMORY_LOADAVG_COUNT          = 3
};

static int  compare_ints(const void *lhs, const void *rhs);
static void c_memory_runtime_atexit_hook(void);

int run_orientation_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args)
{
    bool  p101_bool_result_1;
    int   p101_int_result_1;
    void *p101_void_pointer_result_1;

    static const char output_text[] = "p101-orientation: env err wrappers tools\n"
                                      "env: created once in main and passed to every p101-aware function\n"
                                      "err: checked after fallible wrappers and preserved through cleanup\n"
                                      "wrappers: p101_malloc p101_memset p101_snprintf p101_open p101_write p101_close p101_free\n"
                                      "tools: audit inspect test model resource trace report\n";
    char             *message;
    int               ret_val;

    P101_TRACE_SCOPE(env);
    message = NULL;
    ret_val = EXIT_FAILURE;

    p101_printf(env, err, "orientation: env + err + wrappers + tools\n");
    p101_bool_result_1 = p101_error_has_error(err);
    if(p101_bool_result_1)
    {
        goto done;
    }

    p101_void_pointer_result_1 = p101_malloc(env, err, ORIENTATION_MESSAGE_BYTES);
    message                    = (char *)p101_void_pointer_result_1;
    p101_bool_result_1         = p101_error_has_error(err);
    if(message == NULL || p101_bool_result_1)
    {
        goto done;
    }
    p101_memset(env, message, 0, ORIENTATION_MESSAGE_BYTES);
    p101_snprintf(env, err, message, ORIENTATION_MESSAGE_BYTES, "%s", "p101 orientation");
    p101_bool_result_1 = p101_error_has_error(err);
    if(p101_bool_result_1)
    {
        goto done;
    }

    p101_int_result_1 = p101_tool_playground_write_text_output(env, err, args, output_text);
    if(p101_int_result_1 != EXIT_SUCCESS)
    {
        goto done;
    }

    ret_val = EXIT_SUCCESS;

done:
    p101_free(env, message);
    return ret_val;
}

int run_clean_file(const struct p101_env *env, struct p101_error *err, const struct arguments *args)
{
    int p101_int_result_1;

    P101_TRACE_SCOPE(env);
    p101_printf(env, err, "clean-file: write/close/free all resources\n");
    p101_int_result_1 = p101_tool_playground_support_write_demo_file(env, err, args, "clean-file", false, false);
    return p101_int_result_1;
}

int run_c_memory_runtime_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args)
{
    bool               p101_bool_result_1;
    char              *p101_char_pointer_result_1;
    const void        *p101_const_void_pointer_result_1;
    double             p101_double_result_1;
    float              p101_float_result_1;
    int                p101_int_result_1;
    int                p101_int_result_2;
    long double        p101_long_double_result_1;
    long long          p101_long_long_result_1;
    long               p101_long_result_1;
    size_t             p101_size_result_1;
    uint32_t           p101_uint32_result_1;
    unsigned long long p101_unsigned_long_long_result_1;
    unsigned long      p101_unsigned_long_result_1;
    unsigned short    *p101_unsigned_short_pointer_result_1;
    void              *p101_void_pointer_result_1;

    char          *buffer;
    char          *grown;
    char          *zeroed;
    char          *putenv_value;
    char          *option_cursor;
    char          *option_value;
    char          *previous_state;
    const char    *active_state;
    void          *aligned;
    void          *c_aligned;
    const int      key = C_MEMORY_SEARCH_KEY;
    const int     *found;
    char           mb_output[MB_LEN_MAX];
    char           option_text[]        = "alpha=value";
    char           option_key_alpha[]   = "alpha";
    char           option_key_beta[]    = "beta";
    char *const    option_keys[]        = {option_key_alpha, option_key_beta, NULL};
    char           temp_file_template[] = "/tmp/p101-memory-runtime-file-XXXXXX";
    char           temp_dir_template[]  = "/tmp/p101-memory-runtime-dir-XXXXXX";
    char           resolved_path[PATH_MAX];
    char           random_state[C_MEMORY_RANDOM_STATE_BYTES];
    double         loadavg[C_MEMORY_LOADAVG_COUNT];
    int            values[] = {C_MEMORY_SORT_VALUE_MID_HIGH, C_MEMORY_SORT_VALUE_MID_LOW, C_MEMORY_SORT_VALUE_HIGH, C_MEMORY_SORT_VALUE_LOW, C_MEMORY_SEARCH_KEY};
    int            temp_fd;
    int            pty_fd;
    int            ret_val;
    wchar_t        wide_text[C_MEMORY_WIDE_TEXT_CAPACITY];
    unsigned char  random_bytes[C_MEMORY_RANDOM_BYTES];
    unsigned short seed_values[C_MEMORY_LOADAVG_COUNT] = {1, 2, C_MEMORY_SEARCH_KEY};
    div_t          div_result;
    ldiv_t         ldiv_result;
    lldiv_t        lldiv_result;

    P101_TRACE_SCOPE(env);
    (void)args;
    buffer         = NULL;
    grown          = NULL;
    zeroed         = NULL;
    putenv_value   = NULL;
    previous_state = NULL;
    active_state   = NULL;
    aligned        = NULL;
    c_aligned      = NULL;
    option_cursor  = option_text;
    option_value   = NULL;
    temp_fd        = -1;
    pty_fd         = -1;
    ret_val        = EXIT_FAILURE;

    p101_void_pointer_result_1 = p101_malloc(env, err, C_MEMORY_INITIAL_BYTES);
    buffer                     = (char *)p101_void_pointer_result_1;
    p101_bool_result_1         = p101_error_has_error(err);
    if(buffer == NULL || p101_bool_result_1)
    {
        goto done;
    }
    p101_memset(env, buffer, 'm', C_MEMORY_INITIAL_BYTES);

    p101_void_pointer_result_1 = p101_calloc(env, err, C_MEMORY_ZEROED_ITEMS, C_MEMORY_ZEROED_ITEM_BYTES);
    zeroed                     = (char *)p101_void_pointer_result_1;
    p101_bool_result_1         = p101_error_has_error(err);
    if(zeroed == NULL || p101_bool_result_1)
    {
        goto done;
    }

    p101_void_pointer_result_1 = p101_realloc(env, err, buffer, C_MEMORY_GROWN_BYTES);
    grown                      = (char *)p101_void_pointer_result_1;
    p101_bool_result_1         = p101_error_has_error(err);
    if(grown == NULL || p101_bool_result_1)
    {
        goto done;
    }
    buffer = grown;
    grown  = NULL;

    p101_qsort(env, values, C_MEMORY_VALUE_COUNT, sizeof(values[0]), compare_ints);
    p101_const_void_pointer_result_1 = p101_bsearch(env, &key, values, C_MEMORY_VALUE_COUNT, sizeof(values[0]), compare_ints);
    found                            = (const int *)p101_const_void_pointer_result_1;
    if(found == NULL || *found != key)
    {
        P101_ERROR_RAISE_USER(err, "The qsort/bsearch smoke check failed.", ERR_SCENARIO_FAILURE);
        goto done;
    }

    p101_int_result_1  = p101_abs(env, err, -C_MEMORY_ABS_VALUE);
    p101_bool_result_1 = p101_error_has_error(err);
    if(p101_int_result_1 != C_MEMORY_ABS_VALUE || p101_bool_result_1)
    {
        goto done;
    }
    p101_long_result_1 = p101_labs(env, err, -C_MEMORY_ABS_VALUE);
    p101_bool_result_1 = p101_error_has_error(err);
    if(p101_long_result_1 != C_MEMORY_ABS_VALUE || p101_bool_result_1)
    {
        goto done;
    }
    p101_long_long_result_1 = p101_llabs(env, err, -C_MEMORY_ABS_VALUE);
    p101_bool_result_1      = p101_error_has_error(err);
    if(p101_long_long_result_1 != C_MEMORY_ABS_VALUE || p101_bool_result_1)
    {
        goto done;
    }
    /*
     * These wrappers intentionally mirror the standard div-family API, whose
     * result type is an aggregate. Keep the suppression local so the warning
     * still protects the rest of the playground code.
     */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Waggregate-return"
    div_result   = p101_div(env, err, C_MEMORY_DIV_NUMERATOR, C_MEMORY_DIV_DENOMINATOR);
    ldiv_result  = p101_ldiv(env, err, C_MEMORY_DIV_NUMERATOR, C_MEMORY_DIV_DENOMINATOR);
    lldiv_result = p101_lldiv(env, err, C_MEMORY_DIV_NUMERATOR, C_MEMORY_DIV_DENOMINATOR);
#pragma GCC diagnostic pop
    if(div_result.quot != C_MEMORY_EXPECTED_QUOTIENT || ldiv_result.rem != C_MEMORY_EXPECTED_REMAINDER || lldiv_result.quot != C_MEMORY_EXPECTED_QUOTIENT)
    {
        P101_ERROR_RAISE_USER(err, "The div/ldiv/lldiv smoke check failed.", ERR_SCENARIO_FAILURE);
        goto done;
    }

    p101_int_result_1  = p101_mblen(env, err, "A", 1);
    p101_bool_result_1 = p101_error_has_error(err);
    if(p101_int_result_1 != 1 || p101_bool_result_1)
    {
        goto done;
    }
    p101_int_result_1  = p101_mbtowc(env, err, wide_text, "A", 1);
    p101_bool_result_1 = p101_error_has_error(err);
    if(p101_int_result_1 != 1 || p101_bool_result_1)
    {
        goto done;
    }
    p101_size_result_1 = p101_mbstowcs(env, err, wide_text, "ABC", C_MEMORY_WIDE_TEXT_CAPACITY);
    p101_bool_result_1 = p101_error_has_error(err);
    if(p101_size_result_1 == (size_t)-1 || p101_bool_result_1)
    {
        goto done;
    }
    p101_size_result_1 = p101_wcstombs(env, err, mb_output, L"Z", sizeof(mb_output));
    p101_bool_result_1 = p101_error_has_error(err);
    if(p101_size_result_1 == (size_t)-1 || p101_bool_result_1)
    {
        goto done;
    }
    p101_int_result_1  = p101_wctomb(env, err, mb_output, L'Q');
    p101_bool_result_1 = p101_error_has_error(err);
    if(p101_int_result_1 <= 0 || p101_bool_result_1)
    {
        goto done;
    }

    p101_double_result_1 = p101_strtod(env, err, "1.25", NULL);
    (void)p101_double_result_1;
    p101_bool_result_1 = p101_error_has_error(err);
    if(p101_bool_result_1)
    {
        goto done;
    }
    p101_float_result_1 = p101_strtof(env, err, "2.5", NULL);
    (void)p101_float_result_1;
    p101_bool_result_1 = p101_error_has_error(err);
    if(p101_bool_result_1)
    {
        goto done;
    }
    p101_long_double_result_1 = p101_strtold(env, err, "3.75", NULL);
    (void)p101_long_double_result_1;
    p101_bool_result_1 = p101_error_has_error(err);
    if(p101_bool_result_1)
    {
        goto done;
    }
    p101_long_result_1 = p101_strtol(env, err, "42", NULL, C_MEMORY_PARSE_BASE);
    (void)p101_long_result_1;
    p101_bool_result_1 = p101_error_has_error(err);
    if(p101_bool_result_1)
    {
        goto done;
    }
    p101_long_long_result_1 = p101_strtoll(env, err, "420", NULL, C_MEMORY_PARSE_BASE);
    (void)p101_long_long_result_1;
    p101_bool_result_1 = p101_error_has_error(err);
    if(p101_bool_result_1)
    {
        goto done;
    }
    p101_unsigned_long_result_1 = p101_strtoul(env, err, "42", NULL, C_MEMORY_PARSE_BASE);
    (void)p101_unsigned_long_result_1;
    p101_bool_result_1 = p101_error_has_error(err);
    if(p101_bool_result_1)
    {
        goto done;
    }
    p101_unsigned_long_long_result_1 = p101_strtoull(env, err, "420", NULL, C_MEMORY_PARSE_BASE);
    (void)p101_unsigned_long_long_result_1;
    p101_bool_result_1 = p101_error_has_error(err);
    if(p101_bool_result_1)
    {
        goto done;
    }

    p101_setenv(env, err, "P101_TRACK_MEMORY_RUNTIME", "setenv", 1);
    p101_bool_result_1 = p101_error_has_error(err);
    if(p101_bool_result_1)
    {
        goto done;
    }
    p101_char_pointer_result_1 = p101_getenv(env, err, "P101_TRACK_MEMORY_RUNTIME");
    if(p101_char_pointer_result_1 == NULL)
    {
        P101_ERROR_RAISE_USER(err, "The setenv/getenv smoke check failed.", ERR_SCENARIO_FAILURE);
        goto done;
    }
    p101_unsetenv(env, err, "P101_TRACK_MEMORY_RUNTIME");
    p101_bool_result_1 = p101_error_has_error(err);
    if(p101_bool_result_1)
    {
        goto done;
    }

    p101_void_pointer_result_1 = p101_malloc(env, err, C_MEMORY_PUTENV_BYTES);
    putenv_value               = (char *)p101_void_pointer_result_1;
    p101_bool_result_1         = p101_error_has_error(err);
    if(putenv_value == NULL || p101_bool_result_1)
    {
        goto done;
    }
    p101_snprintf(env, err, putenv_value, C_MEMORY_PUTENV_BYTES, "%s", "P101_TRACK_PUTENV=1");
    p101_bool_result_1 = p101_error_has_error(err);
    if(p101_bool_result_1)
    {
        goto done;
    }
    p101_putenv(env, err, putenv_value);
    p101_bool_result_1 = p101_error_has_error(err);
    if(p101_bool_result_1)
    {
        goto done;
    }
    p101_char_pointer_result_1 = p101_getenv(env, err, "P101_TRACK_PUTENV");
    if(p101_char_pointer_result_1 == NULL)
    {
        P101_ERROR_RAISE_USER(err, "The putenv/getenv smoke check failed.", ERR_SCENARIO_FAILURE);
        goto done;
    }
    p101_unsetenv(env, err, "P101_TRACK_PUTENV");
    p101_bool_result_1 = p101_error_has_error(err);
    if(p101_bool_result_1)
    {
        goto done;
    }

    p101_int_result_1 = p101_getsubopt(env, &option_cursor, option_keys, &option_value);
    if(p101_int_result_1 != 0 || option_value == NULL)
    {
        goto done;
    }

    temp_fd            = p101_mkstemp(env, err, temp_file_template);
    p101_bool_result_1 = p101_error_has_error(err);
    if(temp_fd == -1 || p101_bool_result_1)
    {
        goto done;
    }
    p101_tool_playground_support_close_fd_preserving_error(env, err, &temp_fd);
    p101_bool_result_1 = p101_error_has_error(err);
    if(p101_bool_result_1)
    {
        goto done;
    }
    p101_remove(env, err, temp_file_template);
    p101_bool_result_1 = p101_error_has_error(err);
    if(p101_bool_result_1)
    {
        goto done;
    }

    p101_char_pointer_result_1 = p101_mkdtemp(env, err, temp_dir_template);
    p101_bool_result_1         = p101_error_has_error(err);
    if(p101_char_pointer_result_1 == NULL || p101_bool_result_1)
    {
        goto done;
    }
    p101_rmdir(env, err, temp_dir_template);
    p101_bool_result_1 = p101_error_has_error(err);
    if(p101_bool_result_1)
    {
        goto done;
    }

    p101_char_pointer_result_1 = p101_realpath(env, err, ".", resolved_path);
    p101_bool_result_1         = p101_error_has_error(err);
    if(p101_char_pointer_result_1 == NULL || p101_bool_result_1)
    {
        goto done;
    }

    p101_int_result_1  = p101_posix_memalign(env, err, &aligned, sizeof(void *) * C_MEMORY_ALIGNMENT_MULTIPLIER, C_MEMORY_ALIGNED_BYTES);
    p101_bool_result_1 = p101_error_has_error(err);
    if(p101_int_result_1 != 0 || aligned == NULL || p101_bool_result_1)
    {
        goto done;
    }
    c_aligned          = p101_aligned_alloc(env, err, sizeof(void *) * C_MEMORY_ALIGNMENT_MULTIPLIER, C_MEMORY_ALIGNED_BYTES);
    p101_bool_result_1 = p101_error_has_error(err);
    if(c_aligned == NULL || p101_bool_result_1)
    {
        goto done;
    }

    p101_long_result_1 = p101_a64l(env, "Az");
    (void)p101_long_result_1;
    p101_char_pointer_result_1 = p101_l64a(env, C_MEMORY_L64A_VALUE);
    (void)p101_char_pointer_result_1;
    previous_state     = p101_initstate(env, err, 1U, random_state, sizeof(random_state));
    p101_bool_result_1 = p101_error_has_error(err);
    if(previous_state == NULL || p101_bool_result_1)
    {
        goto done;
    }
    active_state       = p101_setstate(env, err, random_state);
    p101_bool_result_1 = p101_error_has_error(err);
    if(active_state == NULL || p101_bool_result_1)
    {
        goto done;
    }
    p101_char_pointer_result_1 = p101_setstate(env, err, previous_state);
    (void)p101_char_pointer_result_1;
    p101_srandom(env, 1U);
    p101_srand48(env, 1L);
    p101_unsigned_short_pointer_result_1 = p101_seed48(env, seed_values);
    (void)p101_unsigned_short_pointer_result_1;

    p101_uint32_result_1 = p101_arc4random(env);
    (void)p101_uint32_result_1;
    p101_arc4random_buf(env, random_bytes, sizeof(random_bytes));
    p101_uint32_result_1 = p101_arc4random_uniform(env, C_MEMORY_ARC4RANDOM_UPPER_BOUND);
    (void)p101_uint32_result_1;
    p101_int_result_1 = p101_getloadavg(env, err, loadavg, C_MEMORY_LOADAVG_COUNT);
    (void)p101_int_result_1;
    p101_bool_result_1 = p101_error_has_error(err);
    if(p101_bool_result_1)
    {
        goto done;
    }
    p101_int_result_1 = p101_rpmatch(env, "yes");
    (void)p101_int_result_1;

    /*
     * Some systems can deny pseudo-terminal allocation in constrained
     * environments. The wrapper still gets exercised; grantpt/unlockpt/ptsname
     * are exercised only when a pty was actually acquired.
     */
    pty_fd             = p101_posix_openpt(env, err, O_RDWR | O_NOCTTY);
    p101_bool_result_1 = p101_error_has_no_error(err);
    if(pty_fd != -1 && p101_bool_result_1)
    {
        p101_grantpt(env, err, pty_fd);
        p101_unlockpt(env, err, pty_fd);
        p101_char_pointer_result_1 = p101_ptsname(env, err, pty_fd);
        (void)p101_char_pointer_result_1;
        p101_bool_result_1 = p101_error_has_error(err);
        if(p101_bool_result_1)
        {
            goto done;
        }
    }
    else
    {
        p101_error_reset(err);
    }
    if(pty_fd != -1)
    {
        p101_tool_playground_support_close_fd_preserving_error(env, err, &pty_fd);
        p101_bool_result_1 = p101_error_has_error(err);
        if(p101_bool_result_1)
        {
            goto done;
        }
    }

    p101_int_result_1 = p101_atexit(env, err, c_memory_runtime_atexit_hook);
    p101_int_result_2 = 0;
    if(p101_int_result_1 == 0)
    {
        p101_int_result_2 = p101_at_quick_exit(env, err, c_memory_runtime_atexit_hook);
    }
    p101_bool_result_1 = p101_error_has_error(err);
    if(p101_int_result_1 != 0 || p101_int_result_2 != 0 || p101_bool_result_1)
    {
        goto done;
    }

    p101_int_result_1  = p101_system(env, err, ":");
    p101_bool_result_1 = p101_error_has_error(err);
    if(p101_int_result_1 == -1 || p101_bool_result_1)
    {
        goto done;
    }

    p101_printf(env, err, "c-memory-runtime: completed declared wrapper smoke path\n");
    p101_bool_result_1 = p101_error_has_error(err);
    if(p101_bool_result_1)
    {
        goto done;
    }
    p101_int_result_1 = p101_tool_playground_write_text_output(env, err, args, "c-memory-runtime: completed declared wrapper smoke path\n");
    if(p101_int_result_1 != EXIT_SUCCESS)
    {
        goto done;
    }
    p101_bool_result_1 = p101_error_has_no_error(err);
    if(p101_bool_result_1)
    {
        ret_val = EXIT_SUCCESS;
    }

done:
    p101_tool_playground_support_close_fd_preserving_error(env, err, &pty_fd);
    p101_tool_playground_support_close_fd_preserving_error(env, err, &temp_fd);
    p101_free(env, c_aligned);
    p101_free(env, aligned);
    p101_free(env, putenv_value);
    p101_free(env, zeroed);
    p101_free(env, buffer);
    return ret_val;
}

static int compare_ints(const void *lhs, const void *rhs)
{
    const int left  = *(const int *)lhs;
    const int right = *(const int *)rhs;

    return (left > right) - (left < right);
}

static void c_memory_runtime_atexit_hook(void)
{
}

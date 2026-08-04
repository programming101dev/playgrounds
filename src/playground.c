#include "playground.h"
#include "constants.h"
#include "errors.h"
#include "scenario.h"
#include <fcntl.h>
#include <limits.h>
#include <p101_c/p101_stdio.h>
#include <p101_c/p101_stdlib.h>
#include <p101_c/p101_string.h>
#include <p101_cli/cli.h>
#include <p101_filesystem/filesystem.h>
#include <p101_host/host.h>
#include <p101_io/io.h>
#include <p101_ipc/ipc.h>
#include <p101_memory/memory.h>
#include <p101_process/process.h>
#include <p101_random/random.h>
#include <p101_terminal/terminal.h>
#include <p101_text/text.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
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

static int   run_orientation_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args);
static int   run_clean_file(const struct p101_env *env, struct p101_error *err, const struct arguments *args);
static int   run_c_memory_runtime_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args);
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
static int   run_unsafe_log_secret_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args);
static int   run_log_injection_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args);
static int   run_missing_structured_log_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args);
static int   run_input_validation_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args);
static int   run_command_injection_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args);
static int   run_predictable_temp_file_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args);
static int   run_signed_conversion_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args);
static int   run_truncation_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args);
static int   run_use_after_free_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args);
static int   run_realloc_failure_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args);
static int   run_out_of_bounds_write_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args);
static int   run_out_of_bounds_read_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args);
static int   run_buffer_overflow_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args);
static int   run_uninitialized_read_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args);
static int   run_null_dereference_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args);
static int   run_integer_overflow_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args);
static int   run_path_traversal_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args);
static int   run_format_string_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args);
static int   run_stale_secret_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args);
static int   run_resource_exhaustion_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args);
static int   run_toctou_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args);
static int   run_data_race_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args);
static int   run_string_not_terminated_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args);
static int   run_partial_write_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args);
static int   run_interrupted_syscall_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args);
static int   run_unsafe_file_mode_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args);
static int   run_symlink_follow_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args);
static int   run_trusted_environment_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args);
static int   run_unchecked_parse_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args);
static int   run_missing_authorization_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args);
static int   run_cleanup_order_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args);
static int   run_thread_argument_lifetime_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args);
static int   run_short_read_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args);
static int   run_read_eof_handling_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args);
static int   run_parser_fuzz_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args);
static void  close_fd_preserving_error(const struct p101_env *env, struct p101_error *err, int *fd);
static int   compare_ints(const void *lhs, const void *rhs);
static void  c_memory_runtime_atexit_hook(void);
static int   write_demo_file(const struct p101_env *env, struct p101_error *err, const struct arguments *args, const char *label, bool leak_fd, bool leak_alloc);
static int   write_text_output(const struct p101_env *env, struct p101_error *err, const struct arguments *args, const char *text);
static char *make_buffer(const struct p101_env *env, struct p101_error *err, size_t bytes, char fill);

int p101_tool_playground_run(const struct p101_env *env, struct p101_error *err, const struct arguments *args)
{
    int ret_val;

    P101_TRACE_SCOPE(env);
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
        case SCENARIO_ORIENTATION:
        {
            ret_val = run_orientation_demo(env, err, args);
            break;
        }
        case SCENARIO_CLEAN_FILE:
        {
            ret_val = run_clean_file(env, err, args);
            break;
        }
        case SCENARIO_C_MEMORY_RUNTIME:
        {
            ret_val = run_c_memory_runtime_demo(env, err, args);
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
        case SCENARIO_UNSAFE_LOG_SECRET:
        {
            ret_val = run_unsafe_log_secret_demo(env, err, args);
            break;
        }
        case SCENARIO_LOG_INJECTION:
        {
            ret_val = run_log_injection_demo(env, err, args);
            break;
        }
        case SCENARIO_MISSING_STRUCTURED_LOG:
        {
            ret_val = run_missing_structured_log_demo(env, err, args);
            break;
        }
        case SCENARIO_INPUT_VALIDATION:
        {
            ret_val = run_input_validation_demo(env, err, args);
            break;
        }
        case SCENARIO_COMMAND_INJECTION:
        {
            ret_val = run_command_injection_demo(env, err, args);
            break;
        }
        case SCENARIO_PREDICTABLE_TEMP_FILE:
        {
            ret_val = run_predictable_temp_file_demo(env, err, args);
            break;
        }
        case SCENARIO_SIGNED_CONVERSION:
        {
            ret_val = run_signed_conversion_demo(env, err, args);
            break;
        }
        case SCENARIO_TRUNCATION:
        {
            ret_val = run_truncation_demo(env, err, args);
            break;
        }
        case SCENARIO_USE_AFTER_FREE:
        {
            ret_val = run_use_after_free_demo(env, err, args);
            break;
        }
        case SCENARIO_REALLOC_FAILURE:
        {
            ret_val = run_realloc_failure_demo(env, err, args);
            break;
        }
        case SCENARIO_OUT_OF_BOUNDS_WRITE:
        {
            ret_val = run_out_of_bounds_write_demo(env, err, args);
            break;
        }
        case SCENARIO_OUT_OF_BOUNDS_READ:
        {
            ret_val = run_out_of_bounds_read_demo(env, err, args);
            break;
        }
        case SCENARIO_BUFFER_OVERFLOW:
        {
            ret_val = run_buffer_overflow_demo(env, err, args);
            break;
        }
        case SCENARIO_UNINITIALIZED_READ:
        {
            ret_val = run_uninitialized_read_demo(env, err, args);
            break;
        }
        case SCENARIO_NULL_DEREFERENCE:
        {
            ret_val = run_null_dereference_demo(env, err, args);
            break;
        }
        case SCENARIO_INTEGER_OVERFLOW:
        {
            ret_val = run_integer_overflow_demo(env, err, args);
            break;
        }
        case SCENARIO_PATH_TRAVERSAL:
        {
            ret_val = run_path_traversal_demo(env, err, args);
            break;
        }
        case SCENARIO_FORMAT_STRING:
        {
            ret_val = run_format_string_demo(env, err, args);
            break;
        }
        case SCENARIO_STALE_SECRET:
        {
            ret_val = run_stale_secret_demo(env, err, args);
            break;
        }
        case SCENARIO_RESOURCE_EXHAUSTION:
        {
            ret_val = run_resource_exhaustion_demo(env, err, args);
            break;
        }
        case SCENARIO_TOCTOU:
        {
            ret_val = run_toctou_demo(env, err, args);
            break;
        }
        case SCENARIO_DATA_RACE:
        {
            ret_val = run_data_race_demo(env, err, args);
            break;
        }
        case SCENARIO_STRING_NOT_TERMINATED:
        {
            ret_val = run_string_not_terminated_demo(env, err, args);
            break;
        }
        case SCENARIO_PARTIAL_WRITE:
        {
            ret_val = run_partial_write_demo(env, err, args);
            break;
        }
        case SCENARIO_INTERRUPTED_SYSCALL:
        {
            ret_val = run_interrupted_syscall_demo(env, err, args);
            break;
        }
        case SCENARIO_UNSAFE_FILE_MODE:
        {
            ret_val = run_unsafe_file_mode_demo(env, err, args);
            break;
        }
        case SCENARIO_SYMLINK_FOLLOW:
        {
            ret_val = run_symlink_follow_demo(env, err, args);
            break;
        }
        case SCENARIO_TRUSTED_ENVIRONMENT:
        {
            ret_val = run_trusted_environment_demo(env, err, args);
            break;
        }
        case SCENARIO_UNCHECKED_PARSE:
        {
            ret_val = run_unchecked_parse_demo(env, err, args);
            break;
        }
        case SCENARIO_MISSING_AUTHORIZATION:
        {
            ret_val = run_missing_authorization_demo(env, err, args);
            break;
        }
        case SCENARIO_CLEANUP_ORDER:
        {
            ret_val = run_cleanup_order_demo(env, err, args);
            break;
        }
        case SCENARIO_THREAD_ARGUMENT_LIFETIME:
        {
            ret_val = run_thread_argument_lifetime_demo(env, err, args);
            break;
        }
        case SCENARIO_SHORT_READ:
        {
            ret_val = run_short_read_demo(env, err, args);
            break;
        }
        case SCENARIO_READ_EOF_HANDLING:
        {
            ret_val = run_read_eof_handling_demo(env, err, args);
            break;
        }
        case SCENARIO_PARSER_FUZZ:
        {
            ret_val = run_parser_fuzz_demo(env, err, args);
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

static int run_orientation_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args)
{
    static const char output_text[] = "p101-orientation: env err wrappers tools\n"
                                      "env: created once in main and passed to every p101-aware function\n"
                                      "err: checked after fallible wrappers and preserved through cleanup\n"
                                      "wrappers: p101_malloc p101_memset p101_snprintf p101_open p101_write p101_close p101_free\n"
                                      "tools: p101 check observe resource-tracker trace report error-path-walk wrapper-audit module-map\n";
    char             *message;
    int               ret_val;

    P101_TRACE_SCOPE(env);
    message = NULL;
    ret_val = EXIT_FAILURE;

    p101_printf(env, err, "orientation: env + err + wrappers + tools\n");
    if(p101_error_has_error(err))
    {
        goto done;
    }

    message = (char *)p101_malloc(env, err, ORIENTATION_MESSAGE_BYTES);
    if(message == NULL || p101_error_has_error(err))
    {
        goto done;
    }
    p101_memset(env, message, 0, ORIENTATION_MESSAGE_BYTES);
    p101_snprintf(env, err, message, ORIENTATION_MESSAGE_BYTES, "%s", "p101 orientation");
    if(p101_error_has_error(err))
    {
        goto done;
    }

    if(write_text_output(env, err, args, output_text) != EXIT_SUCCESS)
    {
        goto done;
    }

    ret_val = EXIT_SUCCESS;

done:
    p101_free(env, message);
    return ret_val;
}

static int run_clean_file(const struct p101_env *env, struct p101_error *err, const struct arguments *args)
{
    P101_TRACE_SCOPE(env);
    p101_printf(env, err, "clean-file: write/close/free all resources\n");
    return write_demo_file(env, err, args, "clean-file", false, false);
}

static int run_c_memory_runtime_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args)
{
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

    buffer = (char *)p101_malloc(env, err, C_MEMORY_INITIAL_BYTES);
    if(buffer == NULL || p101_error_has_error(err))
    {
        goto done;
    }
    p101_memset(env, buffer, 'm', C_MEMORY_INITIAL_BYTES);

    zeroed = (char *)p101_calloc(env, err, C_MEMORY_ZEROED_ITEMS, C_MEMORY_ZEROED_ITEM_BYTES);
    if(zeroed == NULL || p101_error_has_error(err))
    {
        goto done;
    }

    grown = (char *)p101_realloc(env, err, buffer, C_MEMORY_GROWN_BYTES);
    if(grown == NULL || p101_error_has_error(err))
    {
        goto done;
    }
    buffer = grown;
    grown  = NULL;

    p101_qsort(env, values, C_MEMORY_VALUE_COUNT, sizeof(values[0]), compare_ints);
    found = (const int *)p101_bsearch(env, &key, values, C_MEMORY_VALUE_COUNT, sizeof(values[0]), compare_ints);
    if(found == NULL || *found != key)
    {
        P101_ERROR_RAISE_USER(err, "The qsort/bsearch smoke check failed.", ERR_SCENARIO_FAILURE);
        goto done;
    }

    if(p101_abs(env, err, -C_MEMORY_ABS_VALUE) != C_MEMORY_ABS_VALUE || p101_error_has_error(err))
    {
        goto done;
    }
    if(p101_labs(env, err, -C_MEMORY_ABS_VALUE) != C_MEMORY_ABS_VALUE || p101_error_has_error(err))
    {
        goto done;
    }
    if(p101_llabs(env, err, -C_MEMORY_ABS_VALUE) != C_MEMORY_ABS_VALUE || p101_error_has_error(err))
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

    if(p101_mblen(env, err, "A", 1) != 1 || p101_error_has_error(err))
    {
        goto done;
    }
    if(p101_mbtowc(env, err, wide_text, "A", 1) != 1 || p101_error_has_error(err))
    {
        goto done;
    }
    if(p101_mbstowcs(env, err, wide_text, "ABC", C_MEMORY_WIDE_TEXT_CAPACITY) == (size_t)-1 || p101_error_has_error(err))
    {
        goto done;
    }
    if(p101_wcstombs(env, err, mb_output, L"Z", sizeof(mb_output)) == (size_t)-1 || p101_error_has_error(err))
    {
        goto done;
    }
    if(p101_wctomb(env, err, mb_output, L'Q') <= 0 || p101_error_has_error(err))
    {
        goto done;
    }

    (void)p101_strtod(env, err, "1.25", NULL);
    if(p101_error_has_error(err))
    {
        goto done;
    }
    (void)p101_strtof(env, err, "2.5", NULL);
    if(p101_error_has_error(err))
    {
        goto done;
    }
    (void)p101_strtold(env, err, "3.75", NULL);
    if(p101_error_has_error(err))
    {
        goto done;
    }
    (void)p101_strtol(env, err, "42", NULL, C_MEMORY_PARSE_BASE);
    if(p101_error_has_error(err))
    {
        goto done;
    }
    (void)p101_strtoll(env, err, "420", NULL, C_MEMORY_PARSE_BASE);
    if(p101_error_has_error(err))
    {
        goto done;
    }
    (void)p101_strtoul(env, err, "42", NULL, C_MEMORY_PARSE_BASE);
    if(p101_error_has_error(err))
    {
        goto done;
    }
    (void)p101_strtoull(env, err, "420", NULL, C_MEMORY_PARSE_BASE);
    if(p101_error_has_error(err))
    {
        goto done;
    }

    p101_setenv(env, err, "P101_TRACK_MEMORY_RUNTIME", "setenv", 1);
    if(p101_error_has_error(err))
    {
        goto done;
    }
    if(p101_getenv(env, err, "P101_TRACK_MEMORY_RUNTIME") == NULL)
    {
        P101_ERROR_RAISE_USER(err, "The setenv/getenv smoke check failed.", ERR_SCENARIO_FAILURE);
        goto done;
    }
    p101_unsetenv(env, err, "P101_TRACK_MEMORY_RUNTIME");
    if(p101_error_has_error(err))
    {
        goto done;
    }

    putenv_value = (char *)p101_malloc(env, err, C_MEMORY_PUTENV_BYTES);
    if(putenv_value == NULL || p101_error_has_error(err))
    {
        goto done;
    }
    p101_snprintf(env, err, putenv_value, C_MEMORY_PUTENV_BYTES, "%s", "P101_TRACK_PUTENV=1");
    if(p101_error_has_error(err))
    {
        goto done;
    }
    p101_putenv(env, err, putenv_value);
    if(p101_error_has_error(err))
    {
        goto done;
    }
    if(p101_getenv(env, err, "P101_TRACK_PUTENV") == NULL)
    {
        P101_ERROR_RAISE_USER(err, "The putenv/getenv smoke check failed.", ERR_SCENARIO_FAILURE);
        goto done;
    }
    p101_unsetenv(env, err, "P101_TRACK_PUTENV");
    if(p101_error_has_error(err))
    {
        goto done;
    }

    if(p101_getsubopt(env, &option_cursor, option_keys, &option_value) != 0 || option_value == NULL)
    {
        goto done;
    }

    temp_fd = p101_mkstemp(env, err, temp_file_template);
    if(temp_fd == -1 || p101_error_has_error(err))
    {
        goto done;
    }
    close_fd_preserving_error(env, err, &temp_fd);
    if(p101_error_has_error(err))
    {
        goto done;
    }
    p101_remove(env, err, temp_file_template);
    if(p101_error_has_error(err))
    {
        goto done;
    }

    if(p101_mkdtemp(env, err, temp_dir_template) == NULL || p101_error_has_error(err))
    {
        goto done;
    }
    p101_rmdir(env, err, temp_dir_template);
    if(p101_error_has_error(err))
    {
        goto done;
    }

    if(p101_realpath(env, err, ".", resolved_path) == NULL || p101_error_has_error(err))
    {
        goto done;
    }

    if(p101_posix_memalign(env, err, &aligned, sizeof(void *) * C_MEMORY_ALIGNMENT_MULTIPLIER, C_MEMORY_ALIGNED_BYTES) != 0 || aligned == NULL || p101_error_has_error(err))
    {
        goto done;
    }
    c_aligned = p101_aligned_alloc(env, err, sizeof(void *) * C_MEMORY_ALIGNMENT_MULTIPLIER, C_MEMORY_ALIGNED_BYTES);
    if(c_aligned == NULL || p101_error_has_error(err))
    {
        goto done;
    }

    (void)p101_a64l(env, "Az");
    (void)p101_l64a(env, C_MEMORY_L64A_VALUE);
    previous_state = p101_initstate(env, err, 1U, random_state, sizeof(random_state));
    if(previous_state == NULL || p101_error_has_error(err))
    {
        goto done;
    }
    active_state = p101_setstate(env, err, random_state);
    if(active_state == NULL || p101_error_has_error(err))
    {
        goto done;
    }
    (void)p101_setstate(env, err, previous_state);
    p101_srandom(env, 1U);
    p101_srand48(env, 1L);
    (void)p101_seed48(env, seed_values);

    (void)p101_arc4random(env);
    p101_arc4random_buf(env, random_bytes, sizeof(random_bytes));
    (void)p101_arc4random_uniform(env, C_MEMORY_ARC4RANDOM_UPPER_BOUND);
    (void)p101_getloadavg(env, err, loadavg, C_MEMORY_LOADAVG_COUNT);
    if(p101_error_has_error(err))
    {
        goto done;
    }
    (void)p101_rpmatch(env, "yes");

    /*
     * Some systems can deny pseudo-terminal allocation in constrained
     * environments. The wrapper still gets exercised; grantpt/unlockpt/ptsname
     * are exercised only when a pty was actually acquired.
     */
    pty_fd = p101_posix_openpt(env, err, O_RDWR | O_NOCTTY);
    if(pty_fd != -1 && p101_error_has_no_error(err))
    {
        p101_grantpt(env, err, pty_fd);
        p101_unlockpt(env, err, pty_fd);
        (void)p101_ptsname(env, err, pty_fd);
        if(p101_error_has_error(err))
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
        close_fd_preserving_error(env, err, &pty_fd);
        if(p101_error_has_error(err))
        {
            goto done;
        }
    }

    if(p101_atexit(env, err, c_memory_runtime_atexit_hook) != 0 || p101_at_quick_exit(env, err, c_memory_runtime_atexit_hook) != 0 || p101_error_has_error(err))
    {
        goto done;
    }

    if(p101_system(env, err, ":") == -1 || p101_error_has_error(err))
    {
        goto done;
    }

    p101_printf(env, err, "c-memory-runtime: exercised 53 wrappers\n");
    if(p101_error_has_error(err))
    {
        goto done;
    }
    if(write_text_output(env, err, args, "c-memory-runtime: exercised 53 wrappers\n") != EXIT_SUCCESS)
    {
        goto done;
    }
    if(p101_error_has_no_error(err))
    {
        ret_val = EXIT_SUCCESS;
    }

done:
    close_fd_preserving_error(env, err, &pty_fd);
    close_fd_preserving_error(env, err, &temp_fd);
    p101_free(env, c_aligned);
    p101_free(env, aligned);
    p101_free(env, putenv_value);
    p101_free(env, zeroed);
    p101_free(env, buffer);
    return ret_val;
}

static int run_realloc_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args)
{
    char *buffer;
    char *grown;
    int   ret_val;

    P101_TRACE_SCOPE(env);
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

    P101_TRACE_SCOPE(env);
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

    P101_TRACE_SCOPE(env);
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

        child_buffer = make_buffer(env, err, DEFAULT_BYTES, 'c');
        p101_close(env, err, fds[0]);
        fds[0] = -1;
        p101_write(env, err, fds[1], message, sizeof(message));
        p101_close(env, err, fds[1]);
        fds[1] = -1;
        p101_free(env, child_buffer);
        ret_val = EXIT_SUCCESS;

        if(p101_error_has_error(err))
        {
            ret_val = EXEC_FAILURE;
        }
    }
    else
    {
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
    P101_TRACE_SCOPE(env);
    p101_printf(env, err, "fd-leak: intentionally leaves the output descriptor open\n");
    return write_demo_file(env, err, args, "fd-leak", true, false);
}

static int run_alloc_leak_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args)
{
    P101_TRACE_SCOPE(env);
    p101_printf(env, err, "alloc-leak: intentionally leaves one heap allocation live\n");
    return write_demo_file(env, err, args, "alloc-leak", false, true);
}

static int run_double_close_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args)
{
    int ret_val;
    int fd;

    P101_TRACE_SCOPE(env);
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

    P101_TRACE_SCOPE(env);
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

    P101_TRACE_SCOPE(env);
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
    int   ret_val;

    P101_TRACE_SCOPE(env);
    buffer  = NULL;
    ret_val = EXIT_FAILURE;
    p101_printf(env, err, "early-return-fd-leak: returns before descriptor cleanup\n");

    fd = p101_open(env, err, args->output_path, O_WRONLY | O_CREAT | O_TRUNC, REPORT_FILE_MODE);
    if(p101_error_has_error(err))
    {
        goto done;
    }

    buffer = make_buffer(env, err, args->bytes, 'e');
    if(p101_error_has_error(err))
    {
        goto done;
    }

    p101_write(env, err, fd, buffer, args->bytes);
    p101_free(env, buffer);

    if(p101_error_has_no_error(err))
    {
        ret_val = EXIT_SUCCESS;
    }

done:
    return ret_val;
}

static int run_early_return_alloc_leak_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args)
{
    const char *buffer;
    int         ret_val;

    P101_TRACE_SCOPE(env);
    ret_val = EXIT_FAILURE;
    p101_printf(env, err, "early-return-alloc-leak: returns before allocation cleanup\n");

    buffer = make_buffer(env, err, args->bytes, 'a');
    if(buffer != NULL && p101_error_has_no_error(err))
    {
        ret_val = EXIT_SUCCESS;
    }

    return ret_val;
}

static int run_partial_cleanup_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args)
{
    int         file_fd;
    int         pipe_fds[2];
    const char *buffer;
    int         ret_val;

    P101_TRACE_SCOPE(env);
    buffer      = NULL;
    pipe_fds[0] = -1;
    pipe_fds[1] = -1;
    ret_val     = EXIT_FAILURE;
    p101_printf(env, err, "partial-cleanup: acquires several resources and releases only some\n");

    file_fd = p101_open(env, err, args->output_path, O_WRONLY | O_CREAT | O_TRUNC, REPORT_FILE_MODE);
    if(p101_error_has_error(err))
    {
        goto done;
    }

    buffer = make_buffer(env, err, args->bytes, 'p');
    if(p101_error_has_error(err))
    {
        goto done;
    }

    p101_pipe(env, err, pipe_fds);
    if(p101_error_has_error(err))
    {
        goto done;
    }

    p101_write(env, err, file_fd, buffer, args->bytes);
    p101_close(env, err, pipe_fds[1]);

    if(p101_error_has_no_error(err))
    {
        ret_val = EXIT_SUCCESS;
    }

done:
    return ret_val;
}

static int run_realloc_leak_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args)
{
    char *buffer;
    int   ret_val;

    P101_TRACE_SCOPE(env);
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

    P101_TRACE_SCOPE(env);
    ret_val = EXIT_FAILURE;
    p101_printf(env, err, "exec-inherit: opens a descriptor without FD_CLOEXEC and reaches exec\n");

    fd = p101_open(env, err, args->output_path, O_WRONLY | O_CREAT | O_TRUNC, REPORT_FILE_MODE);
    if(p101_error_has_error(err))
    {
        goto done;
    }

    /*
     * Record the privilege-boundary event without replacing this teaching
     * process. A real failed exec would correctly cancel the inheritance
     * finding with EXECFAIL; this case models the successful boundary while
     * retaining control long enough to write the completion record.
     */
    P101_TRACK_EXEC(env, child_argv[0]);
    ret_val = EXIT_SUCCESS;

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
    int   ret_val;

    P101_TRACE_SCOPE(env);
    ret_val = EXIT_FAILURE;
    p101_printf(env, err, "double-free: intentionally releases one allocation twice\n");

    buffer = make_buffer(env, err, args->bytes, 'd');
    if(buffer != NULL && p101_error_has_no_error(err))
    {
        /*
         * Emit the erroneous first release, then let p101_free emit the second
         * release while performing the one real free. Calling free twice would
         * be undefined behavior and could abort before the event stream
         * completes.
         */
        P101_TRACK_FREE(env, buffer);
        p101_free(env, buffer);
        ret_val = EXIT_SUCCESS;
    }

    return ret_val;
}

static int run_stray_free_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args)
{
    char *buffer;
    int   ret_val;

    P101_TRACE_SCOPE(env);
    ret_val = EXIT_FAILURE;
    p101_printf(env, err, "stray-free: intentionally frees an interior pointer\n");

    buffer = make_buffer(env, err, args->bytes, 's');
    if(buffer != NULL && p101_error_has_no_error(err))
    {
        const char *interior;

        interior = buffer + 1;
        /*
         * The bad event is the attempted release of an interior pointer.
         * Record it explicitly, then free the actual allocation once so the
         * fixture is deterministic under every allocator.
         */
        P101_TRACK_FREE(env, interior);
        p101_free(env, buffer);
        ret_val = EXIT_SUCCESS;
    }

    return ret_val;
}

static int run_sizeof_pointer_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args)
{
    int   fd;
    char *buffer;
    int   ret_val;

    P101_TRACE_SCOPE(env);
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

#ifndef __clang_analyzer__
    p101_write(env, err, fd, buffer, sizeof(buffer));
#else
    p101_write(env, err, fd, buffer, args->bytes);
#endif
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

    P101_TRACE_SCOPE(env);
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

static int run_unsafe_log_secret_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args)
{
    const char log_text[] = "severity=warning event=login_failed user=student password=hunter2 outcome=denied\n";

    P101_TRACE_SCOPE(env);
    p101_printf(env, err, "unsafe-log-secret: writes a secret directly into a log record\n");
    return write_text_output(env, err, args, log_text);
}

static int run_log_injection_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args)
{
    const char log_text[] = "severity=warning event=login_failed user=mallory\nseverity=info event=admin_login outcome=success user=root\n outcome=denied\n";

    P101_TRACE_SCOPE(env);
    p101_printf(env, err, "log-injection: untrusted text forges an extra log record\n");
    return write_text_output(env, err, args, log_text);
}

static int run_missing_structured_log_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args)
{
    const char log_text[] = "bad password\n";

    P101_TRACE_SCOPE(env);
    p101_printf(env, err, "missing-structured-log: emits vague text without event, severity, or outcome fields\n");
    return write_text_output(env, err, args, log_text);
}

static int run_input_validation_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args)
{
    const char log_text[] = "severity=info event=file_export path=../../etc/passwd outcome=accepted\n";

    P101_TRACE_SCOPE(env);
    p101_printf(env, err, "input-validation: accepts a path-shaped value that should be rejected at the boundary\n");
    return write_text_output(env, err, args, log_text);
}

static int run_command_injection_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args)
{
    const char log_text[] = "severity=warning event=command_preview command=\"tar -cf backup.tar playground; rm -rf /\" outcome=prepared\n";

    P101_TRACE_SCOPE(env);
    p101_printf(env, err, "command-injection: builds a shell command by concatenating untrusted text\n");
    return write_text_output(env, err, args, log_text);
}

static int run_predictable_temp_file_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args)
{
    const char log_text[] = "severity=info event=tempfile_create path=/tmp/p101-tool-playground.tmp outcome=created\n";

    P101_TRACE_SCOPE(env);
    p101_printf(env, err, "predictable-temp-file: uses a predictable temporary filename\n");
    return write_text_output(env, err, args, log_text);
}

static int run_signed_conversion_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args)
{
    char         log_text[READ_BUF_LEN];
    const int    parsed_count    = -1;
    const size_t converted_count = (size_t)parsed_count;

    P101_TRACE_SCOPE(env);
    p101_snprintf(env, err, log_text, sizeof(log_text), "severity=error event=count_parse parsed=%d converted=%zu outcome=accepted\n", parsed_count, converted_count);
    p101_printf(env, err, "signed-conversion: turns a negative count into a huge unsigned size\n");
    return write_text_output(env, err, args, log_text);
}

static int run_truncation_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args)
{
    char               log_text[READ_BUF_LEN];
    const unsigned int requested_count = 70000U;
    unsigned int       stored_count;
    unsigned short     narrow_count;

    P101_TRACE_SCOPE(env);
    narrow_count = (unsigned short)requested_count;
    stored_count = narrow_count;
    p101_snprintf(env, err, log_text, sizeof(log_text), "severity=error event=count_store requested=%u stored=%u outcome=accepted\n", requested_count, stored_count);
    p101_printf(env, err, "truncation: stores a large count in a too-small integer type\n");
    return write_text_output(env, err, args, log_text);
}

static int run_use_after_free_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args)
{
    const char log_text[] = "severity=error event=use_after_free pointer_state=freed action=used outcome=accepted\n";

    P101_TRACE_SCOPE(env);
    p101_printf(env, err, "use-after-free: records ownership use after free\n");
    return write_text_output(env, err, args, log_text);
}

static int run_realloc_failure_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args)
{
    const char log_text[] = "severity=error event=realloc_failure original_pointer=lost outcome=leaked\n";

    P101_TRACE_SCOPE(env);
    p101_printf(env, err, "realloc-failure: records losing the original pointer on failed grow\n");
    return write_text_output(env, err, args, log_text);
}

static int run_out_of_bounds_write_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args)
{
    const char log_text[] = "severity=error event=bounds_check operation=write index=16 capacity=16 outcome=accepted\n";

    P101_TRACE_SCOPE(env);
    p101_printf(env, err, "out-of-bounds-write: records accepting an index past the writable range\n");
    return write_text_output(env, err, args, log_text);
}

static int run_out_of_bounds_read_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args)
{
    const char log_text[] = "severity=error event=bounds_check operation=read index=16 valid=16 outcome=accepted\n";

    P101_TRACE_SCOPE(env);
    p101_printf(env, err, "out-of-bounds-read: records accepting an index past the readable range\n");
    return write_text_output(env, err, args, log_text);
}

static int run_buffer_overflow_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args)
{
    const char log_text[] = "severity=error event=copy_check source_bytes=32 destination_bytes=16 outcome=accepted\n";

    P101_TRACE_SCOPE(env);
    p101_printf(env, err, "buffer-overflow: records accepting a copy that does not fit\n");
    return write_text_output(env, err, args, log_text);
}

static int run_uninitialized_read_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args)
{
    const char log_text[] = "severity=error event=state_read initialized=false outcome=used\n";

    P101_TRACE_SCOPE(env);
    p101_printf(env, err, "uninitialized-read: records using a value before initialization\n");
    return write_text_output(env, err, args, log_text);
}

static int run_null_dereference_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args)
{
    const char log_text[] = "severity=error event=null_check pointer=null action=dereference outcome=accepted\n";

    P101_TRACE_SCOPE(env);
    p101_printf(env, err, "null-dereference: records continuing after a NULL result\n");
    return write_text_output(env, err, args, log_text);
}

static int run_integer_overflow_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args)
{
    const char log_text[] = "severity=error event=size_multiply count=9223372036854775808 width=2 outcome=wrapped\n";

    P101_TRACE_SCOPE(env);
    p101_printf(env, err, "integer-overflow: records an allocation size calculation that wrapped\n");
    return write_text_output(env, err, args, log_text);
}

static int run_path_traversal_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args)
{
    const char log_text[] = "severity=error event=path_check root=/safe path=/safe/../secret.txt outcome=accepted\n";

    P101_TRACE_SCOPE(env);
    p101_printf(env, err, "path-traversal: records a path that escapes its intended root\n");
    return write_text_output(env, err, args, log_text);
}

static int run_format_string_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args)
{
    const char log_text[] = "severity=error event=format_string format=%x%x%x source=user outcome=used_as_format\n";

    P101_TRACE_SCOPE(env);
    p101_printf(env, err, "format-string: records user text treated as a printf format\n");
    return write_text_output(env, err, args, log_text);
}

static int run_stale_secret_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args)
{
    const char log_text[] = "severity=error event=buffer_reuse old_secret=api-key-123 outcome=leaked\n";

    P101_TRACE_SCOPE(env);
    p101_printf(env, err, "stale-secret: records reusing a buffer before clearing secret bytes\n");
    return write_text_output(env, err, args, log_text);
}

static int run_resource_exhaustion_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args)
{
    const char log_text[] = "severity=error event=allocation_request requested=unbounded limit=none outcome=accepted\n";

    P101_TRACE_SCOPE(env);
    p101_printf(env, err, "resource-exhaustion: records accepting an unbounded resource request\n");
    return write_text_output(env, err, args, log_text);
}

static int run_toctou_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args)
{
    const char log_text[] = "severity=error event=file_open pattern=check_then_use path=/tmp/p101-target outcome=accepted\n";

    P101_TRACE_SCOPE(env);
    p101_printf(env, err, "toctou: records checking a path separately from using it\n");
    return write_text_output(env, err, args, log_text);
}

static int run_data_race_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args)
{
    const char log_text[] = "severity=error event=counter_update threads=2 synchronization=none outcome=accepted\n";

    P101_TRACE_SCOPE(env);
    p101_printf(env, err, "data-race: records updating shared state without synchronization\n");
    return write_text_output(env, err, args, log_text);
}

static int run_string_not_terminated_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args)
{
    const char log_text[] = "severity=error event=string_copy copied=16 capacity=16 terminator=missing outcome=accepted\n";

    P101_TRACE_SCOPE(env);
    p101_printf(env, err, "string-not-terminated: records treating unterminated bytes as a C string\n");
    return write_text_output(env, err, args, log_text);
}

static int run_partial_write_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args)
{
    const char log_text[] = "severity=error event=write_loop requested=64 written=17 outcome=accepted\n";

    P101_TRACE_SCOPE(env);
    p101_printf(env, err, "partial-write: records accepting a short write as complete\n");
    return write_text_output(env, err, args, log_text);
}

static int run_interrupted_syscall_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args)
{
    const char log_text[] = "severity=error event=syscall_retry function=read errno=EINTR outcome=failed\n";

    P101_TRACE_SCOPE(env);
    p101_printf(env, err, "interrupted-syscall: records treating EINTR as a final failure\n");
    return write_text_output(env, err, args, log_text);
}

static int run_unsafe_file_mode_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args)
{
    const char log_text[] = "severity=error event=file_create mode=0666 contains_secret=true outcome=accepted\n";

    P101_TRACE_SCOPE(env);
    p101_printf(env, err, "unsafe-file-mode: records creating a sensitive file with broad permissions\n");
    return write_text_output(env, err, args, log_text);
}

static int run_symlink_follow_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args)
{
    const char log_text[] = "severity=error event=file_open path=/tmp/p101-link follows_symlink=true outcome=accepted\n";

    P101_TRACE_SCOPE(env);
    p101_printf(env, err, "symlink-follow: records following a symlink for a sensitive file open\n");
    return write_text_output(env, err, args, log_text);
}

static int run_trusted_environment_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args)
{
    const char log_text[] = "severity=error event=environment_lookup variable=PATH trust=untrusted outcome=used_for_exec\n";

    P101_TRACE_SCOPE(env);
    p101_printf(env, err, "trusted-environment: records trusting an environment variable for execution\n");
    return write_text_output(env, err, args, log_text);
}

static int run_unchecked_parse_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args)
{
    const char log_text[] = "severity=error event=parse_int input=123abc consumed=3 trailing=abc outcome=accepted\n";

    P101_TRACE_SCOPE(env);
    p101_printf(env, err, "unchecked-parse: records accepting trailing junk after a numeric parse\n");
    return write_text_output(env, err, args, log_text);
}

static int run_missing_authorization_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args)
{
    const char log_text[] = "severity=error event=delete_project authenticated=true authorized=false outcome=accepted\n";

    P101_TRACE_SCOPE(env);
    p101_printf(env, err, "missing-authorization: records authentication without an authorization check\n");
    return write_text_output(env, err, args, log_text);
}

static int run_cleanup_order_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args)
{
    const char log_text[] = "severity=error event=cleanup_order parent=destroyed child=still_owned outcome=accepted\n";

    P101_TRACE_SCOPE(env);
    p101_printf(env, err, "cleanup-order: records destroying parent state before child state\n");
    return write_text_output(env, err, args, log_text);
}

static int run_thread_argument_lifetime_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args)
{
    const char log_text[] = "severity=error event=thread_argument storage=stack thread=running outcome=escaped\n";

    P101_TRACE_SCOPE(env);
    p101_printf(env, err, "thread-argument-lifetime: records a thread using an expired argument\n");
    return write_text_output(env, err, args, log_text);
}

static int run_short_read_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args)
{
    const char log_text[] = "severity=error event=read_loop requested=64 read=17 object_complete=false outcome=accepted\n";

    P101_TRACE_SCOPE(env);
    p101_printf(env, err, "short-read: records accepting a short read as a complete object\n");
    return write_text_output(env, err, args, log_text);
}

static int run_read_eof_handling_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args)
{
    const char log_text[] = "severity=error event=read_result result=0 meaning=error outcome=accepted\n";

    P101_TRACE_SCOPE(env);
    p101_printf(env, err, "read-eof-handling: records treating EOF as an I/O error\n");
    return write_text_output(env, err, args, log_text);
}

static int run_parser_fuzz_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args)
{
    const char log_text[] = "severity=warning event=parser_boundary_check fuzz_target=missing outcome=untested\n";

    P101_TRACE_SCOPE(env);
    p101_printf(env, err, "parser-fuzz: records boundary-heavy parsing without a fuzz target\n");
    return write_text_output(env, err, args, log_text);
}

static void close_fd_preserving_error(const struct p101_env *env, struct p101_error *err, int *fd)
{
    P101_TRACE_SCOPE(env);
    if(fd != NULL && *fd != -1)
    {
        if(p101_error_has_no_error(err))
        {
            p101_close(env, err, *fd);
        }
        else
        {
            struct p101_error *cleanup_err;

            cleanup_err = p101_error_create(false);
            if(cleanup_err != NULL)
            {
                p101_close(env, cleanup_err, *fd);
                p101_error_destroy(cleanup_err);
            }
            else
            {
                /*
                 * Last-resort cleanup: p101_error_create itself could not
                 * allocate a scratch error object, so close with the primary
                 * object rather than intentionally leaking the descriptor.
                 */
                p101_close(env, err, *fd);
            }
        }
        *fd = -1;
    }
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

static int write_demo_file(const struct p101_env *env, struct p101_error *err, const struct arguments *args, const char *label, bool leak_fd, bool leak_alloc)
{
    int   fd;
    char *buffer;
    int   ret_val;

    P101_TRACE_SCOPE(env);
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

static int write_text_output(const struct p101_env *env, struct p101_error *err, const struct arguments *args, const char *text)
{
    int    fd;
    size_t length;
    int    ret_val;

    P101_TRACE_SCOPE(env);
    ret_val = EXIT_FAILURE;
    fd      = p101_open(env, err, args->output_path, O_WRONLY | O_CREAT | O_TRUNC, REPORT_FILE_MODE);
    if(p101_error_has_error(err))
    {
        goto done;
    }

    length = p101_strlen(env, text);
    p101_write(env, err, fd, text, length);
    if(p101_error_has_no_error(err))
    {
        ret_val = EXIT_SUCCESS;
    }

done:
    if(fd != -1)
    {
        p101_close(env, err, fd);
    }

    return ret_val;
}

static char *make_buffer(const struct p101_env *env, struct p101_error *err, size_t bytes, char fill)
{
    char *buffer;

    P101_TRACE_SCOPE(env);
    buffer = (char *)p101_malloc(env, err, bytes);

    if(buffer != NULL)
    {
        p101_memset(env, buffer, fill, bytes);
    }

    return buffer;
}

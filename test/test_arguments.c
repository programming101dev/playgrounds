#include "cli.h"
#include "constants.h"
#include "errors.h"
#include "scenario.h"
#include "unity.h"
#include <p101_c/p101_string.h>
#include <p101_env/env.h>
#include <p101_error/error.h>
#include <stdbool.h>
#include <unistd.h>

static struct p101_error *error;
static struct p101_env   *env;

void setUp(void)
{
    error = p101_error_create(false);
    env   = p101_env_create(error, NULL);
}

void tearDown(void)
{
    p101_env_destroy(env);
    p101_error_destroy(error);
}

static void reset_getopt(void)
{
#ifdef __GLIBC__
    optind = 0;
#else
    extern int optreset;
    optreset = 1;
    optind   = 1;
#endif
}

static void set_defaults(struct arguments *args)
{
    p101_memset(env, args, 0, sizeof(*args));
    args->scenario_str = DEFAULT_SCENARIO;
    args->output_path  = DEFAULT_OUTPUT_PATH;
    args->bytes        = DEFAULT_BYTES;
    args->repeats      = DEFAULT_REPEATS;
}

static void test_parse_accepts_full_option_set(void)
{
    char             *argv[] = {"p101-tool-playground", "-v", "-s", "pipe", "-o", "/tmp/out", "-b", "128", "-r", "3", NULL};
    struct arguments  args;

    reset_getopt();
    set_defaults(&args);

    p101_tool_playground_parse_arguments(env, error, 10, argv, &args);
    p101_tool_playground_check_arguments(env, error, &args);
    p101_tool_playground_convert_arguments(env, error, &args);

    TEST_ASSERT_FALSE(p101_error_has_error(error));
    TEST_ASSERT_TRUE(args.verbose);
    TEST_ASSERT_EQUAL_INT(SCENARIO_PIPE, args.scenario);
    TEST_ASSERT_EQUAL_STRING("/tmp/out", args.output_path);
    TEST_ASSERT_EQUAL_UINT(128U, args.bytes);
    TEST_ASSERT_EQUAL_UINT(3U, args.repeats);
}

static void test_scenario_names_round_trip(void)
{
    bool ok;

    TEST_ASSERT_EQUAL_INT(SCENARIO_TOUR, p101_tool_playground_scenario_from_name(env, "tour", &ok));
    TEST_ASSERT_TRUE(ok);
    TEST_ASSERT_EQUAL_STRING("tour", p101_tool_playground_scenario_name(SCENARIO_TOUR));
    TEST_ASSERT_EQUAL_INT(SCENARIO_FAULT_LAB, p101_tool_playground_scenario_from_name(env, "fault-lab", &ok));
    TEST_ASSERT_TRUE(ok);
    TEST_ASSERT_EQUAL_STRING("fault-lab", p101_tool_playground_scenario_name(SCENARIO_FAULT_LAB));
    TEST_ASSERT_EQUAL_INT(SCENARIO_EARLY_RETURN_FD_LEAK, p101_tool_playground_scenario_from_name(env, "early-return-fd-leak", &ok));
    TEST_ASSERT_TRUE(ok);
    TEST_ASSERT_EQUAL_STRING("early-return-fd-leak", p101_tool_playground_scenario_name(SCENARIO_EARLY_RETURN_FD_LEAK));
    TEST_ASSERT_EQUAL_INT(SCENARIO_REALLOC_LEAK, p101_tool_playground_scenario_from_name(env, "realloc-leak", &ok));
    TEST_ASSERT_TRUE(ok);
    TEST_ASSERT_EQUAL_STRING("realloc-leak", p101_tool_playground_scenario_name(SCENARIO_REALLOC_LEAK));
    TEST_ASSERT_EQUAL_INT(SCENARIO_EXEC_INHERIT, p101_tool_playground_scenario_from_name(env, "exec-inherit", &ok));
    TEST_ASSERT_TRUE(ok);
    TEST_ASSERT_EQUAL_STRING("exec-inherit", p101_tool_playground_scenario_name(SCENARIO_EXEC_INHERIT));
    TEST_ASSERT_EQUAL_INT(SCENARIO_DOUBLE_FREE, p101_tool_playground_scenario_from_name(env, "double-free", &ok));
    TEST_ASSERT_TRUE(ok);
    TEST_ASSERT_EQUAL_STRING("double-free", p101_tool_playground_scenario_name(SCENARIO_DOUBLE_FREE));
    TEST_ASSERT_EQUAL_INT(SCENARIO_STRAY_FREE, p101_tool_playground_scenario_from_name(env, "stray-free", &ok));
    TEST_ASSERT_TRUE(ok);
    TEST_ASSERT_EQUAL_STRING("stray-free", p101_tool_playground_scenario_name(SCENARIO_STRAY_FREE));
    TEST_ASSERT_EQUAL_INT(SCENARIO_SIZEOF_POINTER, p101_tool_playground_scenario_from_name(env, "sizeof-pointer", &ok));
    TEST_ASSERT_TRUE(ok);
    TEST_ASSERT_EQUAL_STRING("sizeof-pointer", p101_tool_playground_scenario_name(SCENARIO_SIZEOF_POINTER));
    TEST_ASSERT_EQUAL_INT(SCENARIO_IGNORE_READ_COUNT, p101_tool_playground_scenario_from_name(env, "ignore-read-count", &ok));
    TEST_ASSERT_TRUE(ok);
    TEST_ASSERT_EQUAL_STRING("ignore-read-count", p101_tool_playground_scenario_name(SCENARIO_IGNORE_READ_COUNT));
    TEST_ASSERT_EQUAL_INT(SCENARIO_UNSAFE_LOG_SECRET, p101_tool_playground_scenario_from_name(env, "unsafe-log-secret", &ok));
    TEST_ASSERT_TRUE(ok);
    TEST_ASSERT_EQUAL_STRING("unsafe-log-secret", p101_tool_playground_scenario_name(SCENARIO_UNSAFE_LOG_SECRET));
    TEST_ASSERT_EQUAL_INT(SCENARIO_LOG_INJECTION, p101_tool_playground_scenario_from_name(env, "log-injection", &ok));
    TEST_ASSERT_TRUE(ok);
    TEST_ASSERT_EQUAL_STRING("log-injection", p101_tool_playground_scenario_name(SCENARIO_LOG_INJECTION));
    TEST_ASSERT_EQUAL_INT(SCENARIO_MISSING_STRUCTURED_LOG, p101_tool_playground_scenario_from_name(env, "missing-structured-log", &ok));
    TEST_ASSERT_TRUE(ok);
    TEST_ASSERT_EQUAL_STRING("missing-structured-log", p101_tool_playground_scenario_name(SCENARIO_MISSING_STRUCTURED_LOG));
    TEST_ASSERT_EQUAL_INT(SCENARIO_INPUT_VALIDATION, p101_tool_playground_scenario_from_name(env, "input-validation", &ok));
    TEST_ASSERT_TRUE(ok);
    TEST_ASSERT_EQUAL_STRING("input-validation", p101_tool_playground_scenario_name(SCENARIO_INPUT_VALIDATION));
    TEST_ASSERT_EQUAL_INT(SCENARIO_COMMAND_INJECTION, p101_tool_playground_scenario_from_name(env, "command-injection", &ok));
    TEST_ASSERT_TRUE(ok);
    TEST_ASSERT_EQUAL_STRING("command-injection", p101_tool_playground_scenario_name(SCENARIO_COMMAND_INJECTION));
    TEST_ASSERT_EQUAL_INT(SCENARIO_PREDICTABLE_TEMP_FILE, p101_tool_playground_scenario_from_name(env, "predictable-temp-file", &ok));
    TEST_ASSERT_TRUE(ok);
    TEST_ASSERT_EQUAL_STRING("predictable-temp-file", p101_tool_playground_scenario_name(SCENARIO_PREDICTABLE_TEMP_FILE));
    TEST_ASSERT_EQUAL_INT(SCENARIO_SIGNED_CONVERSION, p101_tool_playground_scenario_from_name(env, "signed-conversion", &ok));
    TEST_ASSERT_TRUE(ok);
    TEST_ASSERT_EQUAL_STRING("signed-conversion", p101_tool_playground_scenario_name(SCENARIO_SIGNED_CONVERSION));
    TEST_ASSERT_EQUAL_INT(SCENARIO_TRUNCATION, p101_tool_playground_scenario_from_name(env, "truncation", &ok));
    TEST_ASSERT_TRUE(ok);
    TEST_ASSERT_EQUAL_STRING("truncation", p101_tool_playground_scenario_name(SCENARIO_TRUNCATION));
    TEST_ASSERT_EQUAL_INT(SCENARIO_USE_AFTER_FREE, p101_tool_playground_scenario_from_name(env, "use-after-free", &ok));
    TEST_ASSERT_TRUE(ok);
    TEST_ASSERT_EQUAL_STRING("use-after-free", p101_tool_playground_scenario_name(SCENARIO_USE_AFTER_FREE));
    TEST_ASSERT_EQUAL_INT(SCENARIO_REALLOC_FAILURE, p101_tool_playground_scenario_from_name(env, "realloc-failure", &ok));
    TEST_ASSERT_TRUE(ok);
    TEST_ASSERT_EQUAL_STRING("realloc-failure", p101_tool_playground_scenario_name(SCENARIO_REALLOC_FAILURE));
    TEST_ASSERT_EQUAL_INT(SCENARIO_OUT_OF_BOUNDS_WRITE, p101_tool_playground_scenario_from_name(env, "out-of-bounds-write", &ok));
    TEST_ASSERT_TRUE(ok);
    TEST_ASSERT_EQUAL_STRING("out-of-bounds-write", p101_tool_playground_scenario_name(SCENARIO_OUT_OF_BOUNDS_WRITE));
    TEST_ASSERT_EQUAL_INT(SCENARIO_OUT_OF_BOUNDS_READ, p101_tool_playground_scenario_from_name(env, "out-of-bounds-read", &ok));
    TEST_ASSERT_TRUE(ok);
    TEST_ASSERT_EQUAL_STRING("out-of-bounds-read", p101_tool_playground_scenario_name(SCENARIO_OUT_OF_BOUNDS_READ));
    TEST_ASSERT_EQUAL_INT(SCENARIO_BUFFER_OVERFLOW, p101_tool_playground_scenario_from_name(env, "buffer-overflow", &ok));
    TEST_ASSERT_TRUE(ok);
    TEST_ASSERT_EQUAL_STRING("buffer-overflow", p101_tool_playground_scenario_name(SCENARIO_BUFFER_OVERFLOW));
    TEST_ASSERT_EQUAL_INT(SCENARIO_UNINITIALIZED_READ, p101_tool_playground_scenario_from_name(env, "uninitialized-read", &ok));
    TEST_ASSERT_TRUE(ok);
    TEST_ASSERT_EQUAL_STRING("uninitialized-read", p101_tool_playground_scenario_name(SCENARIO_UNINITIALIZED_READ));
    TEST_ASSERT_EQUAL_INT(SCENARIO_NULL_DEREFERENCE, p101_tool_playground_scenario_from_name(env, "null-dereference", &ok));
    TEST_ASSERT_TRUE(ok);
    TEST_ASSERT_EQUAL_STRING("null-dereference", p101_tool_playground_scenario_name(SCENARIO_NULL_DEREFERENCE));
    TEST_ASSERT_EQUAL_INT(SCENARIO_INTEGER_OVERFLOW, p101_tool_playground_scenario_from_name(env, "integer-overflow", &ok));
    TEST_ASSERT_TRUE(ok);
    TEST_ASSERT_EQUAL_STRING("integer-overflow", p101_tool_playground_scenario_name(SCENARIO_INTEGER_OVERFLOW));
    TEST_ASSERT_EQUAL_INT(SCENARIO_PATH_TRAVERSAL, p101_tool_playground_scenario_from_name(env, "path-traversal", &ok));
    TEST_ASSERT_TRUE(ok);
    TEST_ASSERT_EQUAL_STRING("path-traversal", p101_tool_playground_scenario_name(SCENARIO_PATH_TRAVERSAL));
    TEST_ASSERT_EQUAL_INT(SCENARIO_FORMAT_STRING, p101_tool_playground_scenario_from_name(env, "format-string", &ok));
    TEST_ASSERT_TRUE(ok);
    TEST_ASSERT_EQUAL_STRING("format-string", p101_tool_playground_scenario_name(SCENARIO_FORMAT_STRING));
    TEST_ASSERT_EQUAL_INT(SCENARIO_STALE_SECRET, p101_tool_playground_scenario_from_name(env, "stale-secret", &ok));
    TEST_ASSERT_TRUE(ok);
    TEST_ASSERT_EQUAL_STRING("stale-secret", p101_tool_playground_scenario_name(SCENARIO_STALE_SECRET));
    TEST_ASSERT_EQUAL_INT(SCENARIO_RESOURCE_EXHAUSTION, p101_tool_playground_scenario_from_name(env, "resource-exhaustion", &ok));
    TEST_ASSERT_TRUE(ok);
    TEST_ASSERT_EQUAL_STRING("resource-exhaustion", p101_tool_playground_scenario_name(SCENARIO_RESOURCE_EXHAUSTION));
    TEST_ASSERT_EQUAL_INT(SCENARIO_TOCTOU, p101_tool_playground_scenario_from_name(env, "toctou", &ok));
    TEST_ASSERT_TRUE(ok);
    TEST_ASSERT_EQUAL_STRING("toctou", p101_tool_playground_scenario_name(SCENARIO_TOCTOU));
    TEST_ASSERT_EQUAL_INT(SCENARIO_DATA_RACE, p101_tool_playground_scenario_from_name(env, "data-race", &ok));
    TEST_ASSERT_TRUE(ok);
    TEST_ASSERT_EQUAL_STRING("data-race", p101_tool_playground_scenario_name(SCENARIO_DATA_RACE));
    TEST_ASSERT_EQUAL_INT(SCENARIO_STRING_NOT_TERMINATED, p101_tool_playground_scenario_from_name(env, "string-not-terminated", &ok));
    TEST_ASSERT_TRUE(ok);
    TEST_ASSERT_EQUAL_STRING("string-not-terminated", p101_tool_playground_scenario_name(SCENARIO_STRING_NOT_TERMINATED));
    TEST_ASSERT_EQUAL_INT(SCENARIO_PARTIAL_WRITE, p101_tool_playground_scenario_from_name(env, "partial-write", &ok));
    TEST_ASSERT_TRUE(ok);
    TEST_ASSERT_EQUAL_STRING("partial-write", p101_tool_playground_scenario_name(SCENARIO_PARTIAL_WRITE));
    TEST_ASSERT_EQUAL_INT(SCENARIO_INTERRUPTED_SYSCALL, p101_tool_playground_scenario_from_name(env, "interrupted-syscall", &ok));
    TEST_ASSERT_TRUE(ok);
    TEST_ASSERT_EQUAL_STRING("interrupted-syscall", p101_tool_playground_scenario_name(SCENARIO_INTERRUPTED_SYSCALL));
    TEST_ASSERT_EQUAL_INT(SCENARIO_UNSAFE_FILE_MODE, p101_tool_playground_scenario_from_name(env, "unsafe-file-mode", &ok));
    TEST_ASSERT_TRUE(ok);
    TEST_ASSERT_EQUAL_STRING("unsafe-file-mode", p101_tool_playground_scenario_name(SCENARIO_UNSAFE_FILE_MODE));
    TEST_ASSERT_EQUAL_INT(SCENARIO_SYMLINK_FOLLOW, p101_tool_playground_scenario_from_name(env, "symlink-follow", &ok));
    TEST_ASSERT_TRUE(ok);
    TEST_ASSERT_EQUAL_STRING("symlink-follow", p101_tool_playground_scenario_name(SCENARIO_SYMLINK_FOLLOW));
    TEST_ASSERT_EQUAL_INT(SCENARIO_TRUSTED_ENVIRONMENT, p101_tool_playground_scenario_from_name(env, "trusted-environment", &ok));
    TEST_ASSERT_TRUE(ok);
    TEST_ASSERT_EQUAL_STRING("trusted-environment", p101_tool_playground_scenario_name(SCENARIO_TRUSTED_ENVIRONMENT));
    TEST_ASSERT_EQUAL_INT(SCENARIO_UNCHECKED_PARSE, p101_tool_playground_scenario_from_name(env, "unchecked-parse", &ok));
    TEST_ASSERT_TRUE(ok);
    TEST_ASSERT_EQUAL_STRING("unchecked-parse", p101_tool_playground_scenario_name(SCENARIO_UNCHECKED_PARSE));
    TEST_ASSERT_EQUAL_INT(SCENARIO_MISSING_AUTHORIZATION, p101_tool_playground_scenario_from_name(env, "missing-authorization", &ok));
    TEST_ASSERT_TRUE(ok);
    TEST_ASSERT_EQUAL_STRING("missing-authorization", p101_tool_playground_scenario_name(SCENARIO_MISSING_AUTHORIZATION));
    TEST_ASSERT_EQUAL_INT(SCENARIO_CLEANUP_ORDER, p101_tool_playground_scenario_from_name(env, "cleanup-order", &ok));
    TEST_ASSERT_TRUE(ok);
    TEST_ASSERT_EQUAL_STRING("cleanup-order", p101_tool_playground_scenario_name(SCENARIO_CLEANUP_ORDER));
    TEST_ASSERT_EQUAL_INT(SCENARIO_THREAD_ARGUMENT_LIFETIME, p101_tool_playground_scenario_from_name(env, "thread-argument-lifetime", &ok));
    TEST_ASSERT_TRUE(ok);
    TEST_ASSERT_EQUAL_STRING("thread-argument-lifetime", p101_tool_playground_scenario_name(SCENARIO_THREAD_ARGUMENT_LIFETIME));
    TEST_ASSERT_EQUAL_INT(SCENARIO_SHORT_READ, p101_tool_playground_scenario_from_name(env, "short-read", &ok));
    TEST_ASSERT_TRUE(ok);
    TEST_ASSERT_EQUAL_STRING("short-read", p101_tool_playground_scenario_name(SCENARIO_SHORT_READ));
    TEST_ASSERT_EQUAL_INT(SCENARIO_READ_EOF_HANDLING, p101_tool_playground_scenario_from_name(env, "read-eof-handling", &ok));
    TEST_ASSERT_TRUE(ok);
    TEST_ASSERT_EQUAL_STRING("read-eof-handling", p101_tool_playground_scenario_name(SCENARIO_READ_EOF_HANDLING));
    TEST_ASSERT_EQUAL_INT(SCENARIO_PARSER_FUZZ, p101_tool_playground_scenario_from_name(env, "parser-fuzz", &ok));
    TEST_ASSERT_TRUE(ok);
    TEST_ASSERT_EQUAL_STRING("parser-fuzz", p101_tool_playground_scenario_name(SCENARIO_PARSER_FUZZ));
}

static void test_rejects_unknown_scenario(void)
{
    char             *argv[] = {"p101-tool-playground", "-s", "wat", NULL};
    struct arguments  args;

    reset_getopt();
    set_defaults(&args);

    p101_tool_playground_parse_arguments(env, error, 3, argv, &args);
    p101_tool_playground_check_arguments(env, error, &args);

    TEST_ASSERT_TRUE(p101_error_is_error(error, P101_ERROR_USER, ERR_USAGE));
}

static void test_rejects_too_many_bytes(void)
{
    char             *argv[] = {"p101-tool-playground", "-b", "4097", NULL};
    struct arguments  args;

    reset_getopt();
    set_defaults(&args);

    p101_tool_playground_parse_arguments(env, error, 3, argv, &args);
    p101_tool_playground_check_arguments(env, error, &args);
    p101_tool_playground_convert_arguments(env, error, &args);

    TEST_ASSERT_TRUE(p101_error_is_error(error, P101_ERROR_USER, ERR_USAGE));
}

static void test_rejects_positional_argument(void)
{
    char             *argv[] = {"p101-tool-playground", "extra", NULL};
    struct arguments  args;

    reset_getopt();
    set_defaults(&args);

    p101_tool_playground_parse_arguments(env, error, 2, argv, &args);

    TEST_ASSERT_TRUE(p101_error_is_error(error, P101_ERROR_USER, ERR_USAGE));
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_parse_accepts_full_option_set);
    RUN_TEST(test_scenario_names_round_trip);
    RUN_TEST(test_rejects_unknown_scenario);
    RUN_TEST(test_rejects_too_many_bytes);
    RUN_TEST(test_rejects_positional_argument);
    return UNITY_END();
}

#include "cli.h"
#include "constants.h"
#include "errors.h"
#include "scenario.h"
#include "unity.h"
#include <p101_c/p101_string.h>
#include <p101_env/env.h>
#include <p101_error/error.h>
#include <p101_posix/p101_unistd.h>
#include <stdbool.h>

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

#include "lesson_demo.h"
#include "constants.h"
#include "errors.h"
#include <fcntl.h>
#include <p101_c/p101_stdio.h>
#include <p101_c/p101_string.h>
#include <p101_io/p101_fcntl.h>
#include <p101_io/p101_unistd.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

struct lesson_demo
{
    enum playground_scenario scenario;
    const char              *description;
    const char              *record;
};

static const struct lesson_demo LESSON_DEMOS[] = {
    {SCENARIO_UNSAFE_LOG_SECRET,        "unsafe-log-secret: writes a secret directly into a log record\n",                         "severity=warning event=login_failed user=student password=hunter2 outcome=denied\n"                                            },
    {SCENARIO_LOG_INJECTION,            "log-injection: untrusted text forges an extra log record\n",                              "severity=warning event=login_failed user=mallory\nseverity=info event=admin_login outcome=success user=root\n outcome=denied\n"},
    {SCENARIO_MISSING_STRUCTURED_LOG,   "missing-structured-log: emits vague text without event, severity, or outcome fields\n",   "bad password\n"                                                                                                                },
    {SCENARIO_INPUT_VALIDATION,         "input-validation: accepts a path-shaped value that should be rejected at the boundary\n", "severity=info event=file_export path=../../etc/passwd outcome=accepted\n"                                                      },
    {SCENARIO_COMMAND_INJECTION,        "command-injection: builds a shell command by concatenating untrusted text\n",             "severity=warning event=command_preview command=\"tar -cf backup.tar playground; rm -rf /\" outcome=prepared\n"                 },
    {SCENARIO_PREDICTABLE_TEMP_FILE,    "predictable-temp-file: uses a predictable temporary filename\n",                          "severity=info event=tempfile_create path=/tmp/p101-tool-playground.tmp outcome=created\n"                                      },
    {SCENARIO_USE_AFTER_FREE,           "use-after-free: records ownership use after free\n",                                      "severity=error event=use_after_free pointer_state=freed action=used outcome=accepted\n"                                        },
    {SCENARIO_REALLOC_FAILURE,          "realloc-failure: records losing the original pointer on failed grow\n",                   "severity=error event=realloc_failure original_pointer=lost outcome=leaked\n"                                                   },
    {SCENARIO_OUT_OF_BOUNDS_WRITE,      "out-of-bounds-write: records accepting an index past the writable range\n",               "severity=error event=bounds_check operation=write index=16 capacity=16 outcome=accepted\n"                                     },
    {SCENARIO_OUT_OF_BOUNDS_READ,       "out-of-bounds-read: records accepting an index past the readable range\n",                "severity=error event=bounds_check operation=read index=16 valid=16 outcome=accepted\n"                                         },
    {SCENARIO_BUFFER_OVERFLOW,          "buffer-overflow: records accepting a copy that does not fit\n",                           "severity=error event=copy_check source_bytes=32 destination_bytes=16 outcome=accepted\n"                                       },
    {SCENARIO_UNINITIALIZED_READ,       "uninitialized-read: records using a value before initialization\n",                       "severity=error event=state_read initialized=false outcome=used\n"                                                              },
    {SCENARIO_NULL_DEREFERENCE,         "null-dereference: records continuing after a NULL result\n",                              "severity=error event=null_check pointer=null action=dereference outcome=accepted\n"                                            },
    {SCENARIO_INTEGER_OVERFLOW,         "integer-overflow: records an allocation size calculation that wrapped\n",                 "severity=error event=size_multiply count=9223372036854775808 width=2 outcome=wrapped\n"                                        },
    {SCENARIO_PATH_TRAVERSAL,           "path-traversal: records a path that escapes its intended root\n",                         "severity=error event=path_check root=/safe path=/safe/../secret.txt outcome=accepted\n"                                        },
    {SCENARIO_FORMAT_STRING,            "format-string: records user text treated as a printf format\n",                           "severity=error event=format_string format=%x%x%x source=user outcome=used_as_format\n"                                         },
    {SCENARIO_STALE_SECRET,             "stale-secret: records reusing a buffer before clearing secret bytes\n",                   "severity=error event=buffer_reuse old_secret=api-key-123 outcome=leaked\n"                                                     },
    {SCENARIO_RESOURCE_EXHAUSTION,      "resource-exhaustion: records accepting an unbounded resource request\n",                  "severity=error event=allocation_request requested=unbounded limit=none outcome=accepted\n"                                     },
    {SCENARIO_TOCTOU,                   "toctou: records checking a path separately from using it\n",                              "severity=error event=file_open pattern=check_then_use path=/tmp/p101-target outcome=accepted\n"                                },
    {SCENARIO_DATA_RACE,                "data-race: records updating shared state without synchronization\n",                      "severity=error event=counter_update threads=2 synchronization=none outcome=accepted\n"                                         },
    {SCENARIO_STRING_NOT_TERMINATED,    "string-not-terminated: records treating unterminated bytes as a C string\n",              "severity=error event=string_copy copied=16 capacity=16 terminator=missing outcome=accepted\n"                                  },
    {SCENARIO_PARTIAL_WRITE,            "partial-write: records accepting a short write as complete\n",                            "severity=error event=write_loop requested=64 written=17 outcome=accepted\n"                                                    },
    {SCENARIO_INTERRUPTED_SYSCALL,      "interrupted-syscall: records treating EINTR as a final failure\n",                        "severity=error event=syscall_retry function=read errno=EINTR outcome=failed\n"                                                 },
    {SCENARIO_UNSAFE_FILE_MODE,         "unsafe-file-mode: records creating a sensitive file with broad permissions\n",            "severity=error event=file_create mode=0666 contains_secret=true outcome=accepted\n"                                            },
    {SCENARIO_SYMLINK_FOLLOW,           "symlink-follow: records following a symlink for a sensitive file open\n",                 "severity=error event=file_open path=/tmp/p101-link follows_symlink=true outcome=accepted\n"                                    },
    {SCENARIO_TRUSTED_ENVIRONMENT,      "trusted-environment: records trusting an environment variable for execution\n",           "severity=error event=environment_lookup variable=PATH trust=untrusted outcome=used_for_exec\n"                                 },
    {SCENARIO_UNCHECKED_PARSE,          "unchecked-parse: records accepting trailing junk after a numeric parse\n",                "severity=error event=parse_int input=123abc consumed=3 trailing=abc outcome=accepted\n"                                        },
    {SCENARIO_MISSING_AUTHORIZATION,    "missing-authorization: records authentication without an authorization check\n",          "severity=error event=delete_project authenticated=true authorized=false outcome=accepted\n"                                    },
    {SCENARIO_CLEANUP_ORDER,            "cleanup-order: records destroying parent state before child state\n",                     "severity=error event=cleanup_order parent=destroyed child=still_owned outcome=accepted\n"                                      },
    {SCENARIO_THREAD_ARGUMENT_LIFETIME, "thread-argument-lifetime: records a thread using an expired argument\n",                  "severity=error event=thread_argument storage=stack thread=running outcome=escaped\n"                                           },
    {SCENARIO_SHORT_READ,               "short-read: records accepting a short read as a complete object\n",                       "severity=error event=read_loop requested=64 read=17 object_complete=false outcome=accepted\n"                                  },
    {SCENARIO_READ_EOF_HANDLING,        "read-eof-handling: records treating EOF as an I/O error\n",                               "severity=error event=read_result result=0 meaning=error outcome=accepted\n"                                                    },
    {SCENARIO_PARSER_FUZZ,              "parser-fuzz: records boundary-heavy parsing without a fuzz target\n",                     "severity=warning event=parser_boundary_check fuzz_target=missing outcome=untested\n"                                           },
};

int p101_tool_playground_write_text_output(const struct p101_env *env, struct p101_error *err, const struct arguments *args, const char *text)
{
    bool p101_bool_result_1;

    int    fd;
    size_t length;
    int    ret_val;

    P101_TRACE_SCOPE(env);
    ret_val            = EXIT_FAILURE;
    fd                 = p101_open(env, err, args->output_path, O_WRONLY | O_CREAT | O_TRUNC, REPORT_FILE_MODE);
    p101_bool_result_1 = p101_error_has_error(err);
    if(p101_bool_result_1)
    {
        goto done;
    }

    length = p101_strlen(env, text);
    p101_write(env, err, fd, text, length);
    p101_bool_result_1 = p101_error_has_no_error(err);
    if(p101_bool_result_1)
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

int p101_tool_playground_run_lesson_demo(const struct p101_env *env, struct p101_error *err, const struct arguments *args)
{
    const struct lesson_demo *selected;
    const char               *record;
    char                      dynamic_record[READ_BUF_LEN];
    int                       ret_val;

    P101_TRACE_SCOPE(env);
    selected = NULL;
    record   = NULL;
    ret_val  = EXIT_FAILURE;

    for(size_t i = 0; i < sizeof(LESSON_DEMOS) / sizeof(LESSON_DEMOS[0]); i++)
    {
        if(LESSON_DEMOS[i].scenario == args->scenario)
        {
            selected = &LESSON_DEMOS[i];
            break;
        }
    }

    if(args->scenario == SCENARIO_SIGNED_CONVERSION)
    {
        const int    parsed_count    = -1;
        const size_t converted_count = (size_t)parsed_count;

        p101_snprintf(env, err, dynamic_record, sizeof(dynamic_record), "severity=error event=count_parse parsed=%d converted=%zu outcome=accepted\n", parsed_count, converted_count);
        p101_printf(env, err, "signed-conversion: turns a negative count into a huge unsigned size\n");
        record = dynamic_record;
    }
    else if(args->scenario == SCENARIO_TRUNCATION)
    {
        const unsigned int requested_count = 70000U;
        const unsigned int stored_count    = (unsigned short)requested_count;

        p101_snprintf(env, err, dynamic_record, sizeof(dynamic_record), "severity=error event=count_store requested=%u stored=%u outcome=accepted\n", requested_count, stored_count);
        p101_printf(env, err, "truncation: stores a large count in a too-small integer type\n");
        record = dynamic_record;
    }
    else if(selected != NULL)
    {
        p101_printf(env, err, "%s", selected->description);
        record = selected->record;
    }
    else
    {
        P101_ERROR_RAISE_USER(err, "The scenario is not a modeled lesson demo.", ERR_USAGE);
    }

    if(record != NULL)
    {
        ret_val = p101_tool_playground_write_text_output(env, err, args, record);
    }

    return ret_val;
}

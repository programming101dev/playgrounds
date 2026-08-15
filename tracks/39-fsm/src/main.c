#include "track_info.h"
#include <p101_c/p101_stdio.h>
#include <p101_c/p101_stdlib.h>
#include <p101_env/env.h>
#include <p101_error/error.h>
#include <p101_fsm/fsm.h>
#include <stddef.h>
#include <stdlib.h>

static const char *const wrapper_functions[] = {"p101_fsm_decide_exit",
                                                "p101_fsm_decide_pause",
                                                "p101_fsm_decide_transition",
                                                "p101_fsm_effect_batch_count",
                                                "p101_fsm_effect_batch_create",
                                                "p101_fsm_effect_batch_destroy",
                                                "p101_fsm_effect_batch_finish_step",
                                                "p101_fsm_effect_batch_sink",
                                                "p101_fsm_emit_effect",
                                                "p101_fsm_exit_immediately",
                                                "p101_fsm_info_create",
                                                "p101_fsm_info_default_bad_change_state_handler",
                                                "p101_fsm_info_default_bad_change_state_notifier",
                                                "p101_fsm_info_default_did_change_state_notifier",
                                                "p101_fsm_info_default_will_change_state_notifier",
                                                "p101_fsm_info_destroy",
                                                "p101_fsm_info_get_bad_change_state_handler",
                                                "p101_fsm_info_get_bad_change_state_notifier",
                                                "p101_fsm_info_get_current_state",
                                                "p101_fsm_info_get_did_change_state_notifier",
                                                "p101_fsm_info_get_name",
                                                "p101_fsm_info_get_step_sequence",
                                                "p101_fsm_info_get_will_change_state_notifier",
                                                "p101_fsm_info_is_terminal",
                                                "p101_fsm_info_set_bad_change_state_handler",
                                                "p101_fsm_info_set_bad_change_state_notifier",
                                                "p101_fsm_info_set_did_change_state_notifier",
                                                "p101_fsm_info_set_step_observer",
                                                "p101_fsm_info_set_will_change_state_notifier",
                                                "p101_fsm_run",
                                                "p101_fsm_step"};

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

static void finish_state(const struct p101_env *env, struct p101_error *err, void *arg, struct p101_fsm_effect_sink *sink, struct p101_fsm_decision *decision)
{
    (void)env;
    (void)err;
    (void)arg;
    (void)sink;
    p101_fsm_decide_exit(decision);
}

static int run_fsm_step_demo(const struct p101_env *env, struct p101_error *err)
{
    struct p101_error                      *fsm_err;
    struct p101_env                        *fsm_env;
    struct p101_fsm_info                   *fsm;
    struct p101_fsm_step_result             result;
    p101_fsm_step_status                    status;
    int                                     ret_val;
    static const struct p101_fsm_transition transitions[] = {
        {P101_FSM_INIT, P101_FSM_USER_START, finish_state},
    };

    ret_val = EXIT_FAILURE;
    fsm     = NULL;
    fsm_env = NULL;
    fsm_err = p101_error_create(false);
    if(fsm_err == NULL)
    {
        goto done;
    }
    fsm_env = p101_env_create(fsm_err, NULL);
    if(fsm_env == NULL || p101_error_has_error(fsm_err))
    {
        goto done;
    }
    fsm = p101_fsm_info_create(env, err, "teaching-fsm", fsm_env, fsm_err, transitions, sizeof(transitions) / sizeof(transitions[0]), NULL);
    if(fsm == NULL || p101_error_has_error(err) || p101_error_has_error(fsm_err))
    {
        goto done;
    }
    status = p101_fsm_step(fsm, NULL, NULL, &result);
    if(status != P101_FSM_STEP_EXITED || result.sequence != 1U || !p101_fsm_info_is_terminal(fsm))
    {
        goto done;
    }
    if(p101_fprintf(env, err, stdout, "FSM %s exited at step %zu\n", p101_fsm_info_get_name(env, fsm), result.sequence) < 0 || p101_error_has_error(err))
    {
        goto done;
    }
    ret_val = EXIT_SUCCESS;

done:
    p101_fsm_info_destroy(env, fsm_err, &fsm);
    p101_env_destroy(fsm_env);
    p101_error_destroy(fsm_err);
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

    if(run_fsm_step_demo(env, err) != EXIT_SUCCESS)
    {
        goto done;
    }

    ret_val = EXIT_SUCCESS;

done:
    p101_env_destroy(env);
    p101_error_destroy(err);
    return ret_val;
}

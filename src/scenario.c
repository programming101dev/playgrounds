#include "scenario.h"
#include <p101_c/p101_stdio.h>
#include <p101_c/p101_string.h>
#include <stddef.h>

enum playground_scenario_behavior
{
    P101_SCENARIO_EXECUTABLE_CLEAN,
    P101_SCENARIO_EXECUTABLE_DEFECT,
    P101_SCENARIO_MODELED_DEFECT
};

struct scenario_definition
{
    const char                       *name;
    const char                       *description;
    enum playground_scenario          scenario;
    enum playground_scenario_behavior behavior;
};

static const struct scenario_definition SCENARIOS[] = {
#define P101_SCENARIO(identifier, name, behavior, description) {name, description, identifier, behavior},
#include "playground_scenarios.def"
#undef P101_SCENARIO
};

static bool scenario_is_valid(enum playground_scenario scenario)
{
    return (scenario >= SCENARIO_TOUR && scenario < SCENARIO_COUNT) != 0;
}

enum playground_scenario p101_tool_playground_scenario_from_name(const struct p101_env *env, const char *name, bool *ok)
{
    int p101_int_result_1;

    enum playground_scenario scenario;
    size_t                   index;

    *ok      = false;
    scenario = SCENARIO_TOUR;

    for(index = 0U; index < (size_t)SCENARIO_COUNT; index++)
    {
        p101_int_result_1 = p101_strcmp(env, name, SCENARIOS[index].name);
        if(p101_int_result_1 == 0)
        {
            scenario = SCENARIOS[index].scenario;
            *ok      = true;
            break;
        }
    }

    return scenario;
}

const char *p101_tool_playground_scenario_name(enum playground_scenario scenario)
{
    bool p101_bool_result_1;

    const char *name;

    name               = "unknown";
    p101_bool_result_1 = scenario_is_valid(scenario);
    if(p101_bool_result_1)
    {
        name = SCENARIOS[(size_t)scenario].name;
    }

    return name;
}

void p101_tool_playground_print_scenarios(const struct p101_env *env, struct p101_error *err, FILE *stream)
{
    bool   p101_bool_result_1;
    size_t index;

    p101_fputs(env, err, "Scenarios (executable-clean, executable-defect, or modeled-defect):\n", stream);
    for(index = 0U; index < (size_t)SCENARIO_COUNT; index++)
    {
        const char *behavior;

        p101_bool_result_1 = p101_error_has_no_error(err);
        if(!p101_bool_result_1)
        {
            break;
        }

#ifdef __clang__
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wcovered-switch-default"
#endif
        switch(SCENARIOS[index].behavior)
        {
            case P101_SCENARIO_EXECUTABLE_CLEAN:
            {
                behavior = "executable-clean";
                break;
            }
            case P101_SCENARIO_EXECUTABLE_DEFECT:
            {
                behavior = "executable-defect";
                break;
            }
            case P101_SCENARIO_MODELED_DEFECT:
            {
                behavior = "modeled-defect";
                break;
            }
            default:
            {
                behavior = "unknown";
                break;
            }
        }
#ifdef __clang__
    #pragma clang diagnostic pop
#endif

        p101_fprintf(env, err, stream, "  %-27s %-17s %s\n", SCENARIOS[index].name, behavior, SCENARIOS[index].description);
    }
}

#include "playground_support.h"
#include "constants.h"
#include <fcntl.h>
#include <p101_c/p101_stdlib.h>
#include <p101_c/p101_string.h>
#include <p101_io/p101_fcntl.h>
#include <p101_io/p101_unistd.h>
#include <p101_memory/p101_stdlib.h>
#include <stdlib.h>

void p101_tool_playground_support_close_fd_preserving_error(const struct p101_env *env, struct p101_error *err, int *fd)
{
    bool p101_bool_result_1;

    P101_TRACE_SCOPE(env);
    if(fd != NULL && *fd != -1)
    {
        p101_bool_result_1 = p101_error_has_no_error(err);
        if(p101_bool_result_1)
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

int p101_tool_playground_support_write_demo_file(const struct p101_env *env, struct p101_error *err, const struct arguments *args, const char *label, bool leak_fd, bool leak_alloc)
{
    bool p101_bool_result_1;

    int   fd;
    char *buffer;
    int   ret_val;

    P101_TRACE_SCOPE(env);
    buffer  = NULL;
    ret_val = EXIT_FAILURE;

    fd                 = p101_open(env, err, args->output_path, O_WRONLY | O_CREAT | O_TRUNC, REPORT_FILE_MODE);
    p101_bool_result_1 = p101_error_has_error(err);
    if(p101_bool_result_1)
    {
        goto done;
    }

    buffer             = p101_tool_playground_support_make_buffer(env, err, args->bytes, label[0]);
    p101_bool_result_1 = p101_error_has_error(err);
    if(p101_bool_result_1)
    {
        goto done;
    }

    for(unsigned int i = 0; i < args->repeats; i++)
    {
        p101_bool_result_1 = p101_error_has_no_error(err);
        if(!p101_bool_result_1)
        {
            break;
        }

        p101_write(env, err, fd, buffer, args->bytes);
    }

    p101_bool_result_1 = p101_error_has_no_error(err);
    if(p101_bool_result_1)
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

char *p101_tool_playground_support_make_buffer(const struct p101_env *env, struct p101_error *err, size_t bytes, char fill)
{
    void *p101_void_pointer_result_1;

    char *buffer;

    P101_TRACE_SCOPE(env);
    p101_void_pointer_result_1 = p101_malloc(env, err, bytes);
    buffer                     = (char *)p101_void_pointer_result_1;

    if(buffer != NULL)
    {
        p101_memset(env, buffer, fill, bytes);
    }

    return buffer;
}

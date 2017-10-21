/* ********************************************************************** */
/**
 * @brief   ESM: handler - common implementation (sample && test application).
 * @author  eel3
 * @date    2017-10-19
 */
/* ********************************************************************** */

#include "handler_public.h"
#include "handler_private.h"

#include "event_id.h"

#include <stdio.h>

/* ---------------------------------------------------------------------- */
/* File scope variables */
/* ---------------------------------------------------------------------- */

static char global_timer_prefix[] = "global";

/* ---------------------------------------------------------------------- */
/* Functions: for event handler */
/* ---------------------------------------------------------------------- */

void
event_handler_on_init(void * const user_data)
{
    const char * const s = (const char *) user_data;

    (void) printf("%s->event_handler->on_init();\n", s);
}

void
event_handler_on_event(void * const user_data, const ESM_EVENT_ID id,
                       const ESM_EVENT_HANDLER * const next_handler)
{
    const char * const s = (const char *) user_data;
    EVENT_BIT bits = (EVENT_BIT) id;

    (void) printf("%s->event_handler->on_event(0x%08lX);\n", s, (unsigned long) id);

    switch (bits & EVENT_BITMASK_CMD) {
    case EVENT_BIT_CMD_NEXT_HANDLER:
        (void) esm_SetNextEventHandler(next_handler);
        break;
    case EVENT_BIT_CMD_SET_TIMER:
        (void) esm_SetTimer((ESM_TIMER_ID) ((bits & EVENT_BITMASK_TIMER_ID) >> 16),
                            (ESM_SYS_TICK_MSEC) (bits & EVENT_BITMASK_TIMER_TIMEOUT),
                            (bits & EVENT_BITMASK_TIMER_REPEAT) != 0 ? true : false);
        break;
    case EVENT_BIT_CMD_KILL_TIMER:
        (void) esm_KillTimer((ESM_TIMER_ID) ((bits & EVENT_BITMASK_TIMER_ID) >> 16));
        break;
    case EVENT_BIT_CMD_SET_GTIMER:
        (void) esm_SetGlobalTimer((ESM_TIMER_ID) ((bits & EVENT_BITMASK_TIMER_ID) >> 16),
                                  (ESM_SYS_TICK_MSEC) (bits & EVENT_BITMASK_TIMER_TIMEOUT),
                                  (bits & EVENT_BITMASK_TIMER_REPEAT) != 0 ? true : false,
                                  &global_timer_handler);
        break;
    case EVENT_BIT_CMD_KILL_GTIMER:
        (void) esm_KillGlobalTimer((ESM_TIMER_ID) ((bits & EVENT_BITMASK_TIMER_ID) >> 16));
        break;
    case EVENT_BIT_CMD_POST_MESSAGE: {
            ESM_MESSAGE msg = default_message;
            msg.user_data = user_data;
            (void) esm_PostMessage(&msg);
        }
        break;
    default:
        break;
    }
}

void
event_handler_on_timer(void * const user_data, const ESM_TIMER_ID id)
{
    const char * const s = (const char *) user_data;

    (void) printf("%s->event_handler->on_timer(%lu);\n", s, (unsigned long) id);
}

void
event_handler_on_destroy(void * const user_data)
{
    const char * const s = (const char *) user_data;

    (void) printf("%s->event_handler->on_destroy();\n", s);
}

void
event_handler_release_user_data(void * const user_data)
{
    const char * const s = (const char *) user_data;

    (void) printf("%s->event_handler->release_user_data();\n", s);
}

/* ---------------------------------------------------------------------- */
/* Private functions: for timer handler */
/* ---------------------------------------------------------------------- */

void
timer_handler_func(void * const user_data)
{
    const char * const s = (const char *) user_data;

    (void) printf("%s->timer_handler->func();\n", s);
}

void
timer_handler_release_user_data(void * const user_data)
{
    const char * const s = (const char *) user_data;

    (void) printf("%s->timer_handler->release_user_data();\n", s);
}

/* ---------------------------------------------------------------------- */
/* Private functions: for message */
/* ---------------------------------------------------------------------- */

void
message_func(void * const user_data)
{
    const char * const s = (const char *) user_data;

    (void) printf("%s->message->func();\n", s);
}

void
message_release_user_data(void * const user_data)
{
    const char * const s = (const char *) user_data;

    (void) printf("%s->message->release_user_data();\n", s);
}

/* ---------------------------------------------------------------------- */
/* Global variables */
/* ---------------------------------------------------------------------- */

const ESM_TIMER_HANDLER global_timer_handler = {
    timer_handler_func,
    timer_handler_release_user_data,
    (void *) global_timer_prefix,
};

const ESM_MESSAGE default_message = {
    message_func,
    message_release_user_data,
    NULL,
};

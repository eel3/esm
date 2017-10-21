/* ********************************************************************** */
/**
 * @brief   ESM: state 2 event handler implementation (sample && test application).
 * @author  eel3
 * @date    2017-10-19
 */
/* ********************************************************************** */

#include "handler_public.h"
#include "handler_private.h"

/* ---------------------------------------------------------------------- */
/* File scope variables */
/* ---------------------------------------------------------------------- */

static char handler_name_prefix[] = "state_2";

/* ---------------------------------------------------------------------- */
/* Private functions: for event handler */
/* ---------------------------------------------------------------------- */

static void
on_event(void * const user_data, const ESM_EVENT_ID id)
{
    event_handler_on_event(user_data, id, &state_3_event_handler);
}

/* ---------------------------------------------------------------------- */
/* Global variables */
/* ---------------------------------------------------------------------- */

const ESM_EVENT_HANDLER state_2_event_handler = {
    event_handler_on_init,
    on_event,
    event_handler_on_timer,
    event_handler_on_destroy,
    event_handler_release_user_data,
    (void *) handler_name_prefix,
};

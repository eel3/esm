/* ********************************************************************** */
/**
 * @brief   ESM: handler - private interfaces (sample && test application).
 * @author  eel3
 * @date    2017-10-19
 */
/* ********************************************************************** */

#ifndef HANDLER_PRIVATE_H_INCLUDED
#define HANDLER_PRIVATE_H_INCLUDED

#include "esm.h"

/* ---------------------------------------------------------------------- */
/* Global variables */
/* ---------------------------------------------------------------------- */

extern const ESM_EVENT_HANDLER state_2_event_handler;
extern const ESM_EVENT_HANDLER state_3_event_handler;
extern const ESM_TIMER_HANDLER global_timer_handler;

/* ---------------------------------------------------------------------- */
/* Functions */
/* ---------------------------------------------------------------------- */

extern void
event_handler_on_init(void * const user_data);
extern void
event_handler_on_event(void * const user_data, const ESM_EVENT_ID id,
                       const ESM_EVENT_HANDLER * const next_handler);
extern void
event_handler_on_timer(void * const user_data, const ESM_TIMER_ID id);
extern void
event_handler_on_destroy(void * const user_data);
extern void
event_handler_release_user_data(void * const user_data);

extern void
timer_handler_func(void * const user_data);
extern void
timer_handler_release_user_data(void * const user_data);

extern void
message_func(void * const user_data);
extern void
message_release_user_data(void * const user_data);

#endif /* ndef HANDLER_PRIVATE_H_INCLUDED */

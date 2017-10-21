/* ********************************************************************** */
/**
 * @brief   ESM: public API implementation.
 * @author  eel3
 * @date    2017-10-18
 */
/* ********************************************************************** */

#include "esm.h"
#include "esm_md.h"

#include <stddef.h>

#ifdef ESM_CFG_USE_ASSERT_H
#include <assert.h>
#else
#define assert(cond)
#endif

/* ---------------------------------------------------------------------- */
/* Data structures */
/* ---------------------------------------------------------------------- */

/** Timer cell type. */
typedef struct {
    ESM_SYS_TICK_MSEC timeout_msec;
    ESM_SYS_TICK_MSEC expire_time_msec;
    bool expired;
    bool repeat;
} ESM_TIMER_CELL;

/** Timer handler cell type. */
typedef struct {
    ESM_SYS_TICK_MSEC timeout_msec;
    ESM_SYS_TICK_MSEC expire_time_msec;
    bool expired;
    bool repeat;
    ESM_TIMER_HANDLER handler;
} ESM_TIMER_HANDLER_CELL;

/** Module context type. */
typedef struct {
    bool initialized;
    bool prepared;

    /* Message queue. */
    ESM_MESSAGE_CELL *first_message_cell;
    ESM_MESSAGE_CELL *last_message_cell;

    /* Event handlers. */
    ESM_EVENT_HANDLER event_handler;
    ESM_EVENT_HANDLER next_event_handler;

    /* Timer handlers */
    ESM_TIMER_CELL timers[ESM_CFG_MAX_TIMER];

    /* Global timer's handlers */
    ESM_TIMER_HANDLER_CELL global_timers[ESM_CFG_MAX_GLOBAL_TIMER];
} MODULE_CTX;

/* ---------------------------------------------------------------------- */
/* File scope variables */
/* ---------------------------------------------------------------------- */

/** Module context. */
static MODULE_CTX module_ctx;

/* ---------------------------------------------------------------------- */
/* Function-like macros */
/* ---------------------------------------------------------------------- */

/* ====================================================================== */
/**
 * @brief  Return the maximum number of elements.
 *
 * @param[in] array  An array.
 *
 * @return  Maximum number of elements.
 */
/* ====================================================================== */
#define NELEMS(array) (sizeof(array) / sizeof((array)[0]))

/* ---------------------------------------------------------------------- */
/* Private functions: dummy callback functions */
/* ---------------------------------------------------------------------- */

static void
dummy_on_init(void * const user_data)
{
    (void) user_data;
}

static void
dummy_on_event(void * const user_data, const ESM_EVENT_ID id)
{
    (void) user_data, (void) id;
}

static void
dummy_on_timer(void * const user_data, const ESM_TIMER_ID id)
{
    (void) user_data, (void) id;
}

static void
dummy_on_destroy(void * const user_data)
{
    (void) user_data;
}

static void
dummy_release_user_data(void * const user_data)
{
    (void) user_data;
}

/* ---------------------------------------------------------------------- */
/* Private functions: for data structures */
/* ---------------------------------------------------------------------- */

/* ====================================================================== */
/**
 * @brief  Sanitize ESM_EVENT_HANDLER members.
 *
 * @param[in,out] handler  Event handler.
 */
/* ====================================================================== */
static void
eeh_Sanitize(ESM_EVENT_HANDLER * const handler)
{
#define SANITIZE_FUNC(func) \
    if (handler->func == NULL) handler->func = dummy_##func

    assert(handler != NULL);

    SANITIZE_FUNC(on_init);
    SANITIZE_FUNC(on_event);
    SANITIZE_FUNC(on_timer);
    SANITIZE_FUNC(on_destroy);
    SANITIZE_FUNC(release_user_data);

#undef SANITIZE_FUNC
}

/* ====================================================================== */
/**
 * @brief  Cleanup ESM_EVENT_HANDLER members.
 *
 * @param[out] handler  Event handler.
 */
/* ====================================================================== */
static void
eeh_Cleanup(ESM_EVENT_HANDLER * const handler)
{
    assert(handler != NULL);

    handler->on_init = NULL;
    handler->on_event = NULL;
    handler->on_timer = NULL;
    handler->on_destroy = NULL;
    handler->release_user_data = NULL;
    handler->user_data = NULL;
}

/* ====================================================================== */
/**
 * @brief  Sanitize ESM_GENERIC_HANDLER members.
 *
 * @param[in,out] handler  Generic handler.
 */
/* ====================================================================== */
static void
egh_Sanitize(ESM_GENERIC_HANDLER * const handler)
{
#define SANITIZE_FUNC(func) \
    if (handler->func == NULL) handler->func = dummy_##func

    assert(handler != NULL);

    SANITIZE_FUNC(release_user_data);

#undef SANITIZE_FUNC
}

/* ====================================================================== */
/**
 * @brief  Sanitize ESM_TIMER_HANDLER members.
 *
 * @param[in,out] handler  Timer handler.
 */
/* ====================================================================== */
#define eth_Sanitize(handler) egh_Sanitize((ESM_GENERIC_HANDLER *) (handler))

/* ====================================================================== */
/**
 * @brief  Sanitize ESM_MESSAGE members.
 *
 * @param[in,out] msg  Message.
 */
/* ====================================================================== */
#define em_Sanitize(msg) egh_Sanitize((ESM_GENERIC_HANDLER *) (msg))

/* ====================================================================== */
/**
 * @brief  Cleanup ESM_GENERIC_HANDLER members.
 *
 * @param[out] handler  Generic handler.
 */
/* ====================================================================== */
static void
egh_Cleanup(ESM_GENERIC_HANDLER * const handler)
{
    assert(handler != NULL);

    handler->func = NULL;
    handler->release_user_data = NULL;
    handler->user_data = NULL;
}

/* ====================================================================== */
/**
 * @brief  Cleanup ESM_TIMER_HANDLER members.
 *
 * @param[out] handler  Timer handler.
 */
/* ====================================================================== */
#define eth_Cleanup(handler) egh_Cleanup((ESM_GENERIC_HANDLER *) (handler))

/* ====================================================================== */
/**
 * @brief  Cleanup ESM_MESSAGE members.
 *
 * @param[out] msg  Message.
 */
/* ====================================================================== */
#define em_Cleanup(msg) egh_Cleanup((ESM_GENERIC_HANDLER *) (msg))

/* ====================================================================== */
/**
 * @brief  Initialize ESM_TIMER_CELL members.
 *
 * @param[out] cell  Timer cell.
 */
/* ====================================================================== */
static void
etc_Initialize(ESM_TIMER_CELL * const cell)
{
    assert(cell != NULL);

    cell->timeout_msec = 0;
    cell->expire_time_msec = 0;
    cell->expired = true;
    cell->repeat = false;
}

/* ====================================================================== */
/**
 * @brief  Initialize ESM_TIMER_HANDLER_CELL members.
 *
 * @param[out] cell  Timer handler cell.
 */
/* ====================================================================== */
static void
ethc_Initialize(ESM_TIMER_HANDLER_CELL * const cell)
{
    assert(cell != NULL);

    cell->timeout_msec = 0;
    cell->expire_time_msec = 0;
    cell->expired = true;
    cell->repeat = false;
    eth_Cleanup(&cell->handler);
}

/* ====================================================================== */
/**
 * @brief  Create a ESM_MESSAGE_CELL object.
 *
 * @param[in] msg  Message.
 *
 * @retval !=NULL  Exit success.
 * @retval   NULL  Exit failure.
 */
/* ====================================================================== */
static ESM_MESSAGE_CELL *
emc_Create(const ESM_MESSAGE * const msg)
{
    ESM_MESSAGE_CELL *cell;

    assert(msg != NULL);

    cell = esm_md_AllocMessageCell();
    if (cell == NULL) {
        return NULL;
    }

    cell->next = NULL;
    cell->message = *msg;
    em_Sanitize(&cell->message);

    return cell;
}

/* ====================================================================== */
/**
 * @brief  Delete the ESM_MESSAGE_CELL object.
 *
 * @param[in,out] cell  Message cell.
 */
/* ====================================================================== */
static void
emc_Delete(ESM_MESSAGE_CELL * const cell)
{
    assert(cell != NULL);

    cell->next = NULL;
    em_Cleanup(&cell->message);

    esm_md_DeallocMessageCell(cell);
}

/* ---------------------------------------------------------------------- */
/* Private functions: process event handler */
/* ---------------------------------------------------------------------- */

static void
force_stop_timers(MODULE_CTX * const mc);

/* ====================================================================== */
/**
 * @brief  Set the default event handler.
 *
 * @param[in,out] mc               Module context.
 * @param[in]     default_handler  Event handler.
 */
/* ====================================================================== */
static void
set_default_event_handler(MODULE_CTX * const mc,
                          const ESM_EVENT_HANDLER * const default_handler)
{
    ESM_EVENT_HANDLER *handler;

    assert((mc != NULL) && (default_handler != NULL));

    handler = &mc->event_handler;
    *handler = *default_handler;
    eeh_Sanitize(handler);

    handler = &mc->next_event_handler;
    eeh_Cleanup(handler);
}

/* ====================================================================== */
/**
 * @brief  Set the next event handler.
 *
 * @param[in,out] mc            Module context.
 * @param[in]     next_handler  Event handler.
 */
/* ====================================================================== */
static void
set_next_event_handler(MODULE_CTX * const mc,
                       const ESM_EVENT_HANDLER * const next_handler)
{
    ESM_EVENT_HANDLER *handler;

    assert((mc != NULL) && (handler != NULL));

    handler = &mc->next_event_handler;
    *handler = *next_handler;
    eeh_Sanitize(handler);
}

/* ====================================================================== */
/**
 * @brief  Update event handler (apply next handler).
 *
 * @param[in,out] mc  Module context.
 */
/* ====================================================================== */
static void
update_event_handler(MODULE_CTX * const mc)
{
    ESM_EVENT_HANDLER *next_handler, *handler;

    assert(mc != NULL);

    next_handler = &mc->next_event_handler;
    if (next_handler->on_init == NULL) {
        /* No need to update. */
        return;
    }

    force_stop_timers(mc);

    handler = &mc->event_handler;
    handler->on_destroy(handler->user_data);
    handler->release_user_data(handler->user_data);

    *handler = *next_handler;
    eeh_Cleanup(next_handler);

    handler->on_init(handler->user_data);
}

/* ====================================================================== */
/**
 * @brief  Remove event handler.
 *
 * @param[in,out] mc  Module context.
 */
/* ====================================================================== */
static void
remove_event_handler(MODULE_CTX * const mc)
{
    ESM_EVENT_HANDLER *handler;

    assert(mc != NULL);

    handler = &mc->event_handler;
    handler->on_destroy(handler->user_data);
    handler->release_user_data(handler->user_data);
    eeh_Cleanup(handler);

    handler = &mc->next_event_handler;
    if (handler->on_init != NULL) {
        handler->release_user_data(handler->user_data);
        eeh_Cleanup(handler);
    }
}

/* ---------------------------------------------------------------------- */
/* Private functions: process event */
/* ---------------------------------------------------------------------- */

/* ====================================================================== */
/**
 * @brief  Process a event.
 *
 * @param[in,out] mc  Module context.
 */
/* ====================================================================== */
static void
process_event(MODULE_CTX * const mc)
{
    ESM_EVENT_ID id;
    ESM_EVENT_HANDLER *handler;

    assert(mc != NULL);

    id = esm_md_PeekEvent();
    if (id == ESM_EVENT_ID_NONE) {
        return;
    }

    handler = &mc->event_handler;
    handler->on_event(handler->user_data, id);

    update_event_handler(mc);
}

/* ---------------------------------------------------------------------- */
/* Private functions: process timer */
/* ---------------------------------------------------------------------- */

/* ====================================================================== */
/**
 * @brief  Validate timer id.
 *
 * @param[in] mc  Module context.
 * @param[in] id  Timer ID.
 *
 * @retval true   Valid.
 * @retval false  Invalid.
 */
/* ====================================================================== */
static bool
valid_timer_id(const MODULE_CTX * const mc, const ESM_TIMER_ID id)
{
    assert(mc != NULL);

    return (size_t) id < NELEMS(mc->timers);
}

/* ====================================================================== */
/**
 * @brief  Initialize global software timers.
 *
 * @param[in,out] mc  Module context.
 */
/* ====================================================================== */
static void
initialize_timers(MODULE_CTX * const mc)
{
    size_t i;

    assert(mc != NULL);

    for (i = 0; i < NELEMS(mc->timers); i++) {
        etc_Initialize(&mc->timers[i]);
    }
}

/* ====================================================================== */
/**
 * @brief  Create and start the software timer.
 *
 * @param[in,out] mc            Module context.
 * @param[in]     id            Timer ID.
 * @param[in]     timeout_msec  Timeout value (in milliseconds).
 * @param[in]     repeat        Repeatedly reschedule or not.
 *
 * @retval ESM_E_OK      Exit success.
 * @retval ESM_E_STATUS  Internal status error.
 */
/* ====================================================================== */
static ESM_ERR
set_timer(MODULE_CTX * const mc,
          const ESM_TIMER_ID id,
          const ESM_SYS_TICK_MSEC timeout_msec,
          const bool repeat)
{
    ESM_TIMER_CELL *cell;

    assert((mc != NULL) && valid_timer_id(mc, id));

    cell = &mc->timers[id];
    if (!cell->expired) {
        return ESM_E_STATUS;
    }

    cell->timeout_msec = timeout_msec;
    cell->expire_time_msec = esm_md_GetTick() + timeout_msec;
    cell->expired = false;
    cell->repeat = repeat;

    return ESM_E_OK;
}

/* ====================================================================== */
/**
 * @brief  Stop and delete the software timer.
 *
 * @param[in,out] mc  Module context.
 * @param[in]     id  Timer ID.
 */
/* ====================================================================== */
static void
kill_timer(MODULE_CTX * const mc, const ESM_TIMER_ID id)
{
    assert((mc != NULL) && valid_timer_id(mc, id));

    mc->timers[id].expired = true;
}

/* ====================================================================== */
/**
 * @brief  Process software timers.
 *
 * @param[in,out] mc  Module context.
 */
/* ====================================================================== */
static void
process_timers(MODULE_CTX * const mc)
{
    ESM_SYS_TICK_MSEC current_time;
    ESM_EVENT_HANDLER *handler;
    size_t i;

    assert(mc != NULL);

    current_time = esm_md_GetTick();
    handler = &mc->event_handler;

    for (i = 0; i < NELEMS(mc->timers); i++) {
        ESM_TIMER_CELL *cell;

        cell = &mc->timers[i];
        if (cell->expired) {
            continue;
        }
        if ((current_time - cell->expire_time_msec) < 0) {
            continue;
        }
        if (cell->repeat) {
            cell->expire_time_msec += cell->timeout_msec;
        } else {
            cell->expired = true;
        }

        handler->on_timer(handler->user_data, (ESM_TIMER_ID) i);

        update_event_handler(mc);
    }
}

/* ====================================================================== */
/**
 * @brief  Force stop software timers.
 *
 * @param[in,out] mc  Module context.
 */
/* ====================================================================== */
static void
force_stop_timers(MODULE_CTX * const mc)
{
    size_t i;

    assert(mc != NULL);

    for (i = 0; i < NELEMS(mc->timers); i++) {
        mc->timers[i].expired = true;
    }
}

/* ---------------------------------------------------------------------- */
/* Private functions: process global timer */
/* ---------------------------------------------------------------------- */

/* ====================================================================== */
/**
 * @brief  Validate global timer id.
 *
 * @param[in] mc  Module context.
 * @param[in] id  Timer ID.
 *
 * @retval true   Valid.
 * @retval false  Invalid.
 */
/* ====================================================================== */
static bool
valid_global_timer_id(const MODULE_CTX * const mc, const ESM_TIMER_ID id)
{
    assert(mc != NULL);

    return (size_t) id < NELEMS(mc->global_timers);
}

/* ====================================================================== */
/**
 * @brief  Initialize global software timers.
 *
 * @param[in,out] mc  Module context.
 */
/* ====================================================================== */
static void
initialize_global_timers(MODULE_CTX * const mc)
{
    size_t i;

    assert(mc != NULL);

    for (i = 0; i < NELEMS(mc->global_timers); i++) {
        ethc_Initialize(&mc->global_timers[i]);
    }
}

/* ====================================================================== */
/**
 * @brief  Create and start the global software timer.
 *
 * @param[in,out] mc            Module context.
 * @param[in]     id            Timer ID.
 * @param[in]     timeout_msec  Timeout value (in milliseconds).
 * @param[in]     repeat        Repeatedly reschedule or not.
 * @param[in]     handler       Timer handler.
 *
 * @retval ESM_E_OK      Exit success.
 * @retval ESM_E_PRM     Parameter error (perhaps arguments error).
 * @retval ESM_E_STATUS  Internal status error.
 */
/* ====================================================================== */
static ESM_ERR
set_global_timer(MODULE_CTX * const mc,
                 const ESM_TIMER_ID id,
                 const ESM_SYS_TICK_MSEC timeout_msec,
                 const bool repeat,
                 const ESM_TIMER_HANDLER * const handler)
{
    ESM_TIMER_HANDLER_CELL *cell;

    assert((mc != NULL) && valid_global_timer_id(mc, id) && (handler != NULL));

    if (handler->func == NULL) {
        return ESM_E_PRM;
    }

    cell = &mc->global_timers[id];
    if (!cell->expired) {
        return ESM_E_STATUS;
    }

    cell->timeout_msec = timeout_msec;
    cell->expire_time_msec = esm_md_GetTick() + timeout_msec;
    cell->expired = false;
    cell->repeat = repeat;
    cell->handler = *handler;
    eth_Sanitize(&cell->handler);

    return ESM_E_OK;
}

/* ====================================================================== */
/**
 * @brief  Stop and delete the global software timer.
 *
 * @param[in,out] mc  Module context.
 * @param[in]     id  Timer ID.
 */
/* ====================================================================== */
static void
kill_global_timer(MODULE_CTX * const mc, const ESM_TIMER_ID id)
{
    ESM_TIMER_HANDLER_CELL *cell;
    ESM_TIMER_HANDLER *handler;

    assert((mc != NULL) && valid_global_timer_id(mc, id));

    cell = &mc->global_timers[id];
    if (cell->expired) {
        return;
    }
    cell->expired = true;
    handler = &cell->handler;
    handler->release_user_data(handler->user_data);
}

/* ====================================================================== */
/**
 * @brief  Process global software timers.
 *
 * @param[in,out] mc  Module context.
 */
/* ====================================================================== */
static void
process_global_timers(MODULE_CTX * const mc)
{
    ESM_SYS_TICK_MSEC current_time;
    size_t i;

    assert(mc != NULL);

    current_time = esm_md_GetTick();

    for (i = 0; i < NELEMS(mc->global_timers); i++) {
        ESM_TIMER_HANDLER_CELL *cell;
        ESM_TIMER_HANDLER *handler;

        cell = &mc->global_timers[i];
        if (cell->expired) {
            continue;
        }
        if ((current_time - cell->expire_time_msec) < 0) {
            continue;
        }

        handler = &cell->handler;
        handler->func(handler->user_data);

        if (cell->repeat) {
            cell->expire_time_msec += cell->timeout_msec;
        } else {
            cell->expired = true;
            handler->release_user_data(handler->user_data);
        }

        update_event_handler(mc);
    }
}

/* ====================================================================== */
/**
 * @brief  Force stop global software timers.
 *
 * @param[in,out] mc  Module context.
 */
/* ====================================================================== */
static void
force_stop_global_timers(MODULE_CTX * const mc)
{
    size_t i;

    assert(mc != NULL);

    for (i = 0; i < NELEMS(mc->global_timers); i++) {
        ESM_TIMER_HANDLER_CELL *cell;

        cell = &mc->global_timers[i];
        if (!cell->expired) {
            ESM_TIMER_HANDLER *handler;

            handler = &cell->handler;
            handler->release_user_data(handler->user_data);
        }
        ethc_Initialize(cell);
    }
}

/* ---------------------------------------------------------------------- */
/* Private functions: process message */
/* ---------------------------------------------------------------------- */

/* ====================================================================== */
/**
 * @brief  Post the message to the mein loop.
 *
 * @param[in,out] mc   Module context.
 * @param[in]     msg  Message.
 *
 * @retval ESM_E_OK   Exit success.
 * @retval ESM_E_PRM  Parameter error (perhaps arguments error).
 * @retval ESM_E_RES  No system resources.
 */
/* ====================================================================== */
static ESM_ERR
post_message(MODULE_CTX * const mc, const ESM_MESSAGE * const msg)
{
    ESM_MESSAGE_CELL *cell;

    assert((mc != NULL) && (msg != NULL));

    if (msg->func == NULL) {
        return ESM_E_PRM;
    }

    cell = emc_Create(msg);
    if (cell == NULL) {
        return ESM_E_RES;
    }

    if (mc->first_message_cell == NULL) {
        mc->first_message_cell = cell;
        mc->last_message_cell = cell;
    } else {
        mc->last_message_cell->next = cell;
        mc->last_message_cell = cell;
    }

    return ESM_E_OK;
}

/* ====================================================================== */
/**
 * @brief  Process all messages.
 *
 * @param[in,out] mc  Module context.
 */
/* ====================================================================== */
static void
process_messages(MODULE_CTX * const mc)
{
    ESM_MESSAGE_CELL *cell, *next_cell;
    ESM_MESSAGE *msg;

    assert(mc != NULL);

    esm_md_LockForAPI();
    cell = mc->first_message_cell;
    mc->first_message_cell = NULL;
    mc->last_message_cell = NULL;
    esm_md_UnlockForAPI();

    for (; cell != NULL; cell = next_cell) {
        next_cell = cell->next;
        msg = &cell->message;
        msg->func(msg->user_data);
        msg->release_user_data(msg->user_data);

        esm_md_LockForAPI();
        emc_Delete(cell);
        esm_md_UnlockForAPI();

        update_event_handler(mc);
    }
}

/* ---------------------------------------------------------------------- */
/* Public API functions */
/* ---------------------------------------------------------------------- */

/* ********************************************************************** */
/**
 * @brief  Initialize the library.
 *
 * @retval ESM_E_OK      Exit success.
 * @retval ESM_E_RES     No system resources.
 * @retval ESM_E_STATUS  Internal status error.
 * @retval ESM_E_SYS     Error caused by underlying library routines.
 */
/* ********************************************************************** */
ESM_ERR
esm_Initialize(void)
{
    MODULE_CTX * const mc = &module_ctx;
    ESM_ERR err;

    if (mc->initialized) {
        return ESM_E_STATUS;
    }

    err = esm_md_Initialize();
    if (err != ESM_E_OK) {
        return err;
    }

    mc->prepared = false;
    mc->first_message_cell = NULL;
    mc->last_message_cell = NULL;

    mc->initialized = true;

    return ESM_E_OK;
}

/* ********************************************************************** */
/**
 * @brief  Finalize the library.
 */
/* ********************************************************************** */
void
esm_Finalize(void)
{
    MODULE_CTX * const mc = &module_ctx;

    if (!mc->initialized) {
        return;
    }

    (void) esm_CleanupAfterMainLoop();
    esm_md_Finalize();

    mc->initialized = false;
}

/* ********************************************************************** */
/**
 * @brief  Prepare the library before main loop.
 *
 * @param[in] params  Preparation parameters.
 *
 * @retval ESM_E_OK      Exit success.
 * @retval ESM_E_PRM     Parameter error (perhaps arguments error).
 * @retval ESM_E_STATUS  Internal status error.
 */
/* ********************************************************************** */
ESM_ERR
esm_PrepareBeforeMainLoop(const ESM_PREPARE_PARAMS * const params)
{
    MODULE_CTX * const mc = &module_ctx;
    ESM_ERR err;
    ESM_EVENT_HANDLER *handler;

    if (params == NULL) {
        return ESM_E_PRM;
    }
    if (params->default_handler == NULL) {
        return ESM_E_PRM;
    }

    if (!mc->initialized) {
        return ESM_E_STATUS;
    }
    if (mc->prepared) {
        return ESM_E_STATUS;
    }

    err = esm_md_PrepareBeforeMainLoop();
    if (err != ESM_E_OK) {
        return err;
    }

    set_default_event_handler(mc, params->default_handler);
    initialize_timers(mc);
    initialize_global_timers(mc);

    mc->prepared = true;

    handler = &mc->event_handler;
    handler->on_init(handler->user_data);

    return ESM_E_OK;
}

/* ********************************************************************** */
/**
 * @brief  A resume-yield function for the main loop.
 *
 * @retval ESM_E_OK      Exit success.
 * @retval ESM_E_STATUS  Internal status error.
 */
/* ********************************************************************** */
ESM_ERR
esm_ResumeAndYield(void)
{
    MODULE_CTX * const mc = &module_ctx;

    if (!mc->initialized) {
        return ESM_E_STATUS;
    }
    if (!mc->prepared) {
        return ESM_E_STATUS;
    }

    update_event_handler(mc);

    process_event(mc);
    process_timers(mc);
    process_global_timers(mc);
    process_messages(mc);

    return ESM_E_OK;
}

/* ********************************************************************** */
/**
 * @brief  Cleanup the library after main loop.
 *
 * @retval ESM_E_OK      Exit success.
 * @retval ESM_E_STATUS  Internal status error.
 */
/* ********************************************************************** */
ESM_ERR
esm_CleanupAfterMainLoop(void)
{
    MODULE_CTX * const mc = &module_ctx;

    if (!mc->initialized) {
        return ESM_E_STATUS;
    }
    if (!mc->prepared) {
        return ESM_E_STATUS;
    }

    process_messages(mc);
    force_stop_global_timers(mc);
    remove_event_handler(mc);

    (void) esm_md_CleanupAfterMainLoop();

    mc->prepared = false;

    return ESM_E_OK;
}

/* ********************************************************************** */
/**
 * @brief  Set the next event handler.
 *
 * @param[in] handler  Event handler.
 *
 * @retval ESM_E_OK      Exit success.
 * @retval ESM_E_PRM     Parameter error (perhaps arguments error).
 * @retval ESM_E_STATUS  Internal status error.
 */
/* ********************************************************************** */
ESM_ERR
esm_SetNextEventHandler(const ESM_EVENT_HANDLER * const handler)
{
    MODULE_CTX * const mc = &module_ctx;

    if (handler == NULL) {
        return ESM_E_PRM;
    }

    if (!mc->initialized) {
        return ESM_E_STATUS;
    }
    if (!mc->prepared) {
        return ESM_E_STATUS;
    }

    set_next_event_handler(mc, handler);

    return ESM_E_OK;
}

/* ********************************************************************** */
/**
 * @brief  Create and start the software timer.
 *
 * @param[in] id            Timer ID.
 * @param[in] timeout_msec  Timeout value (in milliseconds).
 * @param[in] repeat        Repeatedly reschedule or not.
 *
 * @retval ESM_E_OK      Exit success.
 * @retval ESM_E_PRM     Parameter error (perhaps arguments error).
 * @retval ESM_E_STATUS  Internal status error.
 */
/* ********************************************************************** */
ESM_ERR
esm_SetTimer(const ESM_TIMER_ID id,
                const ESM_SYS_TICK_MSEC timeout_msec,
                const bool repeat)
{
    MODULE_CTX * const mc = &module_ctx;
    ESM_ERR err;

    if (!mc->initialized) {
        return ESM_E_STATUS;
    }
    if (!mc->prepared) {
        return ESM_E_STATUS;
    }

    if (!valid_timer_id(mc, id)) {
        return ESM_E_PRM;
    }

    err = set_timer(mc, id, timeout_msec, repeat);

    return err;
}

/* ********************************************************************** */
/**
 * @brief  Stop and delete the software timer.
 *
 * @param[in] id  Timer ID.
 *
 * @retval ESM_E_OK      Exit success.
 * @retval ESM_E_PRM     Parameter error (perhaps arguments error).
 * @retval ESM_E_STATUS  Internal status error.
 */
/* ********************************************************************** */
ESM_ERR
esm_KillTimer(const ESM_TIMER_ID id)
{
    MODULE_CTX * const mc = &module_ctx;

    if (!mc->initialized) {
        return ESM_E_STATUS;
    }
    if (!mc->prepared) {
        return ESM_E_STATUS;
    }

    if (!valid_timer_id(mc, id)) {
        return ESM_E_PRM;
    }

    kill_timer(mc, id);

    return ESM_E_OK;
}

/* ********************************************************************** */
/**
 * @brief  Create and start the global software timer.
 *
 * @param[in] id            Timer ID.
 * @param[in] timeout_msec  Timeout value (in milliseconds).
 * @param[in] repeat        Repeatedly reschedule or not.
 * @param[in] handler       Timer handler.
 *
 * @retval ESM_E_OK      Exit success.
 * @retval ESM_E_PRM     Parameter error (perhaps arguments error).
 * @retval ESM_E_STATUS  Internal status error.
 */
/* ********************************************************************** */
ESM_ERR
esm_SetGlobalTimer(const ESM_TIMER_ID id,
                      const ESM_SYS_TICK_MSEC timeout_msec,
                      const bool repeat,
                      const ESM_TIMER_HANDLER * const handler)
{
    MODULE_CTX * const mc = &module_ctx;
    ESM_ERR err;

    if (handler == NULL) {
        return ESM_E_PRM;
    }

    if (!mc->initialized) {
        return ESM_E_STATUS;
    }
    if (!mc->prepared) {
        return ESM_E_STATUS;
    }

    if (!valid_global_timer_id(mc, id)) {
        return ESM_E_PRM;
    }

    err = set_global_timer(mc, id, timeout_msec, repeat, handler);

    return err;
}

/* ********************************************************************** */
/**
 * @brief  Stop and delete the global software timer.
 *
 * @param[in] id  Timer ID.
 *
 * @retval ESM_E_OK      Exit success.
 * @retval ESM_E_PRM     Parameter error (perhaps arguments error).
 * @retval ESM_E_STATUS  Internal status error.
 */
/* ********************************************************************** */
ESM_ERR
esm_KillGlobalTimer(const ESM_TIMER_ID id)
{
    MODULE_CTX * const mc = &module_ctx;

    if (!mc->initialized) {
        return ESM_E_STATUS;
    }
    if (!mc->prepared) {
        return ESM_E_STATUS;
    }

    if (!valid_global_timer_id(mc, id)) {
        return ESM_E_PRM;
    }

    kill_global_timer(mc, id);

    return ESM_E_OK;
}

/* ********************************************************************** */
/**
 * @brief  Post the message to the mein loop.
 *
 * @param[in] msg  Message.
 *
 * @retval ESM_E_OK      Exit success.
 * @retval ESM_E_PRM     Parameter error (perhaps arguments error).
 * @retval ESM_E_RES     No system resources.
 * @retval ESM_E_STATUS  Internal status error.
 */
/* ********************************************************************** */
ESM_ERR
esm_PostMessage(const ESM_MESSAGE * const msg)
{
    MODULE_CTX * const mc = &module_ctx;
    ESM_ERR err;

    if (msg == NULL) {
        return ESM_E_PRM;
    }

    esm_md_LockForAPI();

    err = ESM_E_STATUS;

    if (!mc->initialized) {
        goto DONE;
    }
    if (!mc->prepared) {
        goto DONE;
    }

    err = post_message(mc, msg);

DONE:
    esm_md_UnlockForAPI();

    return err;
}

/* ********************************************************************** */
/**
 * @brief   ESM: machdep implementation (sample code).
 * @author  eel3
 * @date    2017-10-18
 */
/* ********************************************************************** */

#include "esm_md.h"
#include "esm_md_eq.h"

#include <stddef.h>

#ifdef ESM_CFG_USE_ASSERT_H
#include <assert.h>
#else
#define assert(cond)
#endif

/* ---------------------------------------------------------------------- */
/* Data structures */
/* ---------------------------------------------------------------------- */

/** Event queue type. */
typedef struct {
    ESM_EVENT_ID buf[ESM_CFG_EVENT_QUEUE_SIZE + 1];
    size_t rp;
    size_t wp;
} EVENT_QUEUE;

/** Module context type. */
typedef struct {
    bool initialized;
    bool prepared;
    ESM_MESSAGE_CELL messages[ESM_CFG_MAX_MESSAGE];

    EVENT_QUEUE queue;
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
/* Private functions: event queue */
/* ---------------------------------------------------------------------- */

/* ====================================================================== */
/**
 * @brief  Return a next index.
 *
 * @param[in] q  Event queue.
 * @param[in] i  Current index.
 *
 * @return  The next index.
 */
/* ====================================================================== */
#define eq_NextIndex(q, i) (((i) + 1) % NELEMS((q)->buf))

/* ====================================================================== */
/**
 * @brief  Initialize EVENT_QUEUE members.
 *
 * @param[out] q  Event queue.
 */
/* ====================================================================== */
static void
eq_Initialize(EVENT_QUEUE * const q)
{
    assert(q != NULL);

    q->rp = q->wp = 0;
}

/* ====================================================================== */
/**
 * @brief  Push data to the event queue.
 *
 * @param[in,out] q    Event queue.
 * @param[in]     val  Data.
 *
 * @retval true   Exit success.
 * @retval false  Exit failure.
 */
/* ====================================================================== */
static bool
eq_Push(EVENT_QUEUE * const q, const ESM_EVENT_ID val)
{
    size_t wp_next;

    assert(q != NULL);

    wp_next = eq_NextIndex(q, q->wp);
    if (wp_next == q->rp) {
        /* Queue is full. */
        return false;
    }

    q->buf[q->wp] = val;
    q->wp = wp_next;

    return true;
}

/* ====================================================================== */
/**
 * @brief  Pop data from the event queue.
 *
 * @param[in,out] q    Event queue.
 * @param[out]    val  Data output place.
 *
 * @retval true   Exit success.
 * @retval false  Exit failure.
 */
/* ====================================================================== */
static bool
eq_Pop(EVENT_QUEUE * const q, ESM_EVENT_ID * const val)
{
    assert((q != NULL) && (val != NULL));

    if (q->rp == q->wp) {
        /* Queue is empty. */
        return false;
    }

    *val = q->buf[q->rp];
    q->rp = eq_NextIndex(q, q->rp);

    return true;
}

/* ---------------------------------------------------------------------- */
/* Public API Functions: for ESM library */
/* ---------------------------------------------------------------------- */

/* ********************************************************************** */
/**
 * @brief  Initialize the machdep library.
 *
 * @retval ESM_E_OK      Exit success.
 * @retval ESM_E_RES     No system resources.
 * @retval ESM_E_STATUS  Internal status error.
 * @retval ESM_E_SYS     Error caused by underlying library routines.
 *
 * @note  This function will be called in esm_Initialize().
 */
/* ********************************************************************** */
ESM_ERR
esm_md_Initialize(void)
{
    MODULE_CTX * const mc = &module_ctx;

    if (mc->initialized) {
        return ESM_E_STATUS;
    }

    mc->prepared = false;

    mc->initialized = true;

    return ESM_E_OK;
}

/* ********************************************************************** */
/**
 * @brief  Finalize the machdep library.
 *
 * @note  This function will be called in esm_Finalize().
 */
/* ********************************************************************** */
void
esm_md_Finalize(void)
{
    MODULE_CTX * const mc = &module_ctx;

    if (!mc->initialized) {
        return;
    }

    mc->initialized = false;
}

/* ********************************************************************** */
/**
 * @brief  Prepare the machdep library before main loop.
 *
 * @retval ESM_E_OK      Exit success.
 * @retval ESM_E_STATUS  Internal status error.
 *
 * @note  This function will be called in esm_PrepareBeforeMainLoop().
 */
/* ********************************************************************** */
ESM_ERR
esm_md_PrepareBeforeMainLoop(void)
{
    MODULE_CTX * const mc = &module_ctx;
    size_t i;

    assert(mc->initialized);

    if (mc->prepared) {
        return ESM_E_STATUS;
    }

    for (i = 0; i < NELEMS(mc->messages); i++) {
        mc->messages[i].empty = true;
    }

    eq_Initialize(&mc->queue);

    mc->prepared = true;

    return ESM_E_OK;
}

/* ********************************************************************** */
/**
 * @brief  Cleanup the machdep library after main loop.
 *
 * @retval ESM_E_OK      Exit success.
 * @retval ESM_E_STATUS  Internal status error.
 *
 * @note  This function will be called in esm_CleanupAfterMainLoop().
 */
/* ********************************************************************** */
ESM_ERR
esm_md_CleanupAfterMainLoop(void)
{
    MODULE_CTX * const mc = &module_ctx;

    assert(mc->initialized);

    if (!mc->prepared) {
        return ESM_E_STATUS;
    }

    mc->prepared = false;

    return ESM_E_OK;
}

/* ********************************************************************** */
/**
 * @brief  Allocate memory space for ESM_MESSAGE_CELL type.
 *
 * @retval !=NULL  Exit success.
 * @retval   NULL  Exit failure.
 */
/* ********************************************************************** */
ESM_MESSAGE_CELL *
esm_md_AllocMessageCell(void)
{
    MODULE_CTX * const mc = &module_ctx;
    size_t i;

    assert(mc->initialized);

    for (i = 0; i < NELEMS(mc->messages); i++) {
        ESM_MESSAGE_CELL *cell;

        cell = &mc->messages[i];
        if (cell->empty) {
            cell->empty = false;
            return cell;
        }
    }

    return NULL;
}

/* ********************************************************************** */
/**
 * @brief  Deallocate memory space for ESM_MESSAGE_CELL type.
 *
 * @param[in,out] cell  Memory space to deallocate.
 */
/* ********************************************************************** */
void
esm_md_DeallocMessageCell(ESM_MESSAGE_CELL * const cell)
{
    assert(module_ctx.initialized);

    cell->empty = true;
}

/* ********************************************************************** */
/**
 * @brief  Get system tick value.
 *
 * @return  System tick in milliseconds.
 */
/* ********************************************************************** */
ESM_SYS_TICK_MSEC
esm_md_GetTick(void)
{
    assert(module_ctx.initialized);

    /* TODO: Need to implement this function. */

    return 0;
}

/* ********************************************************************** */
/**
 * @brief  Peek event.
 *
 * @return  Event ID.
 */
/* ********************************************************************** */
ESM_EVENT_ID
esm_md_PeekEvent(void)
{
    MODULE_CTX * const mc = &module_ctx;
    ESM_EVENT_ID id;

    assert(mc->initialized);

    if (!mc->prepared) {
        return ESM_EVENT_ID_NONE;
    }

    if (!eq_Pop(&mc->queue, &id)) {
        return ESM_EVENT_ID_NONE;
    }

    return id;
}

/* ********************************************************************** */
/**
 * @brief  A lock function for the library.
 */
/* ********************************************************************** */
void
esm_md_LockForAPI(void)
{
    assert(module_ctx.initialized);

    /* TODO: Need to implement this function. */
}

/* ********************************************************************** */
/**
 * @brief  An unlock function for the library.
 */
/* ********************************************************************** */
void
esm_md_UnlockForAPI(void)
{
    assert(module_ctx.initialized);

    /* TODO: Need to implement this function. */
}

/* ---------------------------------------------------------------------- */
/* Public API Functions: for submodules */
/* ---------------------------------------------------------------------- */

/* ********************************************************************** */
/**
 * @brief  Post event ID to the event queue.
 *
 * @param[in] id  Event ID.
 *
 * @retval true   Exit success.
 * @retval false  Exit failure.
 */
/* ********************************************************************** */
bool
esm_md_PostEvent(const ESM_EVENT_ID id)
{
    MODULE_CTX * const mc = &module_ctx;

    assert(mc->initialized);

    if (!mc->prepared) {
        return false;
    }

    return eq_Push(&mc->queue, id);
}

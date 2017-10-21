/* ********************************************************************** */
/**
 * @brief   ESM: machdep implementation (sample && test application).
 * @author  eel3
 * @date    2017-10-18
 */
/* ********************************************************************** */

#include "esm_md.h"

#include <cassert>
#include <chrono>
#include <mutex>

namespace {

/* ---------------------------------------------------------------------- */
/* Data structures */
/* ---------------------------------------------------------------------- */

/** Module context type. */
struct MODULE_CTX {
    bool initialized;
    bool prepared;
    ESM_MESSAGE_CELL messages[ESM_CFG_MAX_MESSAGE];
    std::mutex mutex_for_api;

    MODULE_CTX() : initialized(false), prepared(false) {}
};

/* ---------------------------------------------------------------------- */
/* File scope variables */
/* ---------------------------------------------------------------------- */

/** Module context. */
MODULE_CTX module_ctx;

/* ---------------------------------------------------------------------- */
/* Template Functions */
/* ---------------------------------------------------------------------- */

/* ====================================================================== */
/**
 * @brief  Return the maximum number of elements.
 *
 * @param[in] (no_parameter_name)  An array.
 *
 * @return  Maximum number of elements.
 */
/* ====================================================================== */
template <typename T, size_t N>
inline size_t
NELEMS(const T (&)[N])
{
    return N;
}

} // namespace

/* ---------------------------------------------------------------------- */
/* Functions */
/* ---------------------------------------------------------------------- */

extern "C" {

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
    auto& mc = module_ctx;

    if (mc.initialized) {
        return ESM_E_STATUS;
    }

    mc.prepared = false;

    mc.initialized = true;

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
    auto& mc = module_ctx;

    if (!mc.initialized) {
        return;
    }

    mc.initialized = false;
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
    auto& mc = module_ctx;

    assert(mc.initialized);

    if (mc.prepared) {
        return ESM_E_STATUS;
    }

    for (size_t i { 0 }; i < NELEMS(mc.messages); i++) {
        mc.messages[i].empty = true;
    }

    mc.prepared = true;

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
    auto& mc = module_ctx;

    assert(mc.initialized);

    if (!mc.prepared) {
        return ESM_E_STATUS;
    }

    mc.prepared = false;

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
    auto& mc = module_ctx;

    assert(mc.initialized);

    for (size_t i { 0 }; i < NELEMS(mc.messages); i++) {
        auto& cell = mc.messages[i];
        if (cell.empty) {
            cell.empty = false;
            return &cell;
        }
    }

    return nullptr;
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

    using std::chrono::steady_clock;
    using std::chrono::milliseconds;
    using std::chrono::duration_cast;

    auto tp = steady_clock::now();
    auto ms = duration_cast<milliseconds>(tp.time_since_epoch());

    return static_cast<ESM_SYS_TICK_MSEC>(ms.count());
}

/* ********************************************************************** */
/**
 * @brief  A lock function for the library.
 */
/* ********************************************************************** */
void
esm_md_LockForAPI(void)
{
    auto& mc = module_ctx;

    assert(mc.initialized);

    mc.mutex_for_api.lock();
}

/* ********************************************************************** */
/**
 * @brief  An unlock function for the library.
 */
/* ********************************************************************** */
void
esm_md_UnlockForAPI(void)
{
    auto& mc = module_ctx;

    assert(mc.initialized);

    mc.mutex_for_api.unlock();
}

} // extern "C"

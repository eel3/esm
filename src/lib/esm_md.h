/* ********************************************************************** */
/**
 * @brief   ESM: machdep interfaces.
 * @author  eel3
 * @date    2017-10-18
 */
/* ********************************************************************** */

#ifndef ESM_MD_H_INCLUDED
#define ESM_MD_H_INCLUDED

#include "esm.h"
#include "esm_private.h"

/* ---------------------------------------------------------------------- */
/* Functions */
/* ---------------------------------------------------------------------- */

#ifdef __cplusplus
extern "C" {
#endif /* def __cplusplus */

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
extern ESM_ERR
esm_md_Initialize(void);

/* ********************************************************************** */
/**
 * @brief  Finalize the machdep library.
 *
 * @note  This function will be called in esm_Finalize().
 */
/* ********************************************************************** */
extern void
esm_md_Finalize(void);

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
extern ESM_ERR
esm_md_PrepareBeforeMainLoop(void);

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
extern ESM_ERR
esm_md_CleanupAfterMainLoop(void);

/* ********************************************************************** */
/**
 * @brief  Allocate memory space for ESM_MESSAGE_CELL type.
 *
 * @retval !=NULL  Exit success.
 * @retval   NULL  Exit failure.
 */
/* ********************************************************************** */
extern ESM_MESSAGE_CELL *
esm_md_AllocMessageCell(void);

/* ********************************************************************** */
/**
 * @brief  Deallocate memory space for ESM_MESSAGE_CELL type.
 *
 * @param[in,out] cell  memory space to deallocate.
 */
/* ********************************************************************** */
extern void
esm_md_DeallocMessageCell(ESM_MESSAGE_CELL * const cell);

/* ********************************************************************** */
/**
 * @brief  Get system tick value.
 *
 * @return  System tick in milliseconds.
 */
/* ********************************************************************** */
extern ESM_SYS_TICK_MSEC
esm_md_GetTick(void);

/* ********************************************************************** */
/**
 * @brief  Peek event.
 *
 * @return  Event ID.
 */
/* ********************************************************************** */
extern ESM_EVENT_ID
esm_md_PeekEvent(void);

/* ********************************************************************** */
/**
 * @brief  A lock function for the library.
 */
/* ********************************************************************** */
extern void
esm_md_LockForAPI(void);

/* ********************************************************************** */
/**
 * @brief  An unlock function for the library.
 */
/* ********************************************************************** */
extern void
esm_md_UnlockForAPI(void);

#ifdef __cplusplus
} /* extern "C" */
#endif /* def __cplusplus */

#endif /* ndef ESM_MD_H_INCLUDED */

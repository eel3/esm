/* ********************************************************************** */
/**
 * @brief   ESM: public API interfaces.
 * @author  eel3
 * @date    2021-04-15
 */
/* ********************************************************************** */

#ifndef ESM_H_INCLUDED
#define ESM_H_INCLUDED

#include <stdint.h>
#ifdef __cplusplus
#   include <cstdbool>
#else  /* def __cplusplus */
#   include <stdbool.h>
#endif /* def __cplusplus */

#include "esm_types.h"

/* ---------------------------------------------------------------------- */
/* Error type and codes */
/* ---------------------------------------------------------------------- */

/** ESM module error type. */
typedef int32_t ESM_ERR;

#define ESM_E_OK        ((ESM_ERR)  0)      /**< Exit success. */
#define ESM_E_NG        ((ESM_ERR) -1)      /**< Exit failure. */

#define ESM_E_PRM       (ESM_E_NG - 1)      /**< Parameter error (perhaps arguments error). */
#define ESM_E_RES       (ESM_E_NG - 2)      /**< No system resources. */
#define ESM_E_STATUS    (ESM_E_NG - 3)      /**< Internal status error. */
#define ESM_E_SYS       (ESM_E_NG - 4)      /**< Error caused by underlying library routines. */

/* ---------------------------------------------------------------------- */
/* Data types */
/* ---------------------------------------------------------------------- */

/** Timer ID type (must be greater than or equal to 0). */
typedef uint32_t ESM_TIMER_ID;

/* ---------------------------------------------------------------------- */
/* Data structures */
/* ---------------------------------------------------------------------- */

/** Event handler type. */
typedef struct ESM_EVENT_HANDLER ESM_EVENT_HANDLER;
/** Event handler type. */
struct ESM_EVENT_HANDLER {
    void (*on_init)(void * const user_data);
    void (*on_event)(void * const user_data, const ESM_EVENT_ID id);
    void (*on_timer)(void * const user_data, const ESM_TIMER_ID id);
    void (*on_destroy)(void * const user_data);
    void (*release_user_data)(void * const user_data);
    void *user_data;
};

/** Generic handler type. */
typedef struct ESM_GENERIC_HANDLER ESM_GENERIC_HANDLER;
/** Generic handler type. */
struct ESM_GENERIC_HANDLER {
    void (*func)(void * const user_data);
    void (*release_user_data)(void * const user_data);
    void *user_data;
};

/** Timer handler type. */
typedef ESM_GENERIC_HANDLER ESM_TIMER_HANDLER;
/** Message type. */
typedef ESM_GENERIC_HANDLER ESM_MESSAGE;

/** Preparation parameters. */
typedef struct ESM_PREPARE_PARAMS ESM_PREPARE_PARAMS;
/** Preparation parameters. */
struct ESM_PREPARE_PARAMS {
    const ESM_EVENT_HANDLER *default_handler;
};

/* ---------------------------------------------------------------------- */
/* Public API functions */
/* ---------------------------------------------------------------------- */

#ifdef __cplusplus
extern "C" {
#endif /* def __cplusplus */

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
extern ESM_ERR
esm_Initialize(void);

/* ********************************************************************** */
/**
 * @brief  Finalize the library.
 */
/* ********************************************************************** */
extern void
esm_Finalize(void);

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
extern ESM_ERR
esm_PrepareBeforeMainLoop(const ESM_PREPARE_PARAMS * const params);

/* ********************************************************************** */
/**
 * @brief  A resume-yield function for the main loop.
 *
 * @retval ESM_E_OK      Exit success.
 * @retval ESM_E_STATUS  Internal status error.
 */
/* ********************************************************************** */
extern ESM_ERR
esm_ResumeAndYield(void);

/* ********************************************************************** */
/**
 * @brief  Cleanup the library after main loop.
 *
 * @retval ESM_E_OK      Exit success.
 * @retval ESM_E_STATUS  Internal status error.
 */
/* ********************************************************************** */
extern ESM_ERR
esm_CleanupAfterMainLoop(void);

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
extern ESM_ERR
esm_SetNextEventHandler(const ESM_EVENT_HANDLER * const handler);

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
extern ESM_ERR
esm_SetTimer(const ESM_TIMER_ID id,
             const ESM_SYS_TICK_MSEC timeout_msec,
             const bool repeat);

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
extern ESM_ERR
esm_KillTimer(const ESM_TIMER_ID id);

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
extern ESM_ERR
esm_SetGlobalTimer(const ESM_TIMER_ID id,
                   const ESM_SYS_TICK_MSEC timeout_msec,
                   const bool repeat,
                   const ESM_TIMER_HANDLER * const handler);

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
extern ESM_ERR
esm_KillGlobalTimer(const ESM_TIMER_ID id);

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
extern ESM_ERR
esm_PostMessage(const ESM_MESSAGE * const msg);

#ifdef __cplusplus
} /* extern "C" */
#endif /* def __cplusplus */

#endif /* ndef ESM_H_INCLUDED */

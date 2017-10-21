/* ********************************************************************** */
/**
 * @brief   ESM: configurations (sample code).
 * @author  eel3
 * @date    2017-10-18
 */
/* ********************************************************************** */

#ifndef ESM_CONFIG_H_INCLUDED
#define ESM_CONFIG_H_INCLUDED

/* ---------------------------------------------------------------------- */
/* Configurations for the library */
/* ---------------------------------------------------------------------- */

/** Maximum number of timers. */
#define ESM_CFG_MAX_TIMER 8

/** Maximum number of global timers. */
#define ESM_CFG_MAX_GLOBAL_TIMER 8

#if 0
/** Use C standard library's assert.h (for debug on hosted environment). */
#define ESM_CFG_USE_ASSERT_H
#endif

/* ---------------------------------------------------------------------- */
/* Configurations for the machdep library (for sample code only) */
/* ---------------------------------------------------------------------- */

/** Maximum number of messages. */
#define ESM_CFG_MAX_MESSAGE 16

/** Maximum size of event queue. */
#define ESM_CFG_EVENT_QUEUE_SIZE 32

#endif /* ndef ESM_CONFIG_H_INCLUDED */

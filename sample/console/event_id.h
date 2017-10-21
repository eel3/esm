/* ********************************************************************** */
/**
 * @brief   ESM: event ID definitions (sample && test application).
 * @author  eel3
 * @date    2017-10-18
 */
/* ********************************************************************** */

#ifndef EVENT_ID_H_INCLUDED
#define EVENT_ID_H_INCLUDED

#include <stdint.h>

/* ---------------------------------------------------------------------- */
/* Data types */
/* ---------------------------------------------------------------------- */

/** Event bit type (31bit, NOT 32bit). */
typedef uint32_t EVENT_BIT;

/* ---------------------------------------------------------------------- */
/* Constants */
/* ---------------------------------------------------------------------- */

#define EVENT_BITMASK_CMD           ((EVENT_BIT) 0x7F000000UL)
#define EVENT_BITMASK_TIMER_ID      ((EVENT_BIT) 0x007F0000UL)
#define EVENT_BITMASK_TIMER_TIMEOUT ((EVENT_BIT) 0x0000FFFFUL)
#define EVENT_BITMASK_TIMER_REPEAT  ((EVENT_BIT) 0x00800000UL)

#define EVENT_BIT_CMD_NEXT_HANDLER  ((EVENT_BIT) 0x00000000UL)
#define EVENT_BIT_CMD_SET_TIMER     ((EVENT_BIT) 0x01000000UL)
#define EVENT_BIT_CMD_KILL_TIMER    ((EVENT_BIT) 0x02000000UL)
#define EVENT_BIT_CMD_SET_GTIMER    ((EVENT_BIT) 0x03000000UL)
#define EVENT_BIT_CMD_KILL_GTIMER   ((EVENT_BIT) 0x04000000UL)
#define EVENT_BIT_CMD_POST_MESSAGE  ((EVENT_BIT) 0x05000000UL)

#define EVENT_BIT_TIMER_REPEAT_OFF  ((EVENT_BIT) 0x00000000UL)
#define EVENT_BIT_TIMER_REPEAT_ON   ((EVENT_BIT) 0x00800000UL)

#endif /* ndef EVENT_ID_H_INCLUDED */

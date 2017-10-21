/* ********************************************************************** */
/**
 * @brief   ESM: private interfaces.
 * @author  eel3
 * @date    2017-10-18
 */
/* ********************************************************************** */

#ifndef ESM_PRIVATE_H_INCLUDED
#define ESM_PRIVATE_H_INCLUDED

#include "esm.h"
#include "esm_config.h"

/* ---------------------------------------------------------------------- */
/* Data structures */
/* ---------------------------------------------------------------------- */

/** Message cell type. */
typedef struct ESM_MESSAGE_CELL ESM_MESSAGE_CELL;
/** Message cell type. */
struct  ESM_MESSAGE_CELL {
    bool empty;     /* For machdep library only */

    ESM_MESSAGE_CELL *next;
    ESM_MESSAGE message;
};

#endif /* ndef ESM_PRIVATE_H_INCLUDED */

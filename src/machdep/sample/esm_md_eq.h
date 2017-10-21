/* ********************************************************************** */
/**
 * @brief   ESM: machdep interfaces for submodules (sample code).
 * @author  eel3
 * @date    2017-10-18
 */
/* ********************************************************************** */

#ifndef ESM_MD_EQ_H_INCLUDED
#define ESM_MD_EQ_H_INCLUDED

#include "esm.h"

/* ---------------------------------------------------------------------- */
/* Public API Functions */
/* ---------------------------------------------------------------------- */

/* ********************************************************************** */
/**
 * @brief  Post event ID to the event queue.
 *
 * @param[in] id  Event ID.
 *
 * @retval true  Exit success.
 * @retval false Exit failure.
 */
/* ********************************************************************** */
extern bool
esm_md_PostEvent(const ESM_EVENT_ID id);

#endif /* ndef ESM_MD_EQ_H_INCLUDED */

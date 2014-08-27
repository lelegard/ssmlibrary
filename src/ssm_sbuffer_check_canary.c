/*-----------------------------------------------------------------------------
 *
 *  Copyright (c) 2014, Thierry Lelegard
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *  See the file COPYING.txt included with this distribution for more
 *  information.
 *
 *-----------------------------------------------------------------------------
 *
 * Detecting memory corruptions on static memory buffers using canary checks.
 *
 *-----------------------------------------------------------------------------
 *
 *  Modification history:
 *    2014-01-10 Thierry Lelegard
 *               Original creation.
 *
 *-----------------------------------------------------------------------------
 */

#define SSM_USE_CANARY 1
#include "ssm_internal.h"


/*-----------------------------------------------------------------------------
 * Compute the trailing canary address of a static buffer.
 * API is not clean since it removes the const'ness of the buffer.
 *-----------------------------------------------------------------------------
 */
static inline _ssm_canary_t* trail_canary(const ssm_sbuffer_t* buf)
{
    return (_ssm_canary_t*)(buf->_data + buf->_maxSize - sizeof(_ssm_canary_t));
}


/*-----------------------------------------------------------------------------
 * Basic consistency checks of the structure.
 * Return non-zero (true) if consistent, zero (false) if corrupted.
 *-----------------------------------------------------------------------------
 */
static int consistent(const ssm_sbuffer_t* buf)
{
    return buf != NULL &&
        buf->_maxSize > sizeof(_ssm_canary_t) &&
        buf->_maxSize <= SSM_SIZE_MAX &&
        buf->_curSize < buf->_maxSize - sizeof(_ssm_canary_t) &&
        buf->_data[buf->_curSize] == 0;
}


/*-----------------------------------------------------------------------------
 * Check an input static buffer parameter for memory corruption.
 *-----------------------------------------------------------------------------
 */

int _ssm_sbuffer_check_in_canary(const ssm_sbuffer_t* buf, const char* file, size_t line)
{
    int ok = 0;
    
    if (buf == NULL) {
        /* NULL means empty buffer, verification successful */
        return 1;
    }

    ok = consistent(buf);

    if (ok && buf->_canary == _SSM_CANARY_INIT && buf->_curSize == 0) {
        /* Pre-initialized unmodified buffer, verification successful */
        return 1;
    }

    if (ok && buf->_canary == _SSM_CANARY_UPDATE && *trail_canary(buf) == _SSM_CANARY_TRAIL) {
        /* Modified buffer, verification successful */
        return 1;
    }

    /* Memory corruption detected */
    _ssm_canary_corruption(file, line);

    /* Most of the time, we never return from _ssm_canary_corruption() */
    return 0;
}


/*-----------------------------------------------------------------------------
 * Check an output static buffer parameter for memory corruption.
 *-----------------------------------------------------------------------------
 */

int _ssm_sbuffer_check_out_canary(ssm_sbuffer_t* buf, const char* file, size_t line)
{
    const int ok = consistent(buf);

    if (ok && buf->_canary == _SSM_CANARY_INIT && buf->_curSize == 0) {
        /* This is a pre-initialized, unmodified, buffer. Update its canaries */
        buf->_canary = _SSM_CANARY_UPDATE;
        *trail_canary(buf) = _SSM_CANARY_TRAIL;
        /* Verification successful */
        return 1;
    }

    if (ok && buf->_canary == _SSM_CANARY_UPDATE && *trail_canary(buf) == _SSM_CANARY_TRAIL) {
        /* Modified buffer, verification successful */
        return 1;
    }

    /* Memory corruption detected */
    _ssm_canary_corruption(file, line);

    /* Most of the time, we never return from _ssm_canary_corruption() */
    return 0;
}

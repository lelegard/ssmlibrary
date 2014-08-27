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
 * Detecting memory corruptions on dynamic memory buffers using canary checks.
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

int _ssm_dbuffer_check_canary(const ssm_dbuffer_t* buf, const char* file, size_t line)
{
    int ok = 0;

    if (buf == NULL) {
        /* NULL means empty buffer, verification successful */
        return 1;
    }

    /* Check integrity of structure ssm_dbuffer_t */
    ok = buf->_canary1 == _SSM_CANARY_INIT && buf->_canary2 == _SSM_CANARY_TRAIL;

    if (ok) {
        /* Structure ssm_dbuffer_t is likely not corrupted */
        if (buf->_data == NULL) {
            /* No allocated memory */
            ok = buf->_maxSize == 0 && buf->_curSize == 0;
        }
        else {
            /* Check integrity of allocated memory */
            ok = buf->_maxSize > 2 * sizeof(_ssm_canary_t) &&
                buf->_maxSize <= SSM_SIZE_MAX &&
                buf->_curSize < buf->_maxSize - 2 * sizeof(_ssm_canary_t) &&
                buf->_data[sizeof(_ssm_canary_t) + buf->_curSize] == 0 &&
                *(const _ssm_canary_t*)(buf->_data) == _SSM_CANARY_INIT &&
                *(const _ssm_canary_t*)(buf->_data + buf->_maxSize - sizeof(_ssm_canary_t)) == _SSM_CANARY_TRAIL;
        }
        if (ok) {
            /* Verification successful */
            return 1;
        }
    }

    /* Memory corruption detected */
    _ssm_canary_corruption(file, line);

    /* Most of the time, we never return from _ssm_canary_corruption() */
    return 0;
}

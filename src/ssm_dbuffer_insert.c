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
 * Implementation of function _ssm_dbuffer_insert
 *
 *-----------------------------------------------------------------------------
 *
 *  Modification history:
 *    2014-01-10 Thierry Lelegard
 *               Original creation.
 *
 *-----------------------------------------------------------------------------
 */

#include "ssm_internal.h"

ssm_status_t _SSM_CANARY_SYMBOL(_ssm_dbuffer_insert) (ssm_dbuffer_t* buf, size_t start, size_t length _SSM_CANARY_ARG_DECL)
{
    if (buf == NULL) {
        return SSM_NULLOUT;
    }
    else if (!_SSM_CANARY_SYMBOL(_ssm_dbuffer_check)(buf _SSM_CANARY_ARG_USE)) {
        return SSM_CORRUPTED;
    }
    else if (length > SSM_SIZE_MAX) {
        return SSM_SIZETOOLARGE;
    }
    else if (start > buf->_curSize) {
        return SSM_INDEXRANGE;
    }
    else if (length == 0) {
        return SSM_OK; /* nothing to do */
    }
    else {
        /* Enforce output data size, keep previous content */
        ssm_status_t status = _SSM_CANARY_SYMBOL(_ssm_dbuffer_alloc)(buf, buf->_curSize + length, 1 _SSM_CANARY_ARG_USE);
        if (status == SSM_OK) {
            /* Move content at start upward */
            size_t copied = 0;
            status = ssm_copy(buf->_data +_SSM_CANARY_SIZE + start + length,
                              buf->_maxSize - start - length - 2 * _SSM_CANARY_SIZE - 1,
                              buf->_data +_SSM_CANARY_SIZE + start,
                              buf->_curSize - start,
                              &copied);
            if (ssm_success(status)) {
                buf->_curSize = start + length + copied;
                _SSM_ASSERT(buf->_curSize + 2 * _SSM_CANARY_SIZE + 1 <= buf->_maxSize);
                buf->_data[_SSM_CANARY_SIZE + buf->_curSize] = 0;
            }
        }
        return status;
    }
}

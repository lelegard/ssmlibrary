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
 * Implementation of function _ssm_sbuffer_insert
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

ssm_status_t _SSM_CANARY_SYMBOL(_ssm_sbuffer_insert) (ssm_sbuffer_t* buf, size_t start, size_t length _SSM_CANARY_ARG_DECL)
{
    if (buf == NULL) {
        return SSM_NULLOUT;
    }
    else if (!_SSM_CANARY_SYMBOL(_ssm_sbuffer_check_out)(buf _SSM_CANARY_ARG_USE)) {
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
    else if (start + length + _SSM_CANARY_SIZE + 1 >= buf->_maxSize) {
        /* Attempt to relocate past the end of buffer */
        _SSM_ASSERT(buf->_maxSize >= _SSM_CANARY_SIZE + 1);
        buf->_curSize = buf->_maxSize - _SSM_CANARY_SIZE - 1;
        buf->_data[buf->_curSize] = 0;
        return SSM_TRUNCATED;
    }
    else {
        /* Relocated part is partly or entirely within the buffer */
        size_t copied = 0;
        ssm_status_t status = ssm_copy(buf->_data + start + length,
                                       buf->_maxSize - start - length - _SSM_CANARY_SIZE - 1,
                                       buf->_data + start,
                                       buf->_curSize - start,
                                       &copied);
        if (ssm_success(status)) {
            buf->_curSize = start + length + copied;
            _SSM_ASSERT(buf->_curSize + _SSM_CANARY_SIZE + 1 <= buf->_maxSize);
            buf->_data[buf->_curSize] = 0;
        }
        return status;
    }
}

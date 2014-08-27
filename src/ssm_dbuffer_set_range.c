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
 * Implementation of function _ssm_dbuffer_set_range
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

ssm_status_t _SSM_CANARY_SYMBOL(_ssm_dbuffer_set_range) (ssm_dbuffer_t* buf,
                                                         size_t start,
                                                         size_t length,
                                                         uint8_t value
                                                         _SSM_CANARY_ARG_DECL)
{
    if (buf == NULL) {
        return SSM_NULLOUT;
    }
    else if (start > SSM_SIZE_MAX || length > SSM_SIZE_MAX) {
        return SSM_SIZETOOLARGE;
    }
    else if (!_SSM_CANARY_SYMBOL(_ssm_dbuffer_check)(buf _SSM_CANARY_ARG_USE)) {
        return SSM_CORRUPTED;
    }
    else if (start == buf->_curSize && length == 0) {
        return SSM_OK;
    }
    else if (start >= buf->_curSize) {
        return SSM_TRUNCATED;
    }
    else {
        const size_t actualSize = start + length > buf->_curSize ? buf->_curSize - start : length;
        ssm_status_t status = ssm_set(buf->_data + _SSM_CANARY_SIZE + start, actualSize, value);
        if (ssm_success(status)) {
            status = actualSize < length ? SSM_TRUNCATED : SSM_OK;
        }
        return status;
    }
}

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
 * Implementation of function _ssm_dbuffer_copy
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

ssm_status_t _SSM_CANARY_SYMBOL(_ssm_dbuffer_copy) (ssm_dbuffer_t* dest, const ssm_dbuffer_t* src _SSM_CANARY_ARG_DECL)
{
    ssm_status_t status;

    if (dest == NULL) {
        return SSM_NULLOUT;
    }
    else if (!_SSM_CANARY_SYMBOL(_ssm_dbuffer_check)(dest _SSM_CANARY_ARG_USE) ||
             !_SSM_CANARY_SYMBOL(_ssm_dbuffer_check)(src _SSM_CANARY_ARG_USE)) {
        return SSM_CORRUPTED;
    }
    else if (src == dest) {
        /* Don't copy identical buffers */
        return SSM_OK;
    }

    /* Enforce output data size, don't keep previous content */
    status = _SSM_CANARY_SYMBOL(_ssm_dbuffer_alloc)(dest, src == NULL ? 0 : src->_curSize, 0 _SSM_CANARY_ARG_USE);
    if (status == SSM_OK) {
        if (src == NULL) {
            dest->_curSize = 0;
        }
        else {
            status = ssm_copy(dest->_data + _SSM_CANARY_SIZE,
                              dest->_maxSize - 2 * _SSM_CANARY_SIZE - 1,
                              src->_data + _SSM_CANARY_SIZE,
                              src->_curSize,
                              &dest->_curSize);
        }
        dest->_data[_SSM_CANARY_SIZE + dest->_curSize] = 0;
    }
    return status;
}

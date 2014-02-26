/*-----------------------------------------------------------------------------
 *
 *  Copyright (c) 2014, Thierry Lelegard
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 3.0 of the License, or (at your option) any later version.
 *  See the file COPYING.txt included with this distribution for more
 *  information.
 *
 *-----------------------------------------------------------------------------
 *
 * Implementation of function _ssm_dbuffer_import
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

ssm_status_t _SSM_CANARY_SYMBOL(_ssm_dbuffer_import) (ssm_dbuffer_t* dest, const void* src, size_t maxSize _SSM_CANARY_ARG_DECL)
{
    ssm_status_t status;

    /* NULL src means empty buffer */
    if (src == NULL) {
        maxSize = 0;
    }

    /* Enforce output data size, don't keep previous content */
    status = _SSM_CANARY_SYMBOL(_ssm_dbuffer_alloc)(dest, maxSize, 0 _SSM_CANARY_ARG_USE);
    if (status == SSM_OK) {
        /* Copy new buffer content */
        status = ssm_copy(dest->_data + _SSM_CANARY_SIZE,
                          dest->_maxSize - 2 * _SSM_CANARY_SIZE - 1,
                          src,
                          maxSize,
                          &dest->_curSize);
        dest->_data[_SSM_CANARY_SIZE + dest->_curSize] = 0;
    }
    return status;
}

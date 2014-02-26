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
 * Implementation of function _ssm_dbuffer_resize
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

ssm_status_t _SSM_CANARY_SYMBOL(_ssm_dbuffer_resize) (ssm_dbuffer_t* buf, size_t length _SSM_CANARY_ARG_DECL)
{
    ssm_status_t status;

    /* Enforce output data size, keep previous content */
    status = _SSM_CANARY_SYMBOL(_ssm_dbuffer_alloc)(buf, length, 1 _SSM_CANARY_ARG_USE);
    if (status == SSM_OK) {
        buf->_curSize = length;
        buf->_data[_SSM_CANARY_SIZE + buf->_curSize] = 0;
    }
    return status;
}

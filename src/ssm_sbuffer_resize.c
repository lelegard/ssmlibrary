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
 * Implementation of function _ssm_sbuffer_resize
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

ssm_status_t _SSM_CANARY_SYMBOL(_ssm_sbuffer_resize) (ssm_sbuffer_t* buf, size_t length _SSM_CANARY_ARG_DECL)
{

    if (buf == NULL) {
        return SSM_NULLOUT;
    }
    else if (length > SSM_SIZE_MAX) {
        return SSM_SIZETOOLARGE;
    }
    else if (!_SSM_CANARY_SYMBOL(_ssm_sbuffer_check_out)(buf _SSM_CANARY_ARG_USE)) {
        return SSM_CORRUPTED;
    }
    else if (length >= buf->_maxSize - _SSM_CANARY_SIZE) {
        buf->_curSize = buf->_maxSize - _SSM_CANARY_SIZE - 1;
        buf->_data[buf->_curSize] = 0;
        return SSM_TRUNCATED;
    }
    else {
        buf->_curSize = length;
        buf->_data[buf->_curSize] = 0;
        return SSM_OK;
    }
}

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
 * Implementation of function _ssm_sbuffer_concat
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

ssm_status_t _SSM_CANARY_SYMBOL(_ssm_sbuffer_concat) (ssm_sbuffer_t* dest, const ssm_sbuffer_t* src _SSM_CANARY_ARG_DECL)
{
    if (dest == NULL) {
        return SSM_NULLOUT;
    }
    else if (!_SSM_CANARY_SYMBOL(_ssm_sbuffer_check_out)(dest _SSM_CANARY_ARG_USE) ||
             !_SSM_CANARY_SYMBOL(_ssm_sbuffer_check_in)(src _SSM_CANARY_ARG_USE)) {
        return SSM_CORRUPTED;
    }
    else if (src == NULL) {
        /* Same as appending an empty buffer => void operation */
        return SSM_OK;
    }
    else {
        size_t copied = 0;
        ssm_status_t status = ssm_copy(dest->_data + dest->_curSize,
                                       dest->_maxSize - dest->_curSize - _SSM_CANARY_SIZE - 1,
                                       src->_data,
                                       src->_curSize,
                                       &copied);
        dest->_curSize += copied;
        dest->_data[dest->_curSize] = 0;
        return status;
    }
}

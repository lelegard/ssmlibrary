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
 * Detecting basic memory corruptions on dynamic memory buffers.
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

int _ssm_dbuffer_check(const ssm_dbuffer_t* buf)
{
    if (buf == NULL) {
        /* NULL means empty buffer, verification successful */
        return 1;
    }
    else if (buf->_data == NULL) {
        /* No dynamic memory allocated */
        return buf->_maxSize == 0 &&
            buf->_curSize == 0;
    }
    else {
        return buf->_maxSize > 0 &&
            buf->_maxSize <= SSM_SIZE_MAX &&
            buf->_curSize < buf->_maxSize &&
            buf->_data[buf->_curSize] == 0;
    }
}

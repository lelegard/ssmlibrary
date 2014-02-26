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
 * Detecting basic memory corruptions on static memory buffers.
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

int _ssm_sbuffer_check_in(const ssm_sbuffer_t* buf)
{
    return buf == NULL ||
        (buf->_maxSize > 0 && 
        buf->_maxSize <= SSM_SIZE_MAX &&
        buf->_curSize < buf->_maxSize &&
        buf->_data[buf->_curSize] == 0);
}

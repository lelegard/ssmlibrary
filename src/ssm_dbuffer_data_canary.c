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
 * Implementation of function _ssm_dbuffer_data_canary
 *
 *-----------------------------------------------------------------------------
 *
 *  Modification history:
 *    2014-01-10 Thierry Lelegard
 *               Original creation.
 *
 *-----------------------------------------------------------------------------
 */

#define SSM_USE_CANARY 1
#include "ssm_internal.h"

/*
 * This function can be used with or without canary checks. Hovever, the version
 * without canary checks is an inlined optimized version without checks in ssm.h.
 */
const void* _SSM_CANARY_SYMBOL(_ssm_dbuffer_data) (const ssm_dbuffer_t* buf _SSM_CANARY_ARG_DECL)
{
    if (buf == NULL || buf->_data == NULL || !_SSM_CANARY_SYMBOL(_ssm_dbuffer_check)(buf _SSM_CANARY_ARG_USE)) {
        /* Empty or invalid buffer */
        return "";
    }
    else {
        return buf->_data + _SSM_CANARY_SIZE;
    }
}

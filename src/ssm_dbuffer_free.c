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
 * Implementation of function _ssm_dbuffer_free
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

ssm_status_t _SSM_CANARY_SYMBOL(_ssm_dbuffer_free) (ssm_dbuffer_t* buf _SSM_CANARY_ARG_DECL)
{
    /*
     * There are two possible cases of memory corruption:
     *
     * 1) The ssm_dbuffer_t structure is corrupted. It is not possible to
     *    free anything since the _data pointers is probably corrupted.
     * 2) The allocated data is corrupted. Since the ssm_dbuffer_t structure
     *    is still correct, it could be possible to free the allocated memory.
     *    However, many allocation systems use a fixed-size header before the
     *    allocated area. If the beginning of the allocated area is corrupted,
     *    there is a risk that the header immediately preceding it in memory
     *    is corrupted as well. Freeing that pointer may lead to a complete
     *    corruption of the memory allocation system. In that case, we prefer
     *    the memory leak over the memory allocation system corruption.
     *
     * So, in both cases, we don't free anyting and report a corruption status.
     */
    if (!_SSM_CANARY_SYMBOL(_ssm_dbuffer_check)(buf _SSM_CANARY_ARG_USE)) {
        return SSM_CORRUPTED;
    }
    if (buf != NULL) {
        _ssm_free(buf->_data);
        buf->_data = NULL;
        buf->_maxSize = 0;
        buf->_curSize = 0;
    }
    return SSM_OK;
}

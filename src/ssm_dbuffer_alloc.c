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
 * Allocation of dynamic memory buffers.
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

ssm_status_t _SSM_CANARY_SYMBOL(_ssm_dbuffer_alloc) (ssm_dbuffer_t* buf, size_t size, int keepContent _SSM_CANARY_ARG_DECL)
{
    char* newData = NULL;
    ssm_status_t status = SSM_OK;

    /* Object must be modified, cannot be NULL */
    if (buf == NULL) {
        return SSM_NULLOUT;
    }

    /* Buffer consistency checks */
    if (!_SSM_CANARY_SYMBOL(_ssm_dbuffer_check) (buf _SSM_CANARY_ARG_USE)) {
        return SSM_CORRUPTED;
    }
    if (size > SSM_SIZE_MAX) {
        return SSM_SIZETOOLARGE;
    }

    /* Apply allocation size policy and, if identical, keep same buffer. */
    size = _ssm_alloc_size(buf->_maxSize, size + 1 + 2 * _SSM_CANARY_SIZE);
    if (size == buf->_maxSize) {
        return SSM_OK;
    }

    /* Allocate new buffer data */
    newData = (char*)_ssm_malloc(size);
    if (newData == NULL) {
        return SSM_NOMEMORY;
    }

    /* If content preservation is required, copy it */
    if (keepContent && buf->_data != NULL) {
        /* Copy the content */
        status = ssm_copy(newData + _SSM_CANARY_SIZE,
                          size - 2 * _SSM_CANARY_SIZE - 1,
                          buf->_data + _SSM_CANARY_SIZE,
                          buf->_curSize,
                          &buf->_curSize);
        newData[_SSM_CANARY_SIZE + buf->_curSize] = 0;
    }
    else {
        /* Don't preserve content or was already empty */
        buf->_curSize = 0;
        newData[_SSM_CANARY_SIZE] = 0;
    }

    /* Initialize canaries in new allocated area */
#if defined(SSM_USE_CANARY)
    *(_ssm_canary_t*)(newData) = _SSM_CANARY_INIT;
    *(_ssm_canary_t*)(newData + size - _SSM_CANARY_SIZE) = _SSM_CANARY_TRAIL;
#endif

    /* Replace allocated area */
    _ssm_free(buf->_data);
    buf->_data = newData;
    buf->_maxSize = size;

    return status;
}

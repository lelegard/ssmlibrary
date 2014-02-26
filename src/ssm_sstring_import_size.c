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
 * Implementation of function _ssm_sstring_import_size
 *
 *-----------------------------------------------------------------------------
 *
 *  Modification history:
 *    2014-01-06 Thierry Lelegard
 *               Original creation.
 *
 *-----------------------------------------------------------------------------
 */

#include "ssm_internal.h"

ssm_status_t _SSM_CANARY_SYMBOL(_ssm_sstring_import_size) (ssm_sstring_t* dest, const char* src, size_t maxSize _SSM_CANARY_ARG_DECL)
{
    if (dest == NULL) {
        return SSM_NULLOUT;
    }
    else if (maxSize > SSM_SIZE_MAX) {
        return SSM_SIZETOOLARGE;
    }
    else {
        return _SSM_CANARY_SYMBOL(_ssm_sbuffer_import) (&dest->_buf,
                                                        src,
                                                        src == NULL ? 0 : ssm_cstring_length(src, maxSize)
                                                        _SSM_CANARY_ARG_USE);
    }
}

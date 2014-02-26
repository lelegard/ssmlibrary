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
 * Implementation of function _ssm_sbuffer_compare
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

ssm_status_t _SSM_CANARY_SYMBOL(_ssm_sbuffer_compare) (const ssm_sbuffer_t* buf1, const ssm_sbuffer_t* buf2 _SSM_CANARY_ARG_DECL)
{
    if (!_SSM_CANARY_SYMBOL(_ssm_sbuffer_check_in)(buf1 _SSM_CANARY_ARG_USE) ||
        !_SSM_CANARY_SYMBOL(_ssm_sbuffer_check_in)(buf2 _SSM_CANARY_ARG_USE)) {
        return SSM_CORRUPTED;
    }
    else {
        return ssm_compare(buf1 == NULL ? NULL : buf1->_data,
                           buf1 == NULL ? 0 : buf1->_curSize,
                           buf2 == NULL ? NULL : buf2->_data,
                           buf2 == NULL ? 0 : buf2->_curSize);
    }
}

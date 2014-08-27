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
 * Implementation of function ssm_memset_s
 *
 *-----------------------------------------------------------------------------
 *
 *  Modification history:
 *    2014-06-20 Thierry Lelegard
 *               Original creation.
 *
 *-----------------------------------------------------------------------------
 */

#include "ssm_internal.h"

ssm_errno_t ssm_memset_s (void* s, ssm_rsize_t smax, int c, ssm_rsize_t n)
{
    if (smax > SSM_RSIZE_MAX) {
        return SSM_SIZETOOLARGE;
    }
    else {
        const int truncate = n > smax;
        const ssm_status_t status = ssm_set(s, truncate ? smax : n, (uint8_t)c);
        if (!truncate || !ssm_success(status)) {
            return status;
        }
        else if (n > SSM_RSIZE_MAX) {
            return SSM_SIZETOOLARGE;
        }
        else {
            return SSM_TRUNCATED;
        }
    }
}

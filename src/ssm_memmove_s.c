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
 * Implementation of function ssm_memmove_s
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

ssm_errno_t ssm_memmove_s (void* s1, ssm_rsize_t s1max, const void* s2, ssm_rsize_t n)
{
    ssm_status_t status;
    if (s2 == NULL) {
        /* Normally allowed by ssm_copy (meaning empty buffer) */
        status = SSM_NULLIN;
    }
    else if (n > SSM_RSIZE_MAX) {
        /* Normally caught by ssm_copy but need to preceed next test */
        status = SSM_SIZETOOLARGE;
    }
    else if (n > s1max) {
        /* Normally allowed by ssm_copy (truncated copy) */
        status = SSM_INDEXRANGE;
    }
    else {
        status = ssm_copy(s1, s1max, s2, n, NULL);
    }
    if (!ssm_success(status) && s1 != NULL && s1max <= SSM_RSIZE_MAX) {
        /* The C11K standard specifies that the output buffer is zeroed on error */
        ssm_set(s1, s1max, 0);
    }
    return status;
}

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
 * Implementation of function ssm_strcpy_s
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

ssm_errno_t ssm_strcpy_s (char* s1, ssm_rsize_t s1max, const char* s2)
{
    /* This implementation is not optimal, the string at s2 is read twice */
    size_t len = 0;
    ssm_status_t status;
    if (s2 == NULL) {
        /* Normally allowed by ssm_copy (meaning empty buffer) */
        status = SSM_NULLIN;
    }
    else if ((len = ssm_cstring_length(s2, s1max)) >= s1max) {
        /* String at s2 is longer that destination buffer, C11K does not allow truncation */
        status = SSM_INDEXRANGE;
        len = 0;
    }
    else {
        status = ssm_copy(s1, s1max, s2, len, NULL);
        if (!ssm_success(status)) {
            /* The C11K standard specifies that the output buffer gets am empty string on error */
            len = 0;
        }
    }
    if (s1 != NULL && s1max > len && s1max <= SSM_RSIZE_MAX) {
        s1[len] = '\0';
    }
    return status;
}

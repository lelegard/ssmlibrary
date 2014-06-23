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
 * Implementation of function ssm_strcat_s
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

ssm_errno_t ssm_strcat_s (char* s1, ssm_rsize_t s1max, const char* s2)
{
    /* This implementation is not optimal, the string at s2 is read twice */
    if (s1 == NULL) {
        return SSM_NULLOUT;
    }
    else if (s1max < 1) {
        return SSM_SIZEZERO;
    }
    else if (s1max > SSM_RSIZE_MAX) {
        return SSM_SIZETOOLARGE;
    }
    else {
        const size_t len1 = ssm_cstring_length(s1, s1max);
        const size_t m = s1max - len1;
        size_t len2 = 0;
        ssm_status_t status;
        if (s2 == NULL) {
            /* Normally allowed by ssm_copy (meaning empty buffer) */
            status = SSM_NULLIN;
        }
        else if (m == 0) {
            /* String s1 is not a null-terminated string */
            status = SSM_INDEXRANGE;
        }
        else if ((len2 = ssm_cstring_length(s2, m)) >= m) {
            /* String at s2 is longer that destination buffer, C11K does not allow truncation */
            status = SSM_INDEXRANGE;
            len2 = 0;
        }
        else {
            status = ssm_copy(s1 + len1, s1max - len1, s2, len2, NULL);
            /* The C11K standard specifies that the output buffer gets am empty string on error */
            len2 = ssm_success(status) ? len1 + len2 : 0;
        }
        s1[len2] = '\0';
        return status;
    }
}

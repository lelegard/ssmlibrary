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
 * Implementation of functions ssm_strerror_s and ssm_strerrorlen_s
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

/*
 * Map an error code to a string value.
 */
ssm_errno_t ssm_strerror_s (char* s, ssm_rsize_t maxsize, ssm_errno_t errnum)
{
    if (s == NULL) {
        return SSM_NULLOUT;
    }
    else if (maxsize > SSM_SIZE_MAX) {
        return SSM_SIZETOOLARGE;
    }
    else if (maxsize < 1) {
        return SSM_SIZEZERO;
    }
    else {
        const char* const msg = ssm_status_string ((ssm_status_t)errnum);
        const size_t len = ssm_cstring_length (msg, SSM_SIZE_MAX);
        size_t copied = 0;
        const ssm_status_t status = ssm_copy (s, maxsize - 1, msg, len, &copied);
        s[copied] = 0;
        if (len >= maxsize && maxsize > 3) {
            // Overflow, replace end of message by "..." as specified in C11K.
            s[maxsize-2] = s[maxsize-3] = s[maxsize-4] = '.';
        }
        return status;
    }
}

/*
 * Get the length of the string that maps an error code.
 */
size_t ssm_strerrorlen_s (ssm_errno_t errnum)
{
    return ssm_cstring_length (ssm_status_string ((ssm_status_t)errnum), SSM_SIZE_MAX);
}

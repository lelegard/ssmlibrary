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

ssm_errno_t ssm_strerror_s (char* s, ssm_rsize_t maxsize, ssm_errno_t errnum)
{
    return SSM_BUG; //@@ to be completed
}

size_t ssm_strerrorlen_s (ssm_errno_t errnum)
{
    return 0; //@@ to be completed
}

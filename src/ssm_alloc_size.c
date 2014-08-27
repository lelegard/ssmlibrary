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
 * Dynamic memory allocation size policy.
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

/*
 * Minimum number of bytes to allocate.
 */
#define SSM_SMALLEST_ALLOCATION 8

/*
 * Define a "shrink threshold". If a buffer a reallocated with a smaller size,
 * we keep the previous larger buffer to avoid reallocation and copy, unless
 * the new size is really much smaller (below this threshold), in which case
 * we do the reallocation to save memory.
 */
#define SSM_SHRINK_THRESHOLD_PERCENT 20


/*-----------------------------------------------------------------------------
 * Dynamic memory allocation size policy.
 *-----------------------------------------------------------------------------
 */
size_t _ssm_alloc_size(size_t oldSize, size_t newSize)
{
    /* If the new size is unchanged or smaller but not that much, keep same buffer */
    if (newSize == oldSize || (newSize < oldSize && 100 * newSize > SSM_SHRINK_THRESHOLD_PERCENT * oldSize)) {
        return oldSize;
    }

    /* Apply minimum allocation size */
    if (newSize <= SSM_SMALLEST_ALLOCATION) {
        return SSM_SMALLEST_ALLOCATION;
    }

    /* When increasing the size of the buffer, at least double its size. */
    /* This is a questionable policy, may need to be reviewed. */
    if (newSize > oldSize && newSize < 2 * oldSize) {
        return 2 * oldSize;
    }

    /* Otherwise, grant the requested size. */
    return newSize;
}

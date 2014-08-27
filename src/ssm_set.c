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
 * Implementation of function ssm_set
 *
 * If the symbol _SSM_USE_LIBC is defined, use the standard memset()
 * from the libc. Pros: faster optimized implementation on some platforms.
 * Cons: need to link with an existing libc. Build command:
 * make CFLAGS_EXTRA=-D_SSM_USE_LIBC
 *
 * If the symbol _SSM_OPTIM_SIZE is defined, use a small but less efficient
 * code. Ignored if _SSM_USE_LIBC is already defined. Build command:
 * make CFLAGS_EXTRA=-D_SSM_OPTIM_SIZE
 *
 * If the symbol _SSM_OPTIM_SPEED is defined, use a larger but more efficient
 * code. This is the default if none of the above symbols are defined.
 *
 *-----------------------------------------------------------------------------
 *
 *  Modification history:
 *    2014-01-29 Thierry Lelegard
 *               Original creation.
 *
 *-----------------------------------------------------------------------------
 */

#include "ssm_internal.h"

#if defined(_SSM_USE_LIBC)
#include <string.h>
#endif

ssm_status_t ssm_set(void* dest, size_t destSize, uint8_t value)
{
    /* Filter error and trivial cases */
    if (dest == NULL) {
        return SSM_NULLOUT;
    }
    else if (destSize > SSM_SIZE_MAX) {
        return SSM_SIZETOOLARGE;
    }
    else {
        /* Actual dest size after dealing with potential address computation overflow */
        uint8_t* const endDest = (uint8_t*)ssm_addr_size(dest, destSize);
        const size_t actualDestSize = endDest - (uint8_t*)dest;

#if defined(_SSM_USE_LIBC)

        /* Rely on optimized memset() from libc */
        memset(dest, (int)value, actualDestSize);

#else

        /* Do not use libc. Use our own code. */
        uint8_t* curDest = (uint8_t*)dest;

#if defined(_SSM_OPTIM_SPEED)

        /* Copy a few bytes, until curDest is word-aligned */
        while (curDest < endDest && !_SSM_IS_WORD_ALIGNED(curDest)) {
            *curDest++ = value;
        }

        /* Then copy word by word */
        if (endDest - curDest > _SSM_WORD_SIZE) {

            _ssm_word_t* curWord = (_ssm_word_t*)curDest;
            _ssm_word_t word;

            /* Build a machine word where all bytes have the requested value */
            word = ((_ssm_word_t)value << 8) | value;
            word = (word << 16) | word;
#if _SSM_WORD_BITS == 64
            word = (word << 32) | word;
#endif
            /* Point curDest after end of words area */
            curDest = (uint8_t*)((uintptr_t)endDest & ~(size_t)(_SSM_WORD_SIZE - 1));

            while ((uint8_t*)curWord < curDest) {
                *curWord++ = word;
            }
        }

#endif /* _SSM_OPTIM_SPEED */

        /* Copy the rest of the area byte by byte */
        while (curDest < endDest) {
            *curDest++ = value;
        }

#endif /* _SSM_USE_LIBC */

        return actualDestSize < destSize ? SSM_TRUNCATED : SSM_OK;
    }
}

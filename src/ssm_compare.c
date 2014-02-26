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
 * Implementation of function ssm_compare
 *
 * If the symbol _SSM_USE_LIBC is defined, use the standard memcmp()
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


/*-----------------------------------------------------------------------------
 * Compare 'a' and 'b' bytewise in the byte order of the machine where
 * 'a' and 'b' are known to be different.
 *
 * Reference: Copied from glibc.
 *-----------------------------------------------------------------------------
 */
#if defined(_SSM_OPTIM_SPEED) && !defined(_SSM_USE_LIBC)
#if defined(_SSM_BIG_ENDIAN)
#define CMP_LT_OR_GT(a, b) ((a) > (b) ? 1 : -1)
#else
static int CMP_LT_OR_GT(_ssm_word_t a, _ssm_word_t b)
{
    const uint8_t* srcp1 = (const uint8_t*) &a;
    const uint8_t* srcp2 = (const uint8_t*) &b;
    uint8_t a0;
    uint8_t b0;
    do {
        a0 = *srcp1++;
        b0 = *srcp2++;
    } while (a0 == b0);
    return (int)a0 - (int)b0;
}
#endif /* _SSM_BIG_ENDIAN / _SSM_LITTLE_ENDIAN */
#endif /* _SSM_OPTIM_SPEED */


/*-----------------------------------------------------------------------------
 * Compare blocks at SRCP1 and SRCP2 with LEN words (not LEN bytes!).
 * Both SRCP1 and SRCP2 should be aligned for memory operations on words.
 *
 * Reference: Copied from glibc.
 *-----------------------------------------------------------------------------
 */
#if defined(_SSM_OPTIM_SPEED) && !defined(_SSM_USE_LIBC)
static int _memcmp_common_alignment (uintptr_t srcp1, uintptr_t srcp2, size_t len)
{
    _ssm_word_t a0, a1;
    _ssm_word_t b0, b1;

    switch (len % 4) {
    default: /* Avoid warning about uninitialized local variables.  */
    case 2:
        a0 = ((const _ssm_word_t*)srcp1)[0];
        b0 = ((const _ssm_word_t*)srcp2)[0];
        srcp1 -= 2 * _SSM_WORD_SIZE;
        srcp2 -= 2 * _SSM_WORD_SIZE;
        len += 2;
        goto do1;
    case 3:
        a1 = ((const _ssm_word_t*)srcp1)[0];
        b1 = ((const _ssm_word_t*)srcp2)[0];
        srcp1 -= _SSM_WORD_SIZE;
        srcp2 -= _SSM_WORD_SIZE;
        len += 1;
        goto do2;
    case 0:
        if (len == 0) {
            return 0;
        }
        a0 = ((const _ssm_word_t*)srcp1)[0];
        b0 = ((const _ssm_word_t*)srcp2)[0];
        goto do3;
    case 1:
        a1 = ((const _ssm_word_t*)srcp1)[0];
        b1 = ((const _ssm_word_t*)srcp2)[0];
        srcp1 += _SSM_WORD_SIZE;
        srcp2 += _SSM_WORD_SIZE;
        len -= 1;
        if (len == 0) {
            goto do0;
        }
        /* Fall through.  */
    }

    do {
        a0 = ((const _ssm_word_t*)srcp1)[0];
        b0 = ((const _ssm_word_t*)srcp2)[0];
        if (a1 != b1) {
            return CMP_LT_OR_GT (a1, b1);
        }
    do3:
        a1 = ((const _ssm_word_t*)srcp1)[1];
        b1 = ((const _ssm_word_t*)srcp2)[1];
        if (a0 != b0) {
            return CMP_LT_OR_GT (a0, b0);
        }
    do2:
        a0 = ((const _ssm_word_t*)srcp1)[2];
        b0 = ((const _ssm_word_t*)srcp2)[2];
        if (a1 != b1) {
            return CMP_LT_OR_GT (a1, b1);
        }
    do1:
        a1 = ((const _ssm_word_t*)srcp1)[3];
        b1 = ((const _ssm_word_t*)srcp2)[3];
        if (a0 != b0) {
            return CMP_LT_OR_GT (a0, b0);
        }
        srcp1 += 4 * _SSM_WORD_SIZE;
        srcp2 += 4 * _SSM_WORD_SIZE;
        len -= 4;
    } while (len != 0);

    /* This is the right position for do0. Please don't move it into the loop. */
 do0:
    if (a1 != b1) {
        return CMP_LT_OR_GT (a1, b1);
    }
    return 0;
}
#endif


/*-----------------------------------------------------------------------------
 * Compare blocks at SRCP1 and SRCP2 with LEN words (not LEN bytes!).
 * SRCP2 should be aligned on words, but SRCP1 *should be unaligned.
 *
 * Reference: Copied from glibc.
 *-----------------------------------------------------------------------------
 */
#if defined(_SSM_OPTIM_SPEED) && !defined(_SSM_USE_LIBC)
static int _memcmp_not_common_alignment (uintptr_t srcp1, uintptr_t srcp2, size_t len)
{
    _ssm_word_t a0, a1, a2, a3;
    _ssm_word_t b0, b1, b2, b3;
    _ssm_word_t x;
    int shl, shr;

    /* Calculate how to shift a word read at the memory operation aligned srcp1 to make it aligned for comparison. */
    shl = 8 * (srcp1 % _SSM_WORD_SIZE);
    shr = 8 * _SSM_WORD_SIZE - shl;

    /* Make SRCP1 aligned by rounding it down to the beginning of the word it points in the middle of. */
    srcp1 &= -_SSM_WORD_SIZE;

    switch (len % 4) {
    default: /* Avoid warning about uninitialized local variables.  */
    case 2:
        a1 = ((const _ssm_word_t*)srcp1)[0];
        a2 = ((const _ssm_word_t*)srcp1)[1];
        b2 = ((const _ssm_word_t*)srcp2)[0];
        srcp1 -= 1 * _SSM_WORD_SIZE;
        srcp2 -= 2 * _SSM_WORD_SIZE;
        len += 2;
        goto do1;
    case 3:
        a0 = ((const _ssm_word_t*)srcp1)[0];
        a1 = ((const _ssm_word_t*)srcp1)[1];
        b1 = ((const _ssm_word_t*)srcp2)[0];
        srcp2 -= 1 * _SSM_WORD_SIZE;
        len += 1;
        goto do2;
    case 0:
        if (len == 0) {
            return 0;
        }
        a3 = ((const _ssm_word_t*)srcp1)[0];
        a0 = ((const _ssm_word_t*)srcp1)[1];
        b0 = ((const _ssm_word_t*)srcp2)[0];
        srcp1 += 1 * _SSM_WORD_SIZE;
        goto do3;
    case 1:
        a2 = ((const _ssm_word_t*)srcp1)[0];
        a3 = ((const _ssm_word_t*)srcp1)[1];
        b3 = ((const _ssm_word_t*)srcp2)[0];
        srcp1 += 2 * _SSM_WORD_SIZE;
        srcp2 += 1 * _SSM_WORD_SIZE;
        len -= 1;
        if (len == 0) {
            goto do0;
        }
        /* Fall through.  */
    }

    do {
        a0 = ((const _ssm_word_t*)srcp1)[0];
        b0 = ((const _ssm_word_t*)srcp2)[0];
        x = _SSM_MERGE_WORDS(a2, shl, a3, shr);
        if (x != b3) {
            return CMP_LT_OR_GT (x, b3);
        }
    do3:
        a1 = ((const _ssm_word_t*)srcp1)[1];
        b1 = ((const _ssm_word_t*)srcp2)[1];
        x = _SSM_MERGE_WORDS(a3, shl, a0, shr);
        if (x != b0) {
            return CMP_LT_OR_GT (x, b0);
        }
    do2:
        a2 = ((const _ssm_word_t*)srcp1)[2];
        b2 = ((const _ssm_word_t*)srcp2)[2];
        x = _SSM_MERGE_WORDS(a0, shl, a1, shr);
        if (x != b1) {
            return CMP_LT_OR_GT (x, b1);
        }
    do1:
        a3 = ((const _ssm_word_t*)srcp1)[3];
        b3 = ((const _ssm_word_t*)srcp2)[3];
        x = _SSM_MERGE_WORDS(a1, shl, a2, shr);
        if (x != b2) {
            return CMP_LT_OR_GT (x, b2);
        }
        srcp1 += 4 * _SSM_WORD_SIZE;
        srcp2 += 4 * _SSM_WORD_SIZE;
        len -= 4;
    } while (len != 0);

    /* This is the right position for do0. Please don't move it into the loop. */
 do0:
    x = _SSM_MERGE_WORDS(a2, shl, a3, shr);
    if (x != b3) {
        return CMP_LT_OR_GT (x, b3);
    }
    return 0;
}
#endif


/*-----------------------------------------------------------------------------
 * Public API of the memory comparison function.
 *-----------------------------------------------------------------------------
 */
ssm_status_t ssm_compare(const void* addr1, size_t size1, const void* addr2, size_t size2)
{
    /* Filter error and trivial cases */
    if (size1 > SSM_SIZE_MAX || size2 > SSM_SIZE_MAX) {
        return SSM_SIZETOOLARGE;
    }
    else if (addr1 == NULL || size1 == 0) {
        return addr2 == NULL || size2 == 0 ? SSM_EQUAL : SSM_LOWER;
    }
    else if (addr2 == NULL || size2 == 0) {
        return SSM_GREATER;
    }
    else {
        /* Actual sizes after dealing with potential address computation overflow */
        uint8_t* const end1 = (uint8_t*)ssm_addr_size(addr1, size1);
        uint8_t* const end2 = (uint8_t*)ssm_addr_size(addr2, size2);
        const size_t actualSize1 = end1 - (uint8_t*)addr1;
        const size_t actualSize2 = end2 - (uint8_t*)addr2;
        size_t remain = _ssm_min_size_t(actualSize1, actualSize2);
        int comp;

#if defined(_SSM_USE_LIBC)

        /* Rely on optimized memcmp() from libc */
        comp = memcmp(addr1, addr2, remain);

#else

        /* Do not use libc. Use our own code. */
        /* Optimizations for speed are copied from glibc. */
        uint8_t* cur1 = (uint8_t*)addr1;
        uint8_t* cur2 = (uint8_t*)addr2;
        comp = 0;

#if defined(_SSM_OPTIM_SPEED)

        /* Compare a few bytes, until cur2 is word-aligned */
        while (comp == 0 && remain > 0 && !_SSM_IS_WORD_ALIGNED(cur2)) {
            comp = (int)*cur1++ - (int)*cur2++;
            --remain;
        }

        /* cur2 is now aligned on a word. */
        /* cur1 alignment determines if we can do a simple aligned compare or need to shuffle bits. */
        if (remain > 0 && comp == 0) {
            if (_SSM_IS_WORD_ALIGNED(cur1)) {
                comp = _memcmp_common_alignment ((uintptr_t)cur1, (uintptr_t)cur2, remain / _SSM_WORD_SIZE);
            }
            else {
                comp = _memcmp_not_common_alignment ((uintptr_t)cur1, (uintptr_t)cur2, remain / _SSM_WORD_SIZE);
            }
            /* Number of bytes remaining in the interval [0.._SSM_WORD_SIZE-1]. */
            cur1 += remain & -_SSM_WORD_SIZE;
            cur2 += remain & -_SSM_WORD_SIZE;
            remain %= _SSM_WORD_SIZE;
        }

#endif /* _SSM_OPTIM_SPEED */

        /* Copy the rest of the area byte by byte */
        while (comp == 0 && remain > 0) {
            comp = (int)*cur1++ - (int)*cur2++;
            --remain;
        }

#endif /* _SSM_USE_LIBC */

        if (comp != 0) {
            return comp < 0 ? SSM_LOWER : SSM_GREATER;
        }
        else if (actualSize1 == actualSize2) {
            /* Identical content and size */
            return SSM_EQUAL;
        }
        else {
            /* Identical content but distinct sizes */
            return actualSize1 < actualSize2 ? SSM_LOWER : SSM_GREATER;
        }
    }
}

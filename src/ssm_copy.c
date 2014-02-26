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
 * Implementation of function ssm_copy
 *
 * If the symbol _SSM_USE_LIBC is defined, use the standard memmove()
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
 *    2014-01-22 Thierry Lelegard
 *               Original creation, with optimizations copied from GNU C
 *               Library version of strlen() with same LGPL license.
 *
 *-----------------------------------------------------------------------------
 */

#include "ssm_internal.h"

#if defined(_SSM_USE_LIBC)
#include <string.h>
#endif


/*-----------------------------------------------------------------------------
 * Copy block beginning at SRCP to block beginning at DSTP with LEN words
 * (not LEN bytes!). Both SRCP and DSTP should be aligned for memory
 * operations on words.
 *
 * Reference: Copied from glibc.
 *-----------------------------------------------------------------------------
 */
#if defined(_SSM_OPTIM_SPEED) && !defined(_SSM_USE_LIBC)
static void _wordcopy_fwd_aligned (uintptr_t dstp, uintptr_t srcp, size_t len)
{
    _ssm_word_t a0 = 0;
    _ssm_word_t a1 = 0;

    switch (len % 8) {
    case 2:
        a0 = ((const _ssm_word_t*)srcp)[0];
        srcp -= 6 * _SSM_WORD_SIZE;
        dstp -= 7 * _SSM_WORD_SIZE;
        len += 6;
        goto do1;
    case 3:
        a1 = ((const _ssm_word_t*)srcp)[0];
        srcp -= 5 * _SSM_WORD_SIZE;
        dstp -= 6 * _SSM_WORD_SIZE;
        len += 5;
        goto do2;
    case 4:
        a0 = ((const _ssm_word_t*)srcp)[0];
        srcp -= 4 * _SSM_WORD_SIZE;
        dstp -= 5 * _SSM_WORD_SIZE;
        len += 4;
        goto do3;
    case 5:
        a1 = ((const _ssm_word_t*)srcp)[0];
        srcp -= 3 * _SSM_WORD_SIZE;
        dstp -= 4 * _SSM_WORD_SIZE;
        len += 3;
        goto do4;
    case 6:
        a0 = ((const _ssm_word_t*)srcp)[0];
        srcp -= 2 * _SSM_WORD_SIZE;
        dstp -= 3 * _SSM_WORD_SIZE;
        len += 2;
        goto do5;
    case 7:
        a1 = ((const _ssm_word_t*)srcp)[0];
        srcp -= 1 * _SSM_WORD_SIZE;
        dstp -= 2 * _SSM_WORD_SIZE;
        len += 1;
        goto do6;

    case 0:
        if (len == 0) {
            return;
        }
        a0 = ((const _ssm_word_t*)srcp)[0];
        srcp -= 0 * _SSM_WORD_SIZE;
        dstp -= 1 * _SSM_WORD_SIZE;
        goto do7;
    case 1:
        a1 = ((const _ssm_word_t*)srcp)[0];
     /* srcp -=-1 * _SSM_WORD_SIZE; --> signed/unsigned mismatch */
        srcp += 1 * _SSM_WORD_SIZE;
        dstp -= 0 * _SSM_WORD_SIZE;
        len -= 1;
        if (len == 0) {
            goto do0;
        }
        goto do8; /* No-op. */
    }

    do {
    do8:
        a0 = ((const _ssm_word_t*)srcp)[0];
        ((_ssm_word_t*)dstp)[0] = a1;
    do7:
        a1 = ((const _ssm_word_t*)srcp)[1];
        ((_ssm_word_t*)dstp)[1] = a0;
    do6:
        a0 = ((const _ssm_word_t*)srcp)[2];
        ((_ssm_word_t*)dstp)[2] = a1;
    do5:
        a1 = ((const _ssm_word_t*)srcp)[3];
        ((_ssm_word_t*)dstp)[3] = a0;
    do4:
        a0 = ((const _ssm_word_t*)srcp)[4];
        ((_ssm_word_t*)dstp)[4] = a1;
    do3:
        a1 = ((const _ssm_word_t*)srcp)[5];
        ((_ssm_word_t*)dstp)[5] = a0;
    do2:
        a0 = ((const _ssm_word_t*)srcp)[6];
        ((_ssm_word_t*)dstp)[6] = a1;
    do1:
        a1 = ((const _ssm_word_t*)srcp)[7];
        ((_ssm_word_t*)dstp)[7] = a0;

        srcp += 8 * _SSM_WORD_SIZE;
        dstp += 8 * _SSM_WORD_SIZE;
        len -= 8;
    } while (len != 0);

    /* This is the right position for do0. Please don't move it into the loop. */
 do0:
    ((_ssm_word_t*)dstp)[0] = a1;
}
#endif


/*-----------------------------------------------------------------------------
 * Copy block beginning at SRCP to block beginning at DSTP with LEN words
 * (not LEN bytes!). DSTP should be aligned for memory operations on words,
 * but SRCP must *not* be aligned.
 *
 * Reference: Copied from glibc.
 *-----------------------------------------------------------------------------
 */
#if defined(_SSM_OPTIM_SPEED) && !defined(_SSM_USE_LIBC)
static void _wordcopy_fwd_dest_aligned (uintptr_t dstp, uintptr_t srcp, size_t len)
{
    _ssm_word_t a0 = 0;
    _ssm_word_t a1 = 0;
    _ssm_word_t a2 = 0;
    _ssm_word_t a3 = 0;
    int sh_1, sh_2;

    /* Calculate how to shift a word read at the memory operation aligned srcp to make it aligned for copy. */
    sh_1 = 8 * (srcp % _SSM_WORD_SIZE);
    sh_2 = 8 * _SSM_WORD_SIZE - sh_1;

    /* Make SRCP aligned by rounding it down to the beginning of the word it points in the middle of. */
    srcp &= -_SSM_WORD_SIZE;

    switch (len % 4) {
    case 2:
        a1 = ((const _ssm_word_t*)srcp)[0];
        a2 = ((const _ssm_word_t*)srcp)[1];
        srcp -= 1 * _SSM_WORD_SIZE;
        dstp -= 3 * _SSM_WORD_SIZE;
        len += 2;
        goto do1;
    case 3:
        a0 = ((const _ssm_word_t*)srcp)[0];
        a1 = ((const _ssm_word_t*)srcp)[1];
        srcp -= 0 * _SSM_WORD_SIZE;
        dstp -= 2 * _SSM_WORD_SIZE;
        len += 1;
        goto do2;
    case 0:
        if (len == 0) {
            return;
        }
        a3 = ((const _ssm_word_t*)srcp)[0];
        a0 = ((const _ssm_word_t*)srcp)[1];
     /* srcp -=-1 * _SSM_WORD_SIZE; --> signed/unsigned mismatch */
        srcp += 1 * _SSM_WORD_SIZE;
        dstp -= 1 * _SSM_WORD_SIZE;
        len += 0;
        goto do3;
    case 1:
        a2 = ((const _ssm_word_t*)srcp)[0];
        a3 = ((const _ssm_word_t*)srcp)[1];
     /* srcp -=-2 * _SSM_WORD_SIZE; --> signed/unsigned mismatch */
        srcp += 2 * _SSM_WORD_SIZE;
        dstp -= 0 * _SSM_WORD_SIZE;
        len -= 1;
        if (len == 0) {
            goto do0;
        }
        goto do4;			/* No-op. */
    }

    do {
    do4:
        a0 = ((const _ssm_word_t*)srcp)[0];
        ((_ssm_word_t*)dstp)[0] = _SSM_MERGE_WORDS (a2, sh_1, a3, sh_2);
    do3:
        a1 = ((const _ssm_word_t*)srcp)[1];
        ((_ssm_word_t*)dstp)[1] = _SSM_MERGE_WORDS (a3, sh_1, a0, sh_2);
    do2:
        a2 = ((const _ssm_word_t*)srcp)[2];
        ((_ssm_word_t*)dstp)[2] = _SSM_MERGE_WORDS (a0, sh_1, a1, sh_2);
    do1:
        a3 = ((const _ssm_word_t*)srcp)[3];
        ((_ssm_word_t*)dstp)[3] = _SSM_MERGE_WORDS (a1, sh_1, a2, sh_2);

        srcp += 4 * _SSM_WORD_SIZE;
        dstp += 4 * _SSM_WORD_SIZE;
        len -= 4;
    } while (len != 0);

    /* This is the right position for do0. Please don't move it into the loop. */
 do0:
    ((_ssm_word_t*)dstp)[0] = _SSM_MERGE_WORDS (a2, sh_1, a3, sh_2);
}
#endif


/*-----------------------------------------------------------------------------
 * Copy block finishing right before SRCP to block finishing right before DSTP
 * with LEN words (not LEN bytes!). Both SRCP and DSTP should be aligned for
 * memory operations on words.
 *
 * Reference: Copied from glibc.
 *-----------------------------------------------------------------------------
 */
#if defined(_SSM_OPTIM_SPEED) && !defined(_SSM_USE_LIBC)
static void _wordcopy_bwd_aligned (uintptr_t dstp, uintptr_t srcp, size_t len)
{
    _ssm_word_t a0 = 0;
    _ssm_word_t a1 = 0;

    switch (len % 8) {
    case 2:
        srcp -= 2 * _SSM_WORD_SIZE;
        dstp -= 1 * _SSM_WORD_SIZE;
        a0 = ((const _ssm_word_t*)srcp)[1];
        len += 6;
        goto do1;
    case 3:
        srcp -= 3 * _SSM_WORD_SIZE;
        dstp -= 2 * _SSM_WORD_SIZE;
        a1 = ((const _ssm_word_t*)srcp)[2];
        len += 5;
        goto do2;
    case 4:
        srcp -= 4 * _SSM_WORD_SIZE;
        dstp -= 3 * _SSM_WORD_SIZE;
        a0 = ((const _ssm_word_t*)srcp)[3];
        len += 4;
        goto do3;
    case 5:
        srcp -= 5 * _SSM_WORD_SIZE;
        dstp -= 4 * _SSM_WORD_SIZE;
        a1 = ((const _ssm_word_t*)srcp)[4];
        len += 3;
        goto do4;
    case 6:
        srcp -= 6 * _SSM_WORD_SIZE;
        dstp -= 5 * _SSM_WORD_SIZE;
        a0 = ((const _ssm_word_t*)srcp)[5];
        len += 2;
        goto do5;
    case 7:
        srcp -= 7 * _SSM_WORD_SIZE;
        dstp -= 6 * _SSM_WORD_SIZE;
        a1 = ((const _ssm_word_t*)srcp)[6];
        len += 1;
        goto do6;

    case 0:
        if (len == 0) {
            return;
        }
        srcp -= 8 * _SSM_WORD_SIZE;
        dstp -= 7 * _SSM_WORD_SIZE;
        a0 = ((const _ssm_word_t*)srcp)[7];
        goto do7;
    case 1:
        srcp -= 9 * _SSM_WORD_SIZE;
        dstp -= 8 * _SSM_WORD_SIZE;
        a1 = ((const _ssm_word_t*)srcp)[8];
        len -= 1;
        if (len == 0) {
            goto do0;
        }
        goto do8; /* No-op. */
    }

    do {
    do8:
        a0 = ((const _ssm_word_t*)srcp)[7];
        ((_ssm_word_t*)dstp)[7] = a1;
    do7:
        a1 = ((const _ssm_word_t*)srcp)[6];
        ((_ssm_word_t*)dstp)[6] = a0;
    do6:
        a0 = ((const _ssm_word_t*)srcp)[5];
        ((_ssm_word_t*)dstp)[5] = a1;
    do5:
        a1 = ((const _ssm_word_t*)srcp)[4];
        ((_ssm_word_t*)dstp)[4] = a0;
    do4:
        a0 = ((const _ssm_word_t*)srcp)[3];
        ((_ssm_word_t*)dstp)[3] = a1;
    do3:
        a1 = ((const _ssm_word_t*)srcp)[2];
        ((_ssm_word_t*)dstp)[2] = a0;
    do2:
        a0 = ((const _ssm_word_t*)srcp)[1];
        ((_ssm_word_t*)dstp)[1] = a1;
    do1:
        a1 = ((const _ssm_word_t*)srcp)[0];
        ((_ssm_word_t*)dstp)[0] = a0;

        srcp -= 8 * _SSM_WORD_SIZE;
        dstp -= 8 * _SSM_WORD_SIZE;
        len -= 8;
    } while (len != 0);

    /* This is the right position for do0. Please don't move it into the loop. */
 do0:
    ((_ssm_word_t*)dstp)[7] = a1;
}
#endif


/*-----------------------------------------------------------------------------
 * Copy block finishing right before SRCP to block finishing right before DSTP
 * with LEN words (not LEN bytes!). DSTP should be aligned for memory
 * operations on words, but SRCP must *not* be aligned.
 *
 * Reference: Copied from glibc.
 *-----------------------------------------------------------------------------
 */
#if defined(_SSM_OPTIM_SPEED) && !defined(_SSM_USE_LIBC)
static void _wordcopy_bwd_dest_aligned (uintptr_t dstp, uintptr_t srcp, size_t len)
{
    _ssm_word_t a0 = 0;
    _ssm_word_t a1 = 0;
    _ssm_word_t a2 = 0;
    _ssm_word_t a3 = 0;
    int sh_1, sh_2;

    /* Calculate how to shift a word read at the memory operation aligned srcp to make it aligned for copy. */
    sh_1 = 8 * (srcp % _SSM_WORD_SIZE);
    sh_2 = 8 * _SSM_WORD_SIZE - sh_1;

    /* Make srcp aligned by rounding it down to the beginning of the word it points in the middle of. */
    srcp &= -_SSM_WORD_SIZE;
    srcp += _SSM_WORD_SIZE;

    switch (len % 4) {
    case 2:
        srcp -= 3 * _SSM_WORD_SIZE;
        dstp -= 1 * _SSM_WORD_SIZE;
        a2 = ((const _ssm_word_t*)srcp)[2];
        a1 = ((const _ssm_word_t*)srcp)[1];
        len += 2;
        goto do1;
    case 3:
        srcp -= 4 * _SSM_WORD_SIZE;
        dstp -= 2 * _SSM_WORD_SIZE;
        a3 = ((const _ssm_word_t*)srcp)[3];
        a2 = ((const _ssm_word_t*)srcp)[2];
        len += 1;
        goto do2;
    case 0:
        if (len == 0) {
            return;
        }
        srcp -= 5 * _SSM_WORD_SIZE;
        dstp -= 3 * _SSM_WORD_SIZE;
        a0 = ((const _ssm_word_t*)srcp)[4];
        a3 = ((const _ssm_word_t*)srcp)[3];
        goto do3;
    case 1:
        srcp -= 6 * _SSM_WORD_SIZE;
        dstp -= 4 * _SSM_WORD_SIZE;
        a1 = ((const _ssm_word_t*)srcp)[5];
        a0 = ((const _ssm_word_t*)srcp)[4];
        len -= 1;
        if (len == 0) {
            goto do0;
        }
        goto do4; /* No-op. */
    }

    do {
    do4:
        a3 = ((const _ssm_word_t*)srcp)[3];
        ((_ssm_word_t*)dstp)[3] = _SSM_MERGE_WORDS (a0, sh_1, a1, sh_2);
    do3:
        a2 = ((const _ssm_word_t*)srcp)[2];
        ((_ssm_word_t*)dstp)[2] = _SSM_MERGE_WORDS (a3, sh_1, a0, sh_2);
    do2:
        a1 = ((const _ssm_word_t*)srcp)[1];
        ((_ssm_word_t*)dstp)[1] = _SSM_MERGE_WORDS (a2, sh_1, a3, sh_2);
    do1:
        a0 = ((const _ssm_word_t*)srcp)[0];
        ((_ssm_word_t*)dstp)[0] = _SSM_MERGE_WORDS (a1, sh_1, a2, sh_2);

        srcp -= 4 * _SSM_WORD_SIZE;
        dstp -= 4 * _SSM_WORD_SIZE;
        len -= 4;
    } while (len != 0);

    /* This is the right position for do0. Please don't move it into the loop. */
 do0:
    ((_ssm_word_t*)dstp)[3] = _SSM_MERGE_WORDS (a0, sh_1, a1, sh_2);
}
#endif


/*-----------------------------------------------------------------------------
 * Public API of the memory copy function.
 *-----------------------------------------------------------------------------
 */
ssm_status_t ssm_copy(void* dest, size_t destSize, const void* src, size_t srcSize, size_t* copiedSize)
{
    /* Filter error and trivial cases */
    if (dest == NULL) {
        return SSM_NULLOUT;
    }
    else if (destSize > SSM_SIZE_MAX || srcSize > SSM_SIZE_MAX) {
        return SSM_SIZETOOLARGE;
    }
    else if (src == NULL) {
        if (copiedSize != NULL) {
            *copiedSize = 0;
        }
        return SSM_OK;
    }
    else {
        /* Actual dest and src sizes after dealing with potential address computation overflow */
        char* const endDest = (char*)ssm_addr_size(dest, destSize);
        const char* const endSrc = (const char*)ssm_addr_size(src, srcSize);
        const size_t actualDestSize = endDest - (char*)dest;
        const size_t actualSrcSize = endSrc - (const char*)src;
        const size_t actualSize = _ssm_min_size_t(actualDestSize, actualSrcSize);

#if defined(_SSM_USE_LIBC)

        /* Rely on optimized memmove() from libc */
        memmove(dest, src, actualSize);

#else

        /* Do not use libc. Use our own code. */
        /* Optimizations for speed are copied from glibc. */
        if (src < dest && (const void*)endSrc > dest) {

            /* Overlapping areas with beginning of dest in the middle of src. */
            /* Must copy from end to beginning. */
            const char* curSrc = (const char*)src + actualSize;
            char* curDest = (char*)dest + actualSize;
            size_t remain = actualSize;

#if defined(_SSM_OPTIM_SPEED)

            /* Copy a few bytes, until curDest is word-aligned */
            while (remain > 0 && !_SSM_IS_WORD_ALIGNED(curDest)) {
                *--curDest = *--curSrc;
                --remain;
            }

            /* Copy word by word */
            if (_SSM_IS_WORD_ALIGNED(curSrc)) {
                /* Both src and dest are word-aligned */
                _wordcopy_bwd_aligned((uintptr_t)curDest, (uintptr_t)curSrc, remain / _SSM_WORD_SIZE);
            }
            else {
                /* Only dest is word-aligned, src is not */
                _wordcopy_bwd_dest_aligned((uintptr_t)curDest, (uintptr_t)curSrc, remain / _SSM_WORD_SIZE);
            }
            curSrc -= remain & ~(size_t)(_SSM_WORD_SIZE - 1);
            curDest -= remain & ~(size_t)(_SSM_WORD_SIZE - 1);
            remain &= _SSM_WORD_SIZE - 1;

#endif /* _SSM_OPTIM_SPEED */

            /* Copy the rest of the area byte by byte */
            while (remain > 0) {
                *--curDest = *--curSrc;
                --remain;
            }
        }
        else if (src != dest) {

            /* Copy from beginning to end. Areas may overlap. */
            const char* curSrc = (const char*)src;
            char* curDest = (char*)dest;
            size_t remain = actualSize;

#if defined(_SSM_OPTIM_SPEED)

            /* Copy a few bytes, until curDest is word-aligned */
            while (remain > 0 && !_SSM_IS_WORD_ALIGNED(curDest)) {
                *curDest++ = *curSrc++;
                --remain;
            }

            /* Copy word by word */
            if (_SSM_IS_WORD_ALIGNED(curSrc)) {
                /* Both src and dest are word-aligned */
                _wordcopy_fwd_aligned((uintptr_t)curDest, (uintptr_t)curSrc, remain / _SSM_WORD_SIZE);
            }
            else {
                /* Only dest is word-aligned, src is not */
                _wordcopy_fwd_dest_aligned((uintptr_t)curDest, (uintptr_t)curSrc, remain / _SSM_WORD_SIZE);
            }
            curSrc += remain & ~(size_t)(_SSM_WORD_SIZE - 1);
            curDest += remain & ~(size_t)(_SSM_WORD_SIZE - 1);
            remain &= _SSM_WORD_SIZE - 1;

#endif /* _SSM_OPTIM_SPEED */

            /* Copy the rest of the area byte by byte */
            while (remain > 0) {
                *curDest++ = *curSrc++;
                --remain;
            }
        }

#endif /* _SSM_USE_LIBC */

        if (copiedSize != NULL) {
            *copiedSize = actualSize;
        }
        return actualSize < srcSize ? SSM_TRUNCATED : SSM_OK;
    }
}

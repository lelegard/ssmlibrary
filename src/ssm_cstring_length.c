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
 * Implementation of function ssm_cstring_length
 *
 * If the symbol _SSM_OPTIM_SIZE is defined, use a small but less efficient
 * code. Build command:
 * make CFLAGS_EXTRA=-D_SSM_OPTIM_SIZE
 *
 * If the symbol _SSM_OPTIM_SPEED is defined, use a larger but more efficient
 * code. This is the default if the above symbol is defined.
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

size_t ssm_cstring_length(const char* str, size_t maxSize)
{
    const char* const endStr = (const char*)ssm_addr_size(str, maxSize);
    const char* curStr;

#if defined(_SSM_OPTIM_SPEED)
    const _ssm_word_t* wordPtr;
    size_t size;
#if _SSM_WORD_BITS == 32
    static const _ssm_word_t himagic = _SSM_WORD_C(0x80808080);
    static const _ssm_word_t lomagic = _SSM_WORD_C(0x01010101);
#elif _SSM_WORD_BITS == 64
    static const _ssm_word_t himagic = _SSM_WORD_C(0x8080808080808080);
    static const _ssm_word_t lomagic = _SSM_WORD_C(0x0101010101010101);
#else
    #error "Unsupported platform, update this source file"
#endif
#endif /* _SSM_OPTIM_SPEED */

    /*
     * Filter implicit empty or oversized string.
     */
    if (str == NULL || maxSize > SSM_SIZE_MAX) {
        return 0;
    }

#if defined(_SSM_OPTIM_SIZE)

    /*
     * Simple, unefficient but compact code.
     */
    for (curStr = str; curStr < endStr && *curStr != '\0'; ++curStr) {
    }
    return curStr - str;

#else

    /*
     * Optimizations for speed copied from glibc.
     * Handle the first few characters by reading one character at a time.
     * Do this until curStr is aligned on a word boundary.
     */
    for (curStr = str; !_SSM_IS_WORD_ALIGNED(curStr); ++curStr) {
        if (curStr >= endStr || *curStr == '\0') {
            return curStr - str;
        }
    }

    /*
     * Tricky optimization copied from the GNU C Library.
     *
     * Instead of the traditional loop which tests each character,
     * we will test a machine word at a time.  The tricky part is testing
     * if *any of the bytes in the word in question are zero.
     *
     * We use the himagic and lomagic values. Example on 32-bit systems:
     * Bits 31, 24, 16, and 8 of this number are zero.  Call these bits
     * the "holes."  Note that there is a hole just to the left of
     * each byte, with an extra at the end:
     *
     * bits:  01111110 11111110 11111110 11111111
     *
     * The 1-bits make sure that carries propagate to the next 0-bit.
     * The 0-bits provide holes for carries to fall into.
     */
    size = 0;
    for (wordPtr = (const _ssm_word_t*)curStr; wordPtr < (const _ssm_word_t*)endStr; ++wordPtr) {

        /*
         * Load one complete machine word.
         *
         * WARNING: valgrind may complain about "unitialized value" here but this
         * is a false positive. Let's understand why it may complain and why this
         * is not a problem. We load a complete word from memory, the string's
         * trailing zero possibly within. Any byte *after* this zero is not part
         * of the string. If the memory after the string has not been previously
         * initialized, valgrind will complain, and it is right to do so. However,
         * we are also doing right because 1) we do not make any decision based on
         * any byte after the trailing zero and 2) even if the bytes after the string
         * have not been officially allocated, the smallest memory unit to allocate
         * is a page and a page contains an integral number of words and accessing
         * this last word is always physically valid.
         */
        const _ssm_word_t word = *wordPtr;

        /* Check if *any* byte is zero within this word */
        if (((word - lomagic) & ~word & himagic) != 0) {
            /* Which of the bytes was the zero?  If none of them were, it was a misfire; continue the search. */
            const char* const cp = (const char*)wordPtr;
            if (cp[0] == 0) {
                break;
            }
            if (cp[1] == 0) {
                size = 1;
                break;
            }
            if (cp[2] == 0) {
                size = 2;
                break;
            }
            if (cp[3] == 0) {
                size = 3;
                break;
            }
#if _SSM_WORD_BITS == 64
            if (cp[4] == 0) {
                size = 4;
                break;
            }
            if (cp[5] == 0) {
                size = 5;
                break;
            }
            if (cp[6] == 0) {
                size = 6;
                break;
            }
            if (cp[7] == 0) {
                size = 7;
                break;
            }
#endif
        }
    }

    /*
     * Compute actual string length.
     * We need to handle the case where the null character is after maxSize but in the same word as maxSize.
     */
    size = (const char*)wordPtr + size - str;
    return _ssm_min_size_t(size, maxSize);

#endif /* _SSM_OPTIM_SIZE / _SSM_OPTIM_SPEED */
}

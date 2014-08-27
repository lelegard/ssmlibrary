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
 * Private header (internal to SSM library).
 *
 *-----------------------------------------------------------------------------
 *
 *  Modification history:
 *    2014-01-10 Thierry Lelegard
 *               Original creation.
 *
 *-----------------------------------------------------------------------------
 */

#if !defined(_SSM_INTERNAL_H)
#define _SSM_INTERNAL_H 1

#include "ssm.h"

/*
 * Optimization for speed is the default.
 * Use "make CFLAGS_EXTRA=-D_SSM_OPTIM_SIZE" to optimize for code size.
 */
#if !defined(_SSM_OPTIM_SIZE) && !defined(_SSM_OPTIM_SPEED)
    #define _SSM_OPTIM_SPEED 1
#endif

/*
 * System endianness.
 */
#if defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__ /* GCC 4.6+ style */
    #define _SSM_LITTLE_ENDIAN 1
#elif defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__ /* GCC 4.6+ style */
    #define _SSM_BIG_ENDIAN 1
#elif defined(__LITTLE_ENDIAN__) ||                             \
    defined(__i386__) || defined(_M_IX86) || defined(__i386) || \
    defined(__amd64) || defined(__amd64__) ||                   \
    defined(__x86_64__) || defined(__x86_64) ||                 \
    defined(_M_AMD64) || defined(_M_X64) ||                     \
    defined(__stxp70__) || defined(__STxP70__) ||               \
    defined(__ia64__) || defined(__ia64) ||                     \
    defined(__alpha__) || defined(__alpha)
    #define _SSM_LITTLE_ENDIAN 1
#elif defined(__BIG_ENDIAN__) ||                                \
    defined(__powerpc__) || defined(__powerpc) ||               \
    defined(__powerpc64__) || defined(__powerpc64) ||           \
    defined(__sparc__) || defined(__sparc)
    #define _SSM_BIG_ENDIAN 1
#else
    #error "unknown endian"
#endif

/*
 * Define the "native" word type of the system. Accessing a word value
 * at a word-aligned address is likely to give the best performance.
 */
#if defined(__KERNEL__)
    /*
     * Kernel mode, the number of standard definitions is reduced, need to check the CPU.
     * We assume that the compiler is GCC.
     */
    #if defined(CONFIG_X86_32) || defined(CONFIG_ARM_32)
        typedef uint32_t _ssm_word_t;
        #define _SSM_WORD_BITS 32
        #define _SSM_WORD_C(c) (c ## U)
    #elif defined(__amd64__) || defined(__x86_64__) || defined(__aarch64__) || defined(__arm64__)
        typedef uint64_t _ssm_word_t;
        #define _SSM_WORD_BITS 64
        #define _SSM_WORD_C(c) (c ## UL)
    #elif defined(__i386__) || defined(__arm__)
        typedef uint32_t _ssm_word_t;
        #define _SSM_WORD_BITS 32
        #define _SSM_WORD_C(c) (c ## U)
    #endif
#else
    /*
     * User mode, more definitions are available in stdint.h according to ISO/IEC 9899:1999 (C99).
     * Check existence anyway in case of non-compliant version of stdint.h.
     * We make no assumption about the compiler brand.
     */
    #if defined(UINTPTR_MAX) && defined(UINT32_MAX) && defined(UINT32_C) && (UINTPTR_MAX == UINT32_MAX)
        typedef uint32_t _ssm_word_t;
        #define _SSM_WORD_BITS 32
        #define _SSM_WORD_C(c) UINT32_C(c)
    #elif defined(UINTPTR_MAX) && defined(UINT64_MAX) && defined(UINT64_C) && (UINTPTR_MAX == UINT64_MAX)
        typedef uint64_t _ssm_word_t;
        #define _SSM_WORD_BITS 64
        #define _SSM_WORD_C(c) UINT64_C(c)
    #endif
#endif

/*
 * On unexpected platforms, this file needs to be updated. This may happen in case of:
 * - Linux kernel on CPU's other than Intel and ARM.
 * - Non-C99-compliant version of stdint.h.
 */
#if !defined(_SSM_WORD_BITS)
    #error "Unsupported platform, update this header file"
#endif

/*
 * Word size in bytes.
 */
#define _SSM_WORD_SIZE (_SSM_WORD_BITS / 8)

/*
 * Get the offset of an address from the previous word boundary.
 */
#define _SSM_WORD_ALIGNMENT(addr) ((uintptr_t)(addr) & (_SSM_WORD_SIZE - 1))

/*
 * Check if an address is aligned on a word boundary.
 */
#define _SSM_IS_WORD_ALIGNED(addr) (_SSM_WORD_ALIGNMENT(addr) == 0)

/*
 * Merge two words.
 * Reference: Copied from glibc.
 */
#if defined(_SSM_LITTLE_ENDIAN)
    #define _SSM_MERGE_WORDS(w0, sh1, w1, sh2) (((w0) >> (sh1)) | ((w1) << (sh2)))
#else
    #define _SSM_MERGE_WORDS(w0, sh1, w1, sh2) (((w0) << (sh1)) | ((w1) >> (sh2)))
#endif

/*
 * Standard assertions are used in user applications.
 * Assertions are ignored in the kernel.
 */
#if defined(__KERNEL__)
    #define _SSM_ASSERT(ignore) ((void)0)
#else
    #include <assert.h>
    #define _SSM_ASSERT(condition) assert(condition)
#endif

/*
 * Min and max as inlined functions.
 */
static inline size_t _ssm_min_size_t(size_t a, size_t b)
{
    return a < b ? a : b;
}
static inline size_t _ssm_max_size_t(size_t a, size_t b)
{
    return a > b ? a : b;
}

/*
 * Handle a memory corruption error.
 * Invoked by the "canary" functions of the library when they detect a memory corruption.
 */
void _ssm_canary_corruption(const char* file, size_t line);

/*
 * Memory allocation function.
 * Get the number of bytes to request.
 * Return base address of allocated memory or NULL on allocation failure.
 */
void* _ssm_malloc(size_t size);

/*
 * Memory deallocation function.
 * Base address of memory to free. Must be an address which
 * was previously returned by _ssm_malloc(). Ignored if NULL.
 */
void _ssm_free(void* ptr);

/*
 * Dynamic memory allocation size policy.
 *
 * This function defines the allocation and reallocation size policy.
 * To reduce the number of memory allocation operations, it is better to
 * not always allocate the exact number of requested bytes.
 * For instance, if a buffer is shrunk, do we reallocate a smaller one or do we
 * keep the previous and larger one? When we need to allocate memory, could we
 * over-allocate in order to anticipate the future need of a larger one?
 *
 * oldSize is the previous size of a buffer, use zero to allocate a new buffer.
 * newSize is the requested new size.
 * Return the new size to allocate.
 */
size_t _ssm_alloc_size(size_t oldSize, size_t newSize);

/*
 * Allocate memory for a dynamic memory buffer.
 * Size is the number of user bytes to allocate in the buffer (excluding canaries and trailing null byte).
 * If keepContent is true, keep the previous content of the buffer in case of reallocation.
 */
ssm_status_t _SSM_CANARY_SYMBOL(_ssm_dbuffer_alloc) (ssm_dbuffer_t* buf, size_t size, int keepContent _SSM_CANARY_ARG_DECL);

/*
 * Check a dynamic memory buffer for memory corruption.
 * With canary checks, if a memory corruption is detected, the corruption
 * is reported through _ssm_canary_corruption(), which means that the
 * application may be aborted and we never return to the caller.
 * Return Non-zero (true) if the buffer is correct or NULL (empty buffer),
 * zero (false) if a memory corruption has been detected.
 */
int _SSM_CANARY_SYMBOL(_ssm_dbuffer_check) (const ssm_dbuffer_t* buf _SSM_CANARY_ARG_DECL);

/*
 * Check an input static memory buffer parameter for memory corruption.
 * With canary checks, if a memory corruption is detected, the corruption
 * is reported through _ssm_canary_corruption(), which means that the
 * application may be aborted and we never return to the caller.
 * Return Non-zero (true) if the buffer is correct or NULL (empty buffer),
 * zero (false) if a memory corruption has been detected.
 */
int _SSM_CANARY_SYMBOL(_ssm_sbuffer_check_in) (const ssm_sbuffer_t* buf _SSM_CANARY_ARG_DECL);

/*
 * Check an output static memory buffer parameter for memory corruption.
 * With canary checks, if a memory corruption is detected, the corruption
 * is reported through _ssm_canary_corruption(), which means that the
 * application may be aborted and we never return to the caller.
 * If the buffer is pre-initialized (unmodified), its canaries are updated.
 */
#if defined(SSM_USE_CANARY)
int _SSM_CANARY_SYMBOL(_ssm_sbuffer_check_out) (ssm_sbuffer_t* buf _SSM_CANARY_ARG_DECL);
#else
#define _ssm_sbuffer_check_out(buf) _ssm_sbuffer_check_in(buf)
#endif

#endif /* _SSM_INTERNAL_H */

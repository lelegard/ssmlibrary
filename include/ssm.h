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
 *  Public API of SSM, the Safe Strings and Memory buffers library for C.
 *
 *  Note to application developers: Do not read this header file !
 *  Reading it could be misleading and confusing.
 *  Read the documentation, it is designed for the user.
 *
 *  As a general rule, all identifiers in this file starting with an
 *  underscore are private and shall never be used in application code.
 *  On the other hand, identifiers starting with a letter are public and
 *  may be used in application code.
 *
 *  Implementation notes:
 *  - Strings are implemented with the same mechanism as the memory buffers.
 *    For compatibility with strings, memory buffers are always null-terminated.
 *    The trailing null byte is an extra storage and not part of the user data.
 *  - But string types are distinct from memory buffer types to allow the
 *    compiler to check invalid mix of strings and memory buffers.
 *
 *-----------------------------------------------------------------------------
 *
 *  Modification history:
 *    2014-01-03 Thierry Lelegard
 *               Original creation.
 *    2014-01-17 Thierry Lelegard
 *               Reimplement strings over memory buffers.
 *
 *-----------------------------------------------------------------------------
 */

#if !defined(_SSM_H)
#define _SSM_H 1

/*
 * Required external definitions are limited to
 * size_t, uint8_t, uint32_t, uintptr_t.
 */
#include <stddef.h>
#if defined(__KERNEL__)
#include <linux/types.h>
#else
#include <stdint.h>
#endif

/*
 * Turn text into a string literal.
 */
#define _SSM_STRINGIFY1(x) #x
#define _SSM_STRINGIFY(x) _SSM_STRINGIFY1(x)

/*
 * SSM library version.
 */
#define SSM_MAJOR_VERSION   1
#define SSM_MINOR_VERSION   0
#define SSM_VERSION         ((100 * SSM_MAJOR_VERSION) + SSM_MINOR_VERSION)
#define SSM_VERSION_STRING  _SSM_STRINGIFY(SSM_MAJOR_VERSION) "." _SSM_STRINGIFY(SSM_MINOR_VERSION)

/*
 * Some selected Microsoft Visual C++ warnings are disabled below.
 * These warnings are considered as excessive in most cases.
 *
 * warning C4100: 'xxxx' : unreferenced formal parameter
 *   Unused variables are almost always an error. Unused formal parameters in
 *   functions, however, may be legitimately ignored. This is the case when a
 *   function has a required profile but does not need to use a parameter in
 *   a given implementation.
 *
 * warning C4200: nonstandard extension used : zero-sized array in struct/union
 *   If you declare a zero-character-size static string, this warning is
 *   triggered although this is a legitimate usage.
 */
#if defined(_MSC_VER)
    #pragma warning (disable:4100)
    #pragma warning (disable:4200)
#endif

/*
 * SSM as a DLL on Windows.
 *
 * If _SSMDLL_IMPL is defined, we are compiling the SSM DLL.
 * If SSM_USE_DLL is defined, we are compiling an application using the SSM DLL.
 * If none is defined, we are compiling or using the SSM static library.
 * The symbol _SSMAPI shall be used in front of all external symbol declaration.
 * Symbols without that suffix can be used only inside the SSM DLL.
 */
#if defined(_SSMDLL_IMPL)
    #define _SSMAPI __declspec(dllexport)
#elif defined(SSM_USE_DLL)
    #define _SSMAPI __declspec(dllimport)
#else
    #define _SSMAPI
#endif

/*
 * Inline functions are part of the ISO/IEC 9899:1999 (C99) standard.
 * With Microsoft Visual C++, "inline" is not defined in C, only in C++.
 * The MSVC-specific extension "__inline" can be used in C.
 */
#if defined(_MSC_VER) && !defined(__cplusplus) && !defined(inline)
    #define inline __inline
#endif

/*
 * Some compilers need an empty aggregate definition, some forbid it, some don't care.
 */
#if defined(__GNUC__) && __GNUC__ >= 4
    #define _SSM_EMPTY_AGGREGATE ,{}
#else
    #define _SSM_EMPTY_AGGREGATE
#endif

#if defined(__cplusplus)
extern "C" {
#endif

/*
 * Status values, as returned by all SSM functions.
 * When defining new values, beware of success/non-fatal/fatal classification.
 */
typedef enum {
    SSM_OK           = 0x00,  /* The function executed successfully. */
    SSM_TRUNCATED    = 0x01,  /* The result is truncated but safe. */
    SSM_EQUAL        = 0x02,  /* Objects are equal after comparison. */
    SSM_LOWER        = 0x03,  /* Object 1 is lower than object 2 after comparison. */
    SSM_GREATER      = 0x04,  /* Object 1 is greater than object 2 after comparison. */
    SSM_NULLOUT      = 0x40,  /* A NULL pointer was provided as output parameter */
    SSM_SIZETOOLARGE = 0x41,  /* Some size is larger than SSM_SIZE_MAX. */
    SSM_INDEXRANGE   = 0x42,  /* An index parameter in out of range. */
    SSM_NOMEMORY     = 0xC0,  /* Memory allocation failure, result is unchanged. */
    SSM_CORRUPTED    = 0xC1,  /* Memory was previously corrupted, result is undefined but safe. */
    SSM_BUG          = 0xC2   /* Internal inconsistency, there is a bug in the SSM library. */
} ssm_status_t;

/*
 * Status checking macros.
 */
#define ssm_success(status) (((int)(status) & 0x40) == 0)
#define ssm_error(status)   (((int)(status) & 0x40) != 0)
#define ssm_fatal(status)   (((int)(status) & 0x80) != 0)

/*
 * Size and address limits.
 */
#define SSM_SIZE_MAX (0x7FFFFFFFU)
#define SSM_ADDRESS_MAX ((void*)(~(uintptr_t)0))

/*
 * Compute a safe address plus an offset.
 */
static inline const void* ssm_addr_size(const void* addr, size_t size)
{
    void* end = (char*)addr + size; /* may result in an overflow (end < addr) */
    return end < addr ? SSM_ADDRESS_MAX : end;
}

/*
 * General-purpose memory operations.
 */
_SSMAPI ssm_status_t ssm_copy(void* dest, size_t destSize, const void* src, size_t srcSize, size_t* copiedSize);
_SSMAPI ssm_status_t ssm_compare(const void* addr1, size_t size1, const void* addr2, size_t size2);
_SSMAPI ssm_status_t ssm_set(void* dest, size_t destSize, uint8_t value);
_SSMAPI size_t ssm_cstring_length(const char* str, size_t maxSize);

/*
 * Dynamic memory management
 */
typedef void* (*ssm_malloc_t)(size_t size);
typedef void (*ssm_free_t)(void* ptr);
_SSMAPI void ssm_set_memory_management(ssm_malloc_t newMalloc, ssm_free_t newFree);

/*
 * Memory corruption handler type.
 */
typedef void (*ssm_canary_corrupted_handler_t)(const char* file, size_t line);

/*
 * Establish a handler for memory corruption.
 */
_SSMAPI ssm_canary_corrupted_handler_t _ssm_set_canary_corrupted_handler(ssm_canary_corrupted_handler_t handler);
#if defined(SSM_USE_CANARY)
    #define ssm_set_canary_corrupted_handler(handler) _ssm_set_canary_corrupted_handler(handler)
#else
    #define ssm_set_canary_corrupted_handler(handler) ((ssm_canary_corrupted_handler_t)NULL)
#endif

/*
 * Support macros for canary alternatives.
 */
#if defined(SSM_USE_CANARY)
    #define _SSM_CANARY_SIZE            (sizeof(_ssm_canary_t))
    #define _SSM_CANARY_SYMBOL(symbol)  symbol ## _canary
    #define _SSM_CANARY_ARG_DECL        , const char* file, size_t line
    #define _SSM_CANARY_ARG_APP         , __FILE__, __LINE__
    #define _SSM_CANARY_ARG_USE         , file, line
#else
    #define _SSM_CANARY_SIZE            0
    #define _SSM_CANARY_SYMBOL(symbol)  symbol
    #define _SSM_CANARY_ARG_DECL
    #define _SSM_CANARY_ARG_APP
    #define _SSM_CANARY_ARG_USE
#endif

/*
 * Definition of a canary marker.
 */
typedef uint32_t _ssm_canary_t;

#define _SSM_CANARY_INIT   0xDEADBEEF  /* At start of structure for uninitialized static objects */
#define _SSM_CANARY_UPDATE 0x5489A5B7  /* At start of structure */
#define _SSM_CANARY_TRAIL  0xF0015BA7  /* At end of structure */


/*-----------------------------------------------------------------------------
 * STATIC MEMORY BUFFERS
 *-----------------------------------------------------------------------------
 */

typedef struct {
#if defined(SSM_USE_CANARY)
    _ssm_canary_t _canary;  /* _SSM_CANARY_INIT, then _SSM_CANARY_UPDATE */
#endif
    size_t _maxSize;        /* Actual size of _data[] (including canaries and trailing null) */
    size_t _curSize;        /* Currently used size in _data[] (not including caranies and trailing null) */
    char _data[1];          /* Variable size depending of overloaded buffer */
} ssm_sbuffer_t;

#if defined(SSM_USE_CANARY)
    #define _ssm_sbuffer_init(size) {_SSM_CANARY_INIT, (size) + _SSM_CANARY_SIZE + 1, 0, {0}}
#else
    #define _ssm_sbuffer_init(size) {(size) + 1, 0, {0}}
#endif

#define ssm_sbuffer_struct(size)                \
    struct {                                    \
        ssm_sbuffer_t buf;                      \
        char _data[(size) + _SSM_CANARY_SIZE];  \
    }
#define ssm_sbuffer_init(size) {_ssm_sbuffer_init(size) _SSM_EMPTY_AGGREGATE}
#define ssm_sbuffer_declare(variable,size) ssm_sbuffer_struct(size) variable = ssm_sbuffer_init(size)

#define ssm_sbuffer_import(dest,src,maxSize) _SSM_CANARY_SYMBOL(_ssm_sbuffer_import) ((dest), (src), (maxSize) _SSM_CANARY_ARG_APP)
_SSMAPI ssm_status_t _SSM_CANARY_SYMBOL(_ssm_sbuffer_import) (ssm_sbuffer_t* dest, const void* src, size_t maxSize _SSM_CANARY_ARG_DECL);

#define ssm_sbuffer_data(buf) _SSM_CANARY_SYMBOL(_ssm_sbuffer_data) ((buf) _SSM_CANARY_ARG_APP)
#if defined(SSM_USE_CANARY)
_SSMAPI const void* _SSM_CANARY_SYMBOL(_ssm_sbuffer_data) (const ssm_sbuffer_t* buf _SSM_CANARY_ARG_DECL);
#else
static inline const void* _ssm_sbuffer_data(const ssm_sbuffer_t* buf)
{
    return buf == NULL ? "" : buf->_data;
}
#endif

#define ssm_sbuffer_length(buf) _SSM_CANARY_SYMBOL(_ssm_sbuffer_length) ((buf) _SSM_CANARY_ARG_APP)
#if defined(SSM_USE_CANARY)
_SSMAPI size_t _SSM_CANARY_SYMBOL(_ssm_sbuffer_length) (const ssm_sbuffer_t* buf _SSM_CANARY_ARG_DECL);
#else
static inline size_t _ssm_sbuffer_length(const ssm_sbuffer_t* buf)
{
    return buf == NULL ? 0 : buf->_curSize;
}
#endif

#define ssm_sbuffer_max_length(buf) _SSM_CANARY_SYMBOL(_ssm_sbuffer_max_length) ((buf) _SSM_CANARY_ARG_APP)
#if defined(SSM_USE_CANARY)
_SSMAPI size_t _SSM_CANARY_SYMBOL(_ssm_sbuffer_max_length) (const ssm_sbuffer_t* buf _SSM_CANARY_ARG_DECL);
#else
static inline size_t _ssm_sbuffer_max_length(const ssm_sbuffer_t* buf)
{
    return buf == NULL || buf->_maxSize == 0 ? 0 : buf->_maxSize - 1;
}
#endif

#define ssm_sbuffer_resize(buf,length) _SSM_CANARY_SYMBOL(_ssm_sbuffer_resize) ((buf), (length) _SSM_CANARY_ARG_APP)
_SSMAPI ssm_status_t _SSM_CANARY_SYMBOL(_ssm_sbuffer_resize) (ssm_sbuffer_t* buf, size_t length _SSM_CANARY_ARG_DECL);

#define ssm_sbuffer_set_range(buf,start,length,value) \
    _SSM_CANARY_SYMBOL(_ssm_sbuffer_set_range) ((buf), (start), (length), (value) _SSM_CANARY_ARG_APP)
_SSMAPI ssm_status_t _SSM_CANARY_SYMBOL(_ssm_sbuffer_set_range) \
    (ssm_sbuffer_t* buf, size_t start, size_t length, uint8_t value _SSM_CANARY_ARG_DECL);

#define ssm_sbuffer_set(buf,value) _ssm_sbuffer_set((buf), (value) _SSM_CANARY_ARG_APP)
static inline ssm_status_t _ssm_sbuffer_set(ssm_sbuffer_t* buf, uint8_t value _SSM_CANARY_ARG_DECL)
{
    return buf == NULL ? SSM_NULLOUT : _SSM_CANARY_SYMBOL(_ssm_sbuffer_set_range) (buf, 0, buf->_curSize, value _SSM_CANARY_ARG_USE);
}

#define ssm_sbuffer_copy(dest,src) _SSM_CANARY_SYMBOL(_ssm_sbuffer_copy) ((dest), (src) _SSM_CANARY_ARG_APP)
_SSMAPI ssm_status_t _SSM_CANARY_SYMBOL(_ssm_sbuffer_copy) (ssm_sbuffer_t* dest, const ssm_sbuffer_t* src _SSM_CANARY_ARG_DECL);

#define ssm_sbuffer_concat(dest,src) _SSM_CANARY_SYMBOL(_ssm_sbuffer_concat) ((dest), (src) _SSM_CANARY_ARG_APP)
_SSMAPI ssm_status_t _SSM_CANARY_SYMBOL(_ssm_sbuffer_concat) (ssm_sbuffer_t* dest, const ssm_sbuffer_t* src _SSM_CANARY_ARG_DECL);

#define ssm_sbuffer_compare(buf1,buf2) _SSM_CANARY_SYMBOL(_ssm_sbuffer_compare) ((buf1), (buf2) _SSM_CANARY_ARG_APP)
_SSMAPI ssm_status_t _SSM_CANARY_SYMBOL(_ssm_sbuffer_compare) (const ssm_sbuffer_t* buf1, const ssm_sbuffer_t* buf2 _SSM_CANARY_ARG_DECL);

#define ssm_sbuffer_insert(buf,start,length) _SSM_CANARY_SYMBOL(_ssm_sbuffer_insert) ((buf), (start),(length) _SSM_CANARY_ARG_APP)
_SSMAPI ssm_status_t _SSM_CANARY_SYMBOL(_ssm_sbuffer_insert) (ssm_sbuffer_t* buf, size_t start, size_t length _SSM_CANARY_ARG_DECL);


/*-----------------------------------------------------------------------------
 * DYNAMIC MEMORY BUFFERS
 *-----------------------------------------------------------------------------
 */

typedef struct {
#if defined(SSM_USE_CANARY)
    _ssm_canary_t _canary1;  /* _SSM_CANARY_INIT */
#endif
    size_t _maxSize;         /* Actual size of allocated _data (including canaries and trailing null) */
    size_t _curSize;         /* Currently used size in *_data (not including canaries and trailing null) */
    char* _data;             /* Dynamically allocated, also surrounded with canaries */
#if defined(SSM_USE_CANARY)
    _ssm_canary_t _canary2;  /* _SSM_CANARY_TRAIL */
#endif
} ssm_dbuffer_t;

#if defined(SSM_USE_CANARY)
    #define ssm_dbuffer_init {_SSM_CANARY_INIT, 0, 0, NULL, _SSM_CANARY_TRAIL}
#else
    #define ssm_dbuffer_init {0, 0, NULL}
#endif

#define ssm_dbuffer_declare(variable) ssm_dbuffer_t variable = ssm_dbuffer_init

#define ssm_dbuffer_free(buf) _SSM_CANARY_SYMBOL(_ssm_dbuffer_free) ((buf) _SSM_CANARY_ARG_APP)
_SSMAPI ssm_status_t _SSM_CANARY_SYMBOL(_ssm_dbuffer_free) (ssm_dbuffer_t* buf _SSM_CANARY_ARG_DECL);

#define ssm_dbuffer_import(dest,src,maxSize) _SSM_CANARY_SYMBOL(_ssm_dbuffer_import) ((dest), (src), (maxSize) _SSM_CANARY_ARG_APP)
_SSMAPI ssm_status_t _SSM_CANARY_SYMBOL(_ssm_dbuffer_import) (ssm_dbuffer_t* dest, const void* src, size_t maxSize _SSM_CANARY_ARG_DECL);

#define ssm_dbuffer_data(buf) _SSM_CANARY_SYMBOL(_ssm_dbuffer_data) ((buf) _SSM_CANARY_ARG_APP)
#if defined(SSM_USE_CANARY)
_SSMAPI const void* _SSM_CANARY_SYMBOL(_ssm_dbuffer_data) (const ssm_dbuffer_t* buf _SSM_CANARY_ARG_DECL);
#else
static inline const void* _ssm_dbuffer_data(const ssm_dbuffer_t* buf)
{
    return buf == NULL || buf->_data == NULL ? "" : buf->_data;
}
#endif

#define ssm_dbuffer_length(buf) _SSM_CANARY_SYMBOL(_ssm_dbuffer_length) ((buf) _SSM_CANARY_ARG_APP)
#if defined(SSM_USE_CANARY)
_SSMAPI size_t _SSM_CANARY_SYMBOL(_ssm_dbuffer_length) (const ssm_dbuffer_t* buf _SSM_CANARY_ARG_DECL);
#else
static inline size_t _ssm_dbuffer_length(const ssm_dbuffer_t* buf)
{
    return buf == NULL ? 0 : buf->_curSize;
}
#endif

#define ssm_dbuffer_resize(buf,length) _SSM_CANARY_SYMBOL(_ssm_dbuffer_resize) ((buf), (length) _SSM_CANARY_ARG_APP)
_SSMAPI ssm_status_t _SSM_CANARY_SYMBOL(_ssm_dbuffer_resize) (ssm_dbuffer_t* buf, size_t length _SSM_CANARY_ARG_DECL);

#define ssm_dbuffer_set_range(buf,start,length,value) \
    _SSM_CANARY_SYMBOL(_ssm_dbuffer_set_range) ((buf), (start), (length), (value) _SSM_CANARY_ARG_APP)
_SSMAPI ssm_status_t _SSM_CANARY_SYMBOL(_ssm_dbuffer_set_range) \
    (ssm_dbuffer_t* buf, size_t start, size_t length, uint8_t value _SSM_CANARY_ARG_DECL);

#define ssm_dbuffer_set(buf,value) _ssm_dbuffer_set((buf), (value) _SSM_CANARY_ARG_APP)
static inline ssm_status_t _ssm_dbuffer_set(ssm_dbuffer_t* buf, uint8_t value _SSM_CANARY_ARG_DECL)
{
    return buf == NULL ? SSM_NULLOUT : _SSM_CANARY_SYMBOL(_ssm_dbuffer_set_range) (buf, 0, buf->_curSize, value _SSM_CANARY_ARG_USE);
}

#define ssm_dbuffer_copy(dest,src) _SSM_CANARY_SYMBOL(_ssm_dbuffer_copy) ((dest), (src) _SSM_CANARY_ARG_APP)
_SSMAPI ssm_status_t _SSM_CANARY_SYMBOL(_ssm_dbuffer_copy) (ssm_dbuffer_t* dest, const ssm_dbuffer_t* src _SSM_CANARY_ARG_DECL);

#define ssm_dbuffer_concat(dest,src) _SSM_CANARY_SYMBOL(_ssm_dbuffer_concat) ((dest), (src) _SSM_CANARY_ARG_APP)
_SSMAPI ssm_status_t _SSM_CANARY_SYMBOL(_ssm_dbuffer_concat) (ssm_dbuffer_t* dest, const ssm_dbuffer_t* src _SSM_CANARY_ARG_DECL);

#define ssm_dbuffer_compare(buf1,buf2) _SSM_CANARY_SYMBOL(_ssm_dbuffer_compare) ((buf1), (buf2) _SSM_CANARY_ARG_APP)
_SSMAPI ssm_status_t _SSM_CANARY_SYMBOL(_ssm_dbuffer_compare) (const ssm_dbuffer_t* buf1, const ssm_dbuffer_t* buf2 _SSM_CANARY_ARG_DECL);

#define ssm_dbuffer_insert(buf,start,length) _SSM_CANARY_SYMBOL(_ssm_dbuffer_insert) ((buf), (start),(length) _SSM_CANARY_ARG_APP)
_SSMAPI ssm_status_t _SSM_CANARY_SYMBOL(_ssm_dbuffer_insert) (ssm_dbuffer_t* buf, size_t start, size_t length _SSM_CANARY_ARG_DECL);


/*-----------------------------------------------------------------------------
 * STATIC STRINGS
 *-----------------------------------------------------------------------------
 */

typedef struct {
    ssm_sbuffer_t _buf;
} ssm_sstring_t;

#define ssm_sstring_struct(size)               \
    struct {                                   \
        ssm_sstring_t str;                     \
        char _data[(size) + _SSM_CANARY_SIZE]; \
    }
#define ssm_sstring_init(size) {{_ssm_sbuffer_init(size)} _SSM_EMPTY_AGGREGATE}
#define ssm_sstring_declare(variable,size) ssm_sstring_struct(size) variable = ssm_sstring_init(size)

#define ssm_sstring_import_size(dest,src,maxSize) \
    _SSM_CANARY_SYMBOL(_ssm_sstring_import_size) ((dest), (src), (maxSize) _SSM_CANARY_ARG_APP)
_SSMAPI ssm_status_t _SSM_CANARY_SYMBOL(_ssm_sstring_import_size)
    (ssm_sstring_t* dest, const char* src, size_t maxSize _SSM_CANARY_ARG_DECL);

static inline ssm_status_t ssm_sstring_import(ssm_sstring_t* dest, const char* src)
{
    return dest == NULL ? SSM_NULLOUT : ssm_sstring_import_size(dest, src, dest->_buf._maxSize);
}

#define ssm_sstring_chars(str) ((const char*)(ssm_sbuffer_data(&(str)->_buf)))
#define ssm_sstring_length(str) ssm_sbuffer_length(&(str)->_buf)
#define ssm_sstring_max_length(str) ssm_sbuffer_max_length(&(str)->_buf)
#define ssm_sstring_set_range(str,start,length,value) ssm_sbuffer_set_range(&(str)->_buf,(start),(length),(uint8_t)(value)) 
#define ssm_sstring_set(str,value) ssm_sbuffer_set(&(str)->_buf,(uint8_t)(value)) 
#define ssm_sstring_copy(dest,src) ssm_sbuffer_copy(&(dest)->_buf, &(src)->_buf)
#define ssm_sstring_concat(dest,src) ssm_sbuffer_concat(&(dest)->_buf, &(src)->_buf)
#define ssm_sstring_compare(str1,str2) ssm_sbuffer_compare(&(str1)->_buf, &(str2)->_buf)


/*-----------------------------------------------------------------------------
 * DYNAMIC STRINGS
 *-----------------------------------------------------------------------------
 */

typedef struct {
    ssm_dbuffer_t _buf;
} ssm_dstring_t;

#define ssm_dstring_init {ssm_dbuffer_init}
#define ssm_dstring_declare(variable) ssm_dstring_t variable = ssm_dstring_init

#define ssm_dstring_import_size(dest,src,maxSize) \
    _SSM_CANARY_SYMBOL(_ssm_dstring_import_size) ((dest), (src), (maxSize) _SSM_CANARY_ARG_APP)
_SSMAPI ssm_status_t _SSM_CANARY_SYMBOL(_ssm_dstring_import_size) \
    (ssm_dstring_t* dest, const char* src, size_t maxSize _SSM_CANARY_ARG_DECL);

#define ssm_dstring_import(dest,src) ssm_dstring_import_size((dest), (src), SSM_SIZE_MAX)
#define ssm_dstring_free(str) ssm_dbuffer_free(&(str)->_buf)
#define ssm_dstring_chars(str) ((const char*)(ssm_dbuffer_data(&(str)->_buf)))
#define ssm_dstring_length(str) ssm_dbuffer_length(&(str)->_buf)
#define ssm_dstring_set_range(str,start,length,value) ssm_dbuffer_set_range(&(str)->_buf,(start),(length),(uint8_t)(value)) 
#define ssm_dstring_set(str,value) ssm_dbuffer_set(&(str)->_buf,(uint8_t)(value)) 
#define ssm_dstring_copy(dest,src) ssm_dbuffer_copy(&(dest)->_buf, &(src)->_buf)
#define ssm_dstring_concat(dest,src) ssm_dbuffer_concat(&(dest)->_buf, &(src)->_buf)
#define ssm_dstring_compare(str1,str2) ssm_dbuffer_compare(&(str1)->_buf, &(str2)->_buf)

#if defined(__cplusplus)
}
#endif

#endif /* _SSM_H */

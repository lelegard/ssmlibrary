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
 * Handling of dynamic memory allocation.
 *
 *-----------------------------------------------------------------------------
 *
 *  Modification history:
 *    2014-01-06 Thierry Lelegard
 *               Original creation.
 *
 *-----------------------------------------------------------------------------
 */

#include "ssm_internal.h"

#if !defined(__KERNEL__)
#include <stdlib.h>
#endif

/*
 * The default memory management functions could be different in constrained
 * environments. Add some conditionals around these definitions if necessary.
 * Define them as NULL if no default functions are suitable.
 */
#if defined(__KERNEL__)
#define SSM_DEFAULT_MALLOC NULL
#define SSM_DEFAULT_FREE   NULL
#else
#define SSM_DEFAULT_MALLOC malloc
#define SSM_DEFAULT_FREE   free
#endif

/*
 * Current values of the memory management functions.
 * NULL means "use the default".
 * And if the default is NULL, this means "all allocations safely fail".
 */
static volatile ssm_malloc_t malloc_function = NULL;
static volatile ssm_free_t free_function = NULL;


/*-----------------------------------------------------------------------------
 * Specify alternate functions for memory management.
 *-----------------------------------------------------------------------------
 */
void ssm_set_memory_management(ssm_malloc_t newMalloc, ssm_free_t newFree)
{
    malloc_function = newMalloc;
    free_function = newFree;
}


/*-----------------------------------------------------------------------------
 * Memory allocation function.
 *-----------------------------------------------------------------------------
 */
void* _ssm_malloc(size_t size)
{
    ssm_malloc_t func = malloc_function != NULL ? malloc_function : SSM_DEFAULT_MALLOC;
    return func == NULL ? NULL : func(size);
}


/*-----------------------------------------------------------------------------
 * Memory deallocation function.
 *-----------------------------------------------------------------------------
 */
void _ssm_free(void* ptr)
{
    if (ptr != NULL) {
        ssm_free_t func = free_function != NULL ? free_function : SSM_DEFAULT_FREE;
        if (func != NULL) {
            func(ptr);
        }
    }
}

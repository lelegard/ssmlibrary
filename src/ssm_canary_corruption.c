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
 * Handling of detected memory corruptions using canary checks.
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

#if defined(__KERNEL__)
#include <linux/kernel.h>
#else
#include <stdio.h>
#include <stdlib.h>
#endif

/*
 * When doing code coverage, abort() prevents the creation of the coverage
 * data files. Use exit() instead.
 */
#if defined(GCOV)
#define _SSM_ABORT() exit(EXIT_FAILURE)
#else
#define _SSM_ABORT() abort()
#endif


/*-----------------------------------------------------------------------------
 * Current value of the memory corruption handler.
 *-----------------------------------------------------------------------------
 */
static volatile ssm_canary_corrupted_handler_t corrupted_handler = NULL;


/*-----------------------------------------------------------------------------
 * Establish a handler for memory corruption.
 *-----------------------------------------------------------------------------
 */
ssm_canary_corrupted_handler_t _ssm_set_canary_corrupted_handler(ssm_canary_corrupted_handler_t handler)
{
    ssm_canary_corrupted_handler_t previous = corrupted_handler;
    corrupted_handler = handler;
    return previous;
}


/*-----------------------------------------------------------------------------
 * Default memory corruption handler.
 *-----------------------------------------------------------------------------
 */
static void default_corrupted_handler(const char* file, size_t line)
{
#if defined(__KERNEL__)
    printk(KERN_ALERT "SSM fatal memory corruption in %s:%d\n", file, (int)line);
#else
    fprintf(stderr, "SSM fatal memory corruption in %s:%d\n", file, (int)line);
    _SSM_ABORT();
#endif
}


/*-----------------------------------------------------------------------------
 * Handle a memory corruption error.
 *-----------------------------------------------------------------------------
 */
void _ssm_canary_corruption(const char* file, size_t line)
{
    ssm_canary_corrupted_handler_t handler = corrupted_handler;
    if (handler == NULL) {
        handler = default_corrupted_handler;
    }
    handler(file == NULL ? "" : file, line);
}

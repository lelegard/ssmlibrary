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
 *  CUnit test suite for ssm_set_memory_management() with canary runtime checks.
 *
 *-----------------------------------------------------------------------------
 *
 *  Modification history:
 *    2014-01-20 Thierry Lelegard
 *               Original creation.
 *
 *-----------------------------------------------------------------------------
 */

#include "ssm_memory_management_canary_test.h"
#define SSM_USE_CANARY 1
#include "ssm.h"

#include "ssm_memory_management_test.c"

/*
 * Test suite initialization function.
 * Returns zero on success, non-zero on error.
 */
static int init_suite_canary(void)
{
    return init_suite();
}

/*
 * Test suite cleanup function.
 * Returns zero on success, non-zero on error.
 */
static int cleanup_suite_canary(void)
{
    return cleanup_suite();
}

/*
 * Initialize the test suite in this module.
 * We assume here that the main program has set CUnit error mode
 * as "abort" so there is no need to test individual errors.
 */
void ssm_memory_management_canary_test_init(void)
{
    const CU_pSuite suite = CU_add_suite ("memory_management_canary", init_suite_canary, cleanup_suite_canary);

    /* Add same tests as in non-canary functions */
    ssm_memory_management_add_tests(suite);

    /* Add canary-specific tests */
    /* None for the moment... */
}

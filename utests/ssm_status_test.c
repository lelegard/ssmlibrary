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
 *  CUnit test suite for the type ssm_status_t.
 *
 *-----------------------------------------------------------------------------
 *
 *  Modification history:
 *    2014-01-10 Thierry Lelegard
 *               Original creation.
 *
 *-----------------------------------------------------------------------------
 */

#include "ssm_status_test.h"
#include "ssm.h"
#include "utests.h"

/*
 * Test cases
 */
static void test_success(void)
{
    CU_ASSERT ((int)SSM_OK == 0);

    CU_ASSERT (ssm_success(SSM_OK));
    CU_ASSERT (ssm_success(SSM_TRUNCATED));
    CU_ASSERT (!ssm_success(SSM_NULLOUT));
    CU_ASSERT (!ssm_success(SSM_SIZETOOLARGE));
    CU_ASSERT (!ssm_success(SSM_INDEXRANGE));
    CU_ASSERT (!ssm_success(SSM_NOMEMORY));
    CU_ASSERT (!ssm_success(SSM_CORRUPTED));
    CU_ASSERT (!ssm_success(SSM_BUG));
}

static void test_error(void)
{
    CU_ASSERT (!ssm_error(SSM_OK));
    CU_ASSERT (!ssm_error(SSM_TRUNCATED));
    CU_ASSERT (ssm_error(SSM_NULLOUT));
    CU_ASSERT (ssm_error(SSM_SIZETOOLARGE));
    CU_ASSERT (ssm_error(SSM_INDEXRANGE));
    CU_ASSERT (ssm_error(SSM_NOMEMORY));
    CU_ASSERT (ssm_error(SSM_CORRUPTED));
    CU_ASSERT (ssm_error(SSM_BUG));
}

static void test_fatal(void)
{
    CU_ASSERT (!ssm_fatal(SSM_OK));
    CU_ASSERT (!ssm_fatal(SSM_TRUNCATED));
    CU_ASSERT (!ssm_fatal(SSM_NULLOUT));
    CU_ASSERT (!ssm_fatal(SSM_SIZETOOLARGE));
    CU_ASSERT (!ssm_fatal(SSM_INDEXRANGE));
    CU_ASSERT (ssm_fatal(SSM_NOMEMORY));
    CU_ASSERT (ssm_fatal(SSM_CORRUPTED));
    CU_ASSERT (ssm_fatal(SSM_BUG));
}

/*
 * Test suite initialization function.
 * Returns zero on success, non-zero on error.
 */
static int init_suite (void)
{
    return 0;
}

/*
 * Test suite cleanup function.
 * Returns zero on success, non-zero on error.
 */
static int cleanup_suite (void)
{
    return 0;
}

/*
 * Initialize the test suite in this module.
 * We assume here that the main program has set CUnit error mode
 * as "abort" so there is no need to test individual errors.
 */
void ssm_status_test_init(void)
{
    const CU_pSuite suite = CU_add_suite ("status", init_suite, cleanup_suite);
    CU_ADD_TEST (suite, test_success);
    CU_ADD_TEST (suite, test_error);
    CU_ADD_TEST (suite, test_fatal);
}

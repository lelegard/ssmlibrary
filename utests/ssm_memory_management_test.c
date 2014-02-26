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
 *  CUnit test suite for ssm_set_memory_management().
 *
 *-----------------------------------------------------------------------------
 *
 *  Modification history:
 *    2014-01-20 Thierry Lelegard
 *               Original creation.
 *
 *-----------------------------------------------------------------------------
 */

#include "ssm_memory_management_test.h"
#include "ssm.h"
#include "utests.h"
#include <stdlib.h>

/*
 * User-defined memory allocation routines which track their invocation.
 */
static int test_malloc_called = 0;
static int test_free_called = 0;

static void* test_malloc(size_t size)
{
    test_malloc_called = 1;
    return malloc(size);
}
static void test_free(void* ptr)
{
    test_free_called = 1;
    free(ptr);
}

/*
 * User-defined memory allocation routines which fail allocations.
 */
static int failed_malloc_called = 0;

static void* failed_malloc(size_t size)
{
    failed_malloc_called = 1;
    return NULL;
}
static void failed_free(void* ptr)
{
    CU_ASSERT (ptr == NULL); /* failed_free() should never be invoked */
}

/*
 * Test cases
 */
static void test_alloc_free (void)
{
    ssm_dbuffer_declare(b1);
    ssm_dbuffer_declare(b2);
    ssm_dbuffer_declare(b3);

    test_malloc_called = 0;
    test_free_called = 0;

    CU_ASSERT (ssm_dbuffer_import(&b1, "abcd", 4) == SSM_OK);
    CU_ASSERT (ssm_dbuffer_length(&b1) == 4);
    CU_ASSERT (memcmp(ssm_dbuffer_data(&b1), "abcd", 4) == 0);
    CU_ASSERT (ssm_dbuffer_free(&b1) == SSM_OK);

    CU_ASSERT (!test_malloc_called);
    CU_ASSERT (!test_free_called);

    ssm_set_memory_management(test_malloc, test_free);

    CU_ASSERT (!test_malloc_called);
    CU_ASSERT (!test_free_called);

    CU_ASSERT (ssm_dbuffer_import(&b2, "abcd", 4) == SSM_OK);
    CU_ASSERT (ssm_dbuffer_length(&b2) == 4);
    CU_ASSERT (memcmp(ssm_dbuffer_data(&b2), "abcd", 4) == 0);

    CU_ASSERT (test_malloc_called);
    CU_ASSERT (!test_free_called);

    test_malloc_called = 0;
    test_free_called = 0;

    CU_ASSERT (ssm_dbuffer_free(&b2) == SSM_OK);

    CU_ASSERT (!test_malloc_called);
    CU_ASSERT (test_free_called);

    ssm_set_memory_management(NULL, NULL);

    test_malloc_called = 0;
    test_free_called = 0;

    CU_ASSERT (ssm_dbuffer_import(&b3, "abcd", 4) == SSM_OK);
    CU_ASSERT (ssm_dbuffer_length(&b3) == 4);
    CU_ASSERT (memcmp(ssm_dbuffer_data(&b3), "abcd", 4) == 0);
    CU_ASSERT (ssm_dbuffer_free(&b3) == SSM_OK);

    CU_ASSERT (!test_malloc_called);
    CU_ASSERT (!test_free_called);
}

static void test_alloc_failed (void)
{
    ssm_dbuffer_declare(b1);
    ssm_dbuffer_declare(b2);

    CU_ASSERT (ssm_dbuffer_import(&b1, "abcd", 4) == SSM_OK);
    CU_ASSERT (ssm_dbuffer_length(&b1) == 4);
    CU_ASSERT (memcmp(ssm_dbuffer_data(&b1), "abcd", 4) == 0);

    ssm_set_memory_management(failed_malloc, failed_free);

    failed_malloc_called = 0;
    CU_ASSERT (ssm_dbuffer_import(&b2, "abcd", 4) == SSM_NOMEMORY);
    CU_ASSERT (failed_malloc_called);
    CU_ASSERT (ssm_dbuffer_free(&b2) == SSM_OK);

    failed_malloc_called = 0;
    CU_ASSERT (ssm_dbuffer_copy(&b2, &b1) == SSM_NOMEMORY);
    CU_ASSERT (failed_malloc_called);
    CU_ASSERT (ssm_dbuffer_free(&b2) == SSM_OK);

    failed_malloc_called = 0;
    CU_ASSERT (ssm_dbuffer_concat(&b2, &b1) == SSM_NOMEMORY);
    CU_ASSERT (failed_malloc_called);
    CU_ASSERT (ssm_dbuffer_free(&b2) == SSM_OK);

    ssm_set_memory_management(NULL, NULL);

    failed_malloc_called = 0;

    CU_ASSERT (ssm_dbuffer_import(&b2, "efgh", 4) == SSM_OK);
    CU_ASSERT (ssm_dbuffer_length(&b2) == 4);
    CU_ASSERT (memcmp(ssm_dbuffer_data(&b2), "efgh", 4) == 0);
    CU_ASSERT (ssm_dbuffer_free(&b2) == SSM_OK);

    CU_ASSERT (!failed_malloc_called);

    CU_ASSERT (ssm_dbuffer_free(&b1) == SSM_OK);
}

/*
 * Add the tests in this module in a specified test suite.
 */
static void ssm_memory_management_add_tests(CU_pSuite suite)
{
    CU_ADD_TEST (suite, test_alloc_free);
    CU_ADD_TEST (suite, test_alloc_failed);
}

/*
 * Test suite initialization function.
 * Returns zero on success, non-zero on error.
 */
static int init_suite (void)
{
    ssm_set_memory_management(NULL, NULL);
    return 0;
}

/*
 * Test suite cleanup function.
 * Returns zero on success, non-zero on error.
 */
static int cleanup_suite (void)
{
    ssm_set_memory_management(NULL, NULL);
    return 0;
}

/*
 * Initialize the test suite in this module.
 * We assume here that the main program has set CUnit error mode
 * as "abort" so there is no need to test individual errors.
 */
#if !defined(SSM_USE_CANARY)
void ssm_memory_management_test_init(void)
{
    const CU_pSuite suite = CU_add_suite ("memory_management", init_suite, cleanup_suite);
    ssm_memory_management_add_tests(suite);
}
#endif /* SSM_USE_CANARY */

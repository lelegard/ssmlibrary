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
 *  CUnit test suite for ssm_sbuffer_t with canary runtime checks.
 *
 *-----------------------------------------------------------------------------
 *
 *  Modification history:
 *    2014-01-10 Thierry Lelegard
 *               Original creation.
 *
 *-----------------------------------------------------------------------------
 */

#include "ssm_sbuffer_canary_test.h"
#define SSM_USE_CANARY 1
#include "ssm.h"
#include "utests.h"

/*
 * Non-canary-specific test cases (same tests as standard functions).
 */
#include "ssm_sbuffer_test.c"

/*
 * A memory corruption handled which raises a flag.
 */
static int corrupted = 0;

static void corrupted_handler (const char* file, size_t line)
{
    corrupted = 1;
    fprintf (utests_out(), "ssm_sbuffer_canary_test: memory corruption detected at %s:%d\n", file, (int)line);
}

/*
 * Canary-specific test cases
 */
static void test_canary_in (void)
{
    ssm_sbuffer_declare (b1, 10);
    ssm_sbuffer_declare (b2, 10);

    CU_ASSERT (ssm_sbuffer_import(&b1.buf, "abcd", 4) == SSM_OK);
    CU_ASSERT (ssm_sbuffer_length(&b1.buf) == 4);
    CU_ASSERT (memcmp(ssm_sbuffer_data(&b1.buf), "abcd", 4) == 0);

    CU_ASSERT (ssm_sbuffer_import(&b2.buf, "efghij", 6) == SSM_OK);
    CU_ASSERT (ssm_sbuffer_length(&b2.buf) == 6);
    CU_ASSERT (memcmp(ssm_sbuffer_data(&b2.buf), "efghij", 6) == 0);

    memset(&b1, 0, 4);
    corrupted = 0;
    CU_ASSERT (ssm_sbuffer_copy(&b2.buf, &b1.buf) == SSM_CORRUPTED);
    CU_ASSERT (corrupted);

    CU_ASSERT (ssm_sbuffer_length(&b2.buf) == 6);
    CU_ASSERT (memcmp(ssm_sbuffer_data(&b2.buf), "efghij", 6) == 0);
}

static void test_canary_out (void)
{
    ssm_sbuffer_declare (b, 10);

    CU_ASSERT (ssm_sbuffer_import(&b.buf, "abcd", 4) == SSM_OK);
    CU_ASSERT (ssm_sbuffer_length(&b.buf) == 4);
    CU_ASSERT (memcmp(ssm_sbuffer_data(&b.buf), "abcd", 4) == 0);

    memset(&b, 0, 4);
    corrupted = 0;
    CU_ASSERT (ssm_sbuffer_import(&b.buf, "abcd", 4) == SSM_CORRUPTED);
    CU_ASSERT (corrupted);
}

/*
 * This test case is not run in the general case since it tests the default
 * canary memory corruption handler which aborts the application.
 * Define the parameter TEST_DEFAULT_CANARY_CORRUPTED_HANDLER
 * to run it. But this will interrupt the test suite.
 */
static void test_default_canary_corrupted_handler (void)
{
    ssm_sbuffer_declare (b, 10);

    /* Restore default handler */
    CU_ASSERT (ssm_set_canary_corrupted_handler(NULL) == corrupted_handler);

    memset(&b, 0, 4); /* trash the structure */
    ssm_sbuffer_import(&b.buf, "abcd", 4); /* should not return from here */
    CU_ASSERT (0);
}

/*
 * Test suite initialization function.
 * Returns zero on success, non-zero on error.
 */
static int init_suite_canary(void)
{
    corrupted = 0;
    return init_suite() || ssm_set_canary_corrupted_handler(corrupted_handler) != NULL;
}

/*
 * Test suite cleanup function.
 * Returns zero on success, non-zero on error.
 */
static int cleanup_suite_canary(void)
{
    corrupted = 0;
    return ssm_set_canary_corrupted_handler(NULL) != corrupted_handler || cleanup_suite();
}

/*
 * Initialize the test suite in this module.
 * We assume here that the main program has set CUnit error mode
 * as "abort" so there is no need to test individual errors.
 */
void ssm_sbuffer_canary_test_init(void)
{
    const CU_pSuite suite = CU_add_suite ("sbuffer_canary", init_suite_canary, cleanup_suite_canary);

    /* Add same tests as in non-canary functions */
    ssm_sbuffer_test_add_tests(suite);

    /* Add canary-specific tests */
    CU_ADD_TEST (suite, test_canary_in);
    CU_ADD_TEST (suite, test_canary_out);

    /* MSVC considers getenv() as unsecure, don't care in this test suite. */
    #if defined(_MSC_VER)
    #pragma warning (disable:4996)
    #endif

    /* This test case is not run in the general case since it aborts the application. */
    if (utest_param_value("TEST_DEFAULT_CANARY_CORRUPTED_HANDLER") != NULL) {
        CU_ADD_TEST (suite, test_default_canary_corrupted_handler);
    }
}

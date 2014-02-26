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
 *  CUnit test suite for ssm_dbuffer_t with canary runtime checks.
 *
 *  Do not forget to run this test suite under valgrind to look for memory leak.
 *
 *-----------------------------------------------------------------------------
 *
 *  Modification history:
 *    2014-01-10 Thierry Lelegard
 *               Original creation.
 *
 *-----------------------------------------------------------------------------
 */

#include "ssm_dbuffer_canary_test.h"
#define SSM_USE_CANARY 1
#include "ssm.h"
#include "utests.h"

/*
 * Non-canary-specific test cases (same tests as standard functions).
 */
#include "ssm_dbuffer_test.c"

/*
 * A memory corruption handled which raises a flag.
 */
static int corrupted = 0;

static void corrupted_handler (const char* file, size_t line)
{
    corrupted = 1;
    fprintf (utests_out(), "ssm_dbuffer_canary_test: memory corruption detected at %s:%d\n", file, (int)line);
}

/*
 * Canary-specific test cases
 *
 * Now comes the dirty stuff... Since we test memory corruption, we have to know
 * where to corrupt. Thus, we use our knowledge of the implementation, which should
 * not be the case for unitary tests. Moreover, we do not want the corruption to
 * remain permanent to avoid jeopardizing the unitary tests process. Thus, after
 * testing the corruption detection, we gently restore the corrupted memory and
 * we also free the dynamic string to avoid valgrind issuing non-relevant errors.
 */
static void test_canary_corrupt_base (void)
{
    ssm_dbuffer_declare(b);
    int* corrupted_address;
    int saved;

    CU_ASSERT (ssm_dbuffer_import(&b, "abcd", 4) == SSM_OK);
    CU_ASSERT (ssm_dbuffer_length(&b) == 4);
    CU_ASSERT (memcmp(ssm_dbuffer_data(&b), "abcd", 4) == 0);

    /* Corrupt the base data structure */
    corrupted_address = (int*)(&b);
    saved = *corrupted_address;
    *corrupted_address = 0;

    corrupted = 0;
    CU_ASSERT (ssm_dbuffer_import(&b, "abcd", 4) == SSM_CORRUPTED);
    CU_ASSERT (corrupted);

    /* Restore the corrupted area */
    *corrupted_address = saved;

    CU_ASSERT (ssm_dbuffer_free(&b) == SSM_OK);
    CU_ASSERT (ssm_dbuffer_length(&b) == 0);
}

static void test_canary_corrupt_data (void)
{
    ssm_dbuffer_declare(b);
    int* corrupted_address;
    int saved;

    CU_ASSERT (ssm_dbuffer_import(&b, "abcd", 4) == SSM_OK);
    CU_ASSERT (ssm_dbuffer_length(&b) == 4);
    CU_ASSERT (memcmp(ssm_dbuffer_data(&b), "abcd", 4) == 0);

    /* Corrupt the allocated memory */
    CU_ASSERT (b._data != NULL);
    corrupted_address = (int*)(b._data);
    saved = *corrupted_address;
    *corrupted_address = 0;

    corrupted = 0;
    CU_ASSERT (ssm_dbuffer_import(&b, "abcd", 4) == SSM_CORRUPTED);
    CU_ASSERT (corrupted);

    /* Restore the corrupted area */
    *corrupted_address = saved;

    CU_ASSERT (ssm_dbuffer_free(&b) == SSM_OK);
    CU_ASSERT (ssm_dbuffer_length(&b) == 0);
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
void ssm_dbuffer_canary_test_init(void)
{
    const CU_pSuite suite = CU_add_suite ("dbuffer_canary", init_suite_canary, cleanup_suite_canary);

    /* Add same tests as in non-canary functions */
    ssm_dbuffer_add_tests(suite);

    /* Add canary-specific tests */
    CU_ADD_TEST (suite, test_canary_corrupt_base);
    CU_ADD_TEST (suite, test_canary_corrupt_data);
}

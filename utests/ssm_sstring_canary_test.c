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
 *  CUnit test suite for ssm_sstring_t with canary runtime checks.
 *
 *-----------------------------------------------------------------------------
 *
 *  Modification history:
 *    2014-01-06 Thierry Lelegard
 *               Original creation.
 *
 *-----------------------------------------------------------------------------
 */

#include "ssm_sstring_canary_test.h"
#define SSM_USE_CANARY 1
#include "ssm.h"
#include "utests.h"

/*
 * Non-canary-specific test cases (same tests as standard functions).
 */
#include "ssm_sstring_test.c"

/*
 * A memory corruption handled which raises a flag.
 */
static int corrupted = 0;

static void corrupted_handler (const char* file, size_t line)
{
    corrupted = 1;
    fprintf (utests_out(), "ssm_sstring_canary_test: memory corruption detected at %s:%d\n", file, (int)line);
}

/*
 * Canary-specific test cases
 */
static void test_canary (void)
{
    ssm_sstring_declare (s, 10);
    fprintf (utests_out(), "ssm_sstring_canary_test: size of 10-chars static string: %d bytes\n", (int)sizeof(s));

    CU_ASSERT (ssm_sstring_import(&s.str, "abcd") == SSM_OK);
    CU_ASSERT (ssm_sstring_length(&s.str) == 4);
    CU_ASSERT (strcmp(ssm_sstring_chars(&s.str), "abcd") == 0);

    memset(&s, 0, 4);
    corrupted = 0;
    CU_ASSERT (ssm_sstring_import(&s.str, "abcd") == SSM_CORRUPTED);
    CU_ASSERT (corrupted);
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
void ssm_sstring_canary_test_init(void)
{
    const CU_pSuite suite = CU_add_suite ("sstring_canary", init_suite_canary, cleanup_suite_canary);

    /* Add same tests as in non-canary functions */
    ssm_sstring_test_add_tests(suite);

    /* Add canary-specific tests */
    CU_ADD_TEST (suite, test_canary);
}

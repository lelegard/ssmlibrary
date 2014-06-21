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
 *    2014-06-20 Thierry Lelegard
 *               Added ssm_status_string test.
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
    CU_ASSERT (!ssm_success(SSM_SIZEZERO));
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
    CU_ASSERT (ssm_error(SSM_SIZEZERO));
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
    CU_ASSERT (!ssm_fatal(SSM_SIZEZERO));
    CU_ASSERT (ssm_fatal(SSM_NOMEMORY));
    CU_ASSERT (ssm_fatal(SSM_CORRUPTED));
    CU_ASSERT (ssm_fatal(SSM_BUG));
}

static void test_strings(void)
{
    int i;
    for (i = 0; i < 256; i++) {
        const ssm_status_t status = (ssm_status_t)i;
        const char* msg = ssm_status_string(status);
        CU_ASSERT_PTR_NOT_NULL(msg);
        switch (status) {
        case SSM_OK:           CU_ASSERT_STRING_EQUAL (msg, "The function executed successfully"); break;
        case SSM_TRUNCATED:    CU_ASSERT_STRING_EQUAL (msg, "The result is truncated but safe"); break;
        case SSM_EQUAL:        CU_ASSERT_STRING_EQUAL (msg, "Objects are equal after comparison"); break;
        case SSM_LOWER:        CU_ASSERT_STRING_EQUAL (msg, "Object 1 is lower than object 2 after comparison"); break;
        case SSM_GREATER:      CU_ASSERT_STRING_EQUAL (msg, "Object 1 is greater than object 2 after comparison"); break;
        case SSM_NULLOUT:      CU_ASSERT_STRING_EQUAL (msg, "A NULL pointer was provided as output parameter"); break;
        case SSM_SIZETOOLARGE: CU_ASSERT_STRING_EQUAL (msg, "Some size is larger than SSM_SIZE_MAX"); break;
        case SSM_INDEXRANGE:   CU_ASSERT_STRING_EQUAL (msg, "An index parameter in out of range"); break;
        case SSM_SIZEZERO:     CU_ASSERT_STRING_EQUAL (msg, "Some size is zero"); break;
        case SSM_NOMEMORY:     CU_ASSERT_STRING_EQUAL (msg, "Memory allocation failure, result is unchanged"); break;
        case SSM_CORRUPTED:    CU_ASSERT_STRING_EQUAL (msg, "Memory was previously corrupted, result is undefined but safe"); break;
        case SSM_BUG:          CU_ASSERT_STRING_EQUAL (msg, "Internal inconsistency, there is a bug in the SSM library"); break;
        default:               CU_ASSERT_STRING_EQUAL (msg, "Unknown"); break;
        }
    }
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
    CU_ADD_TEST (suite, test_strings);
}

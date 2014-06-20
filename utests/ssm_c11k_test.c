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
 *  CUnit test suite for the C11 Annex K functions (aka "_s" functions).
 *
 *-----------------------------------------------------------------------------
 *
 *  Modification history:
 *    2014-06-00 Thierry Lelegard
 *               Original creation.
 *
 *-----------------------------------------------------------------------------
 */

#define SSM_C11K
#include "ssm_c11k_test.h"
#include "ssm.h"
#include "utests.h"

/*
 * Test cases
 */
static void test_types (void)
{
    CU_ASSERT (sizeof(errno_t) == sizeof(int));
    CU_ASSERT (sizeof(rsize_t) == sizeof(size_t));
    CU_ASSERT (sizeof(ssm_errno_t) == sizeof(int));
    CU_ASSERT (sizeof(ssm_rsize_t) == sizeof(size_t));

    CU_ASSERT ((errno_t)(-1) < 0);      /* errno_t is signed */
    CU_ASSERT ((rsize_t)(-1) > 0);      /* rsize_t is unsigned */
    CU_ASSERT ((ssm_errno_t)(-1) < 0);  /* ssm_errno_t is signed */
    CU_ASSERT ((ssm_rsize_t)(-1) > 0);  /* ssm_rsize_t is unsigned */

    CU_ASSERT (RSIZE_MAX == SSM_SIZE_MAX);
    CU_ASSERT (SSM_RSIZE_MAX == SSM_SIZE_MAX);
}

static void test_memmove_s(void)
{
    //@@
}

static void test_memcpy_s (void)
{
    //@@
}

static void test_memset_s (void)
{
    //@@
}

static void test_strcpy_s (void)
{
    //@@
}

static void test_strncpy_s (void)
{
    //@@
}

static void test_strcat_s (void)
{
    //@@
}

static void test_strncat_s (void)
{
    //@@
}

static void test_strnlen_s (void)
{
    //@@
}

static void test_strerror_s (void)
{
    //@@
}

static void test_strerrorlen_s (void)
{
    //@@
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
void ssm_c11k_test_init(void)
{
    const CU_pSuite suite = CU_add_suite ("c11k", init_suite, cleanup_suite);
    CU_ADD_TEST (suite, test_types);
    CU_ADD_TEST (suite, test_memmove_s);
    CU_ADD_TEST (suite, test_memcpy_s);
    CU_ADD_TEST (suite, test_memset_s);
    CU_ADD_TEST (suite, test_strcpy_s);
    CU_ADD_TEST (suite, test_strncpy_s);
    CU_ADD_TEST (suite, test_strcat_s);
    CU_ADD_TEST (suite, test_strncat_s);
    CU_ADD_TEST (suite, test_strnlen_s);
    CU_ADD_TEST (suite, test_strerror_s);
    CU_ADD_TEST (suite, test_strerrorlen_s);
}

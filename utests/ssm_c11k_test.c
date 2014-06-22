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
    errno_t i;
    errno_t ret;
    char msg[41];

    msg[0] = '\0';
    CU_ASSERT (strerror_s(NULL, 1, 0) == SSM_NULLOUT);
    CU_ASSERT (strerror_s(msg, 0, 0) == SSM_SIZEZERO);
    CU_ASSERT (msg[0] == '\0');
    CU_ASSERT (strerror_s(msg, RSIZE_MAX + 1, 0) == SSM_SIZETOOLARGE);
    CU_ASSERT (msg[0] == '\0');

    msg[0] = '.';
    CU_ASSERT (strerror_s(msg, 1, 0) == SSM_TRUNCATED);
    CU_ASSERT_STRING_EQUAL (msg, "");

    msg[0] = '.';
    CU_ASSERT (strerror_s(msg, 2, 0) == SSM_TRUNCATED);
    CU_ASSERT_STRING_EQUAL (msg, "T");

    msg[0] = '.';
    CU_ASSERT (strerror_s(msg, 3, 0) == SSM_TRUNCATED);
    CU_ASSERT_STRING_EQUAL (msg, "Th");

    msg[0] = '.';
    CU_ASSERT (strerror_s(msg, 4, 0) == SSM_TRUNCATED);
    CU_ASSERT_STRING_EQUAL (msg, "...");

    for (i = 0; i < 256; i++) {
        msg[0] = '\0';
        ret = strerror_s(msg, sizeof(msg), i);
        switch (i) {
        case SSM_OK:
            CU_ASSERT (ret == SSM_OK);
            CU_ASSERT_STRING_EQUAL (msg, "The function executed successfully");
            break;
        case SSM_TRUNCATED:
            CU_ASSERT (ret == SSM_OK);
            CU_ASSERT_STRING_EQUAL (msg, "The result is truncated but safe");
            break;
        case SSM_EQUAL:
            CU_ASSERT (ret == SSM_OK);
            CU_ASSERT_STRING_EQUAL (msg, "Objects are equal after comparison");
            break;
        case SSM_LOWER:
            CU_ASSERT (ret == SSM_TRUNCATED);
            CU_ASSERT_STRING_EQUAL (msg, "Object 1 is lower than object 2 after...");
            break;
        case SSM_GREATER:
            CU_ASSERT (ret == SSM_TRUNCATED);
            CU_ASSERT_STRING_EQUAL (msg, "Object 1 is greater than object 2 aft...");
            break;
        case SSM_NULLOUT:
            CU_ASSERT (ret == SSM_TRUNCATED);
            CU_ASSERT_STRING_EQUAL (msg, "A NULL pointer was provided as output...");
            break;
        case SSM_SIZETOOLARGE:
            CU_ASSERT (ret == SSM_OK);
            CU_ASSERT_STRING_EQUAL (msg, "Some size is larger than SSM_SIZE_MAX");
            break;
        case SSM_INDEXRANGE:
            CU_ASSERT (ret == SSM_OK);
            CU_ASSERT_STRING_EQUAL (msg, "An index parameter in out of range");
            break;
        case SSM_SIZEZERO:
            CU_ASSERT (ret == SSM_OK);
            CU_ASSERT_STRING_EQUAL (msg, "Some size is zero");
            break;
        case SSM_NULLIN:
            CU_ASSERT (ret == SSM_TRUNCATED);
            CU_ASSERT_STRING_EQUAL (msg, "A NULL pointer was provided as input ...");
            break;
        case SSM_NOMEMORY:
            CU_ASSERT (ret == SSM_TRUNCATED);
            CU_ASSERT_STRING_EQUAL (msg, "Memory allocation failure, result is ...");
            break;
        case SSM_CORRUPTED:
            CU_ASSERT (ret == SSM_TRUNCATED);
            CU_ASSERT_STRING_EQUAL (msg, "Memory was previously corrupted, resu...");
            break;
        case SSM_BUG:
            CU_ASSERT (ret == SSM_TRUNCATED);
            CU_ASSERT_STRING_EQUAL (msg, "Internal inconsistency, there is a bu...");
            break;
        default:
            CU_ASSERT (ret == SSM_OK);
            CU_ASSERT_STRING_EQUAL (msg, "Unknown");
            break;
        }
    }
}

static void test_strerrorlen_s (void)
{
    errno_t i;
    for (i = 0; i < 256; i++) {
        const size_t len = strerrorlen_s(i);
        switch (i) {
        case SSM_OK:           CU_ASSERT (len == 34); break;
        case SSM_TRUNCATED:    CU_ASSERT (len == 32); break;
        case SSM_EQUAL:        CU_ASSERT (len == 34); break;
        case SSM_LOWER:        CU_ASSERT (len == 48); break;
        case SSM_GREATER:      CU_ASSERT (len == 50); break;
        case SSM_NULLOUT:      CU_ASSERT (len == 47); break;
        case SSM_SIZETOOLARGE: CU_ASSERT (len == 37); break;
        case SSM_INDEXRANGE:   CU_ASSERT (len == 34); break;
        case SSM_SIZEZERO:     CU_ASSERT (len == 17); break;
        case SSM_NULLIN:       CU_ASSERT (len == 46); break;
        case SSM_NOMEMORY:     CU_ASSERT (len == 46); break;
        case SSM_CORRUPTED:    CU_ASSERT (len == 61); break;
        case SSM_BUG:          CU_ASSERT (len == 57); break;
        default:               CU_ASSERT (len == 7); break;
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

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
 *  CUnit test suite for the C-string functions.
 *
 *-----------------------------------------------------------------------------
 *
 *  Modification history:
 *    2014-01-10 Thierry Lelegard
 *               Original creation.
 *
 *-----------------------------------------------------------------------------
 */

#include "ssm_cstring_test.h"
#include "ssm.h"
#include "utests.h"

/*
 * Test cases
 */
static void test_cstring_length(void)
{
    CU_ASSERT (ssm_cstring_length(NULL, 0) == 0);
    CU_ASSERT (ssm_cstring_length(NULL, 10) == 0);
    CU_ASSERT (ssm_cstring_length("", 0) == 0);
    CU_ASSERT (ssm_cstring_length("", 10) == 0);
    CU_ASSERT (ssm_cstring_length("abcdefghijkl", 0) == 0);
    CU_ASSERT (ssm_cstring_length("abcdefghijkl", 11) == 11);
    CU_ASSERT (ssm_cstring_length("abcdefghijkl", 12) == 12);
    CU_ASSERT (ssm_cstring_length("abcdefghijkl", 13) == 12);
    CU_ASSERT (ssm_cstring_length("abcdefghijkl", SSM_SIZE_MAX) == 12);
}

static void test_cstring_length_alignment(void)
{
    static char const data[] = "abcdefghijklmnopqrs"; /* 19 chars + trailing zero */
    char buf[32];

    /* Prefill buf to make sure valgrind does not complain about uninitialized data. */
    /* See comment on valgrind in file ssm_cstring_length.c. */
    memset(buf, 0, sizeof(buf));

    /* Test various alignments for starting address */
    CU_ASSERT (ssm_cstring_length(data +  0, SSM_SIZE_MAX) == 19);
    CU_ASSERT (ssm_cstring_length(data +  1, SSM_SIZE_MAX) == 18);
    CU_ASSERT (ssm_cstring_length(data +  2, SSM_SIZE_MAX) == 17);
    CU_ASSERT (ssm_cstring_length(data +  3, SSM_SIZE_MAX) == 16);
    CU_ASSERT (ssm_cstring_length(data +  4, SSM_SIZE_MAX) == 15);
    CU_ASSERT (ssm_cstring_length(data +  5, SSM_SIZE_MAX) == 14);
    CU_ASSERT (ssm_cstring_length(data +  6, SSM_SIZE_MAX) == 13);
    CU_ASSERT (ssm_cstring_length(data +  7, SSM_SIZE_MAX) == 12);
    CU_ASSERT (ssm_cstring_length(data +  8, SSM_SIZE_MAX) == 11);
    CU_ASSERT (ssm_cstring_length(data +  9, SSM_SIZE_MAX) == 10);
    CU_ASSERT (ssm_cstring_length(data + 10, SSM_SIZE_MAX) ==  9);
    CU_ASSERT (ssm_cstring_length(data + 11, SSM_SIZE_MAX) ==  8);
    CU_ASSERT (ssm_cstring_length(data + 12, SSM_SIZE_MAX) ==  7);
    CU_ASSERT (ssm_cstring_length(data + 13, SSM_SIZE_MAX) ==  6);
    CU_ASSERT (ssm_cstring_length(data + 14, SSM_SIZE_MAX) ==  5);
    CU_ASSERT (ssm_cstring_length(data + 15, SSM_SIZE_MAX) ==  4);
    CU_ASSERT (ssm_cstring_length(data + 16, SSM_SIZE_MAX) ==  3);
    CU_ASSERT (ssm_cstring_length(data + 17, SSM_SIZE_MAX) ==  2);
    CU_ASSERT (ssm_cstring_length(data + 18, SSM_SIZE_MAX) ==  1);
    CU_ASSERT (ssm_cstring_length(data + 19, SSM_SIZE_MAX) ==  0);

    /* Test various alignments on short strings (smaller that a machine word) */
    memcpy(buf + 0, "ab", 3); CU_ASSERT (ssm_cstring_length(buf + 0, SSM_SIZE_MAX) == 2);
    memcpy(buf + 1, "ab", 3); CU_ASSERT (ssm_cstring_length(buf + 1, SSM_SIZE_MAX) == 2);
    memcpy(buf + 2, "ab", 3); CU_ASSERT (ssm_cstring_length(buf + 2, SSM_SIZE_MAX) == 2);
    memcpy(buf + 3, "ab", 3); CU_ASSERT (ssm_cstring_length(buf + 3, SSM_SIZE_MAX) == 2);
    memcpy(buf + 4, "ab", 3); CU_ASSERT (ssm_cstring_length(buf + 4, SSM_SIZE_MAX) == 2);
    memcpy(buf + 5, "ab", 3); CU_ASSERT (ssm_cstring_length(buf + 5, SSM_SIZE_MAX) == 2);
    memcpy(buf + 6, "ab", 3); CU_ASSERT (ssm_cstring_length(buf + 6, SSM_SIZE_MAX) == 2);
    memcpy(buf + 7, "ab", 3); CU_ASSERT (ssm_cstring_length(buf + 7, SSM_SIZE_MAX) == 2);
    memcpy(buf + 8, "ab", 3); CU_ASSERT (ssm_cstring_length(buf + 8, SSM_SIZE_MAX) == 2);

    /* Test various alignments for trailing null character */
    memcpy(buf, data +  0, 20); CU_ASSERT (ssm_cstring_length(buf, SSM_SIZE_MAX) == 19);
    memcpy(buf, data +  1, 19); CU_ASSERT (ssm_cstring_length(buf, SSM_SIZE_MAX) == 18);
    memcpy(buf, data +  2, 18); CU_ASSERT (ssm_cstring_length(buf, SSM_SIZE_MAX) == 17);
    memcpy(buf, data +  3, 17); CU_ASSERT (ssm_cstring_length(buf, SSM_SIZE_MAX) == 16);
    memcpy(buf, data +  4, 16); CU_ASSERT (ssm_cstring_length(buf, SSM_SIZE_MAX) == 15);
    memcpy(buf, data +  5, 15); CU_ASSERT (ssm_cstring_length(buf, SSM_SIZE_MAX) == 14);
    memcpy(buf, data +  6, 14); CU_ASSERT (ssm_cstring_length(buf, SSM_SIZE_MAX) == 13);
    memcpy(buf, data +  7, 13); CU_ASSERT (ssm_cstring_length(buf, SSM_SIZE_MAX) == 12);
    memcpy(buf, data +  8, 12); CU_ASSERT (ssm_cstring_length(buf, SSM_SIZE_MAX) == 11);
    memcpy(buf, data +  9, 11); CU_ASSERT (ssm_cstring_length(buf, SSM_SIZE_MAX) == 10);
    memcpy(buf, data + 10, 10); CU_ASSERT (ssm_cstring_length(buf, SSM_SIZE_MAX) ==  9);
    memcpy(buf, data + 11,  9); CU_ASSERT (ssm_cstring_length(buf, SSM_SIZE_MAX) ==  8);
    memcpy(buf, data + 12,  8); CU_ASSERT (ssm_cstring_length(buf, SSM_SIZE_MAX) ==  7);
    memcpy(buf, data + 13,  7); CU_ASSERT (ssm_cstring_length(buf, SSM_SIZE_MAX) ==  6);
    memcpy(buf, data + 14,  6); CU_ASSERT (ssm_cstring_length(buf, SSM_SIZE_MAX) ==  5);
    memcpy(buf, data + 15,  5); CU_ASSERT (ssm_cstring_length(buf, SSM_SIZE_MAX) ==  4);
    memcpy(buf, data + 16,  4); CU_ASSERT (ssm_cstring_length(buf, SSM_SIZE_MAX) ==  3);
    memcpy(buf, data + 17,  3); CU_ASSERT (ssm_cstring_length(buf, SSM_SIZE_MAX) ==  2);
    memcpy(buf, data + 18,  2); CU_ASSERT (ssm_cstring_length(buf, SSM_SIZE_MAX) ==  1);
    memcpy(buf, data + 19,  1); CU_ASSERT (ssm_cstring_length(buf, SSM_SIZE_MAX) ==  0);
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
void ssm_cstring_test_init(void)
{
    const CU_pSuite suite = CU_add_suite ("cstring", init_suite, cleanup_suite);
    CU_ADD_TEST (suite, test_cstring_length);
    CU_ADD_TEST (suite, test_cstring_length_alignment);
}

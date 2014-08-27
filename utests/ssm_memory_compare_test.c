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
 *  CUnit test suite for the memory compare functions.
 *
 *-----------------------------------------------------------------------------
 *
 *  Modification history:
 *    2014-01-10 Thierry Lelegard
 *               Original creation.
 *
 *-----------------------------------------------------------------------------
 */

#include "ssm_memory_compare_test.h"
#include "ssm.h"
#include "utests.h"

/*
 * Test cases
 */
static void test_null(void)
{
    CU_ASSERT (ssm_compare(NULL, 10, NULL, 10) == SSM_EQUAL);
    CU_ASSERT (ssm_compare(NULL, 10, NULL, 5) == SSM_EQUAL);
    CU_ASSERT (ssm_compare(NULL, 5, NULL, 10) == SSM_EQUAL);
    CU_ASSERT (ssm_compare(NULL, 0, NULL, 10) == SSM_EQUAL);
    CU_ASSERT (ssm_compare(NULL, 10, NULL, 0) == SSM_EQUAL);
    CU_ASSERT (ssm_compare(NULL, 0, NULL, 0) == SSM_EQUAL);
    CU_ASSERT (ssm_compare(NULL, 4, "", 0) == SSM_EQUAL);
    CU_ASSERT (ssm_compare("", 0, NULL, 4) == SSM_EQUAL);

    CU_ASSERT (ssm_compare("ab", 2, NULL, 0) == SSM_GREATER);
    CU_ASSERT (ssm_compare("ab", 2, NULL, 4) == SSM_GREATER);
    CU_ASSERT (ssm_compare(NULL, 0, "ab", 2) == SSM_LOWER);
    CU_ASSERT (ssm_compare(NULL, 4, "ab", 2) == SSM_LOWER);
}

static void test_compare(void)
{
    CU_ASSERT (ssm_compare("", 0, "", 0) == SSM_EQUAL);
    CU_ASSERT (ssm_compare("", 0, "a", 1) == SSM_LOWER);
    CU_ASSERT (ssm_compare("a", 1, "", 0) == SSM_GREATER);
    CU_ASSERT (ssm_compare("ab", 2, "ab", 2) == SSM_EQUAL);
    CU_ASSERT (ssm_compare("ab", 2, "abcd", 4) == SSM_LOWER);
    CU_ASSERT (ssm_compare("abc", 3, "ab", 2) == SSM_GREATER);
    CU_ASSERT (ssm_compare("abcdef", 6, "abddef", 6) == SSM_LOWER);
    CU_ASSERT (ssm_compare("abcdef", 6, "abddef", SSM_SIZE_MAX + 1) == SSM_SIZETOOLARGE);
}

static void test_all_alignments(void)
{
    uint8_t buf1[256];
    uint8_t buf2[256];
    size_t start1;
    size_t start2;
    size_t size1;
    size_t size2;
    size_t diffIndex;
    int equal_tests = 0;
    int lg_tests = 0;
    int comp;

    /* CUnit does not like a huge number of CU_ASSERT. */
    /* Count errors and then assert that there was no error. */
    int equal_errors = 0;
    int lower_errors = 0;
    int greater_errors = 0;

    memset(buf1, 0x80, sizeof(buf1));
    memset(buf2, 0x80, sizeof(buf2));

    /* Test all possible combinations of data alignments on 8 bytes */
    for (start1 = 0; start1 < 8; ++start1) {
        for (start2 = 0; start2 < 8; ++start2) {
            for (size1 = 1; size1 <= 64; ++size1) {
                for (size2 = 1; size2 <= 64; ++size2) {

                    /* Compare buffers with identical contents, various sizes */
                    comp = ssm_compare(buf1 + start1, size1, buf2 + start2, size2);
                    if (size1 == size2) {
                        equal_errors += comp != SSM_EQUAL;
                    }
                    else if (size1 < size2) {
                        equal_errors += comp != SSM_LOWER;
                    }
                    else {
                        equal_errors += comp != SSM_GREATER;
                    }
                    equal_tests++;

                    /* Compare buffers differing by one byte, either lower or greater, */
                    /* for all possible positions of the differing byte */
                    for (diffIndex = 0; diffIndex < size1 && diffIndex < size2; ++diffIndex) {

                        /* On byte is lower in buf1 */
                        buf1[start1 + diffIndex] = 0x10;
                        comp = ssm_compare(buf1 + start1, size1, buf2 + start2, size2);
                        lower_errors += comp != SSM_LOWER;

                        /* On byte is greater in buf1 */
                        buf1[start1 + diffIndex] = 0xC0;
                        comp = ssm_compare(buf1 + start1, size1, buf2 + start2, size2);
                        greater_errors += comp != SSM_GREATER;

                        /* Restore previous value */
                        buf1[start1 + diffIndex] = 0x80;
                        lg_tests++;
                    }
                }
            }
        }
    }

    fprintf(utests_out(), "memory_compare: equal_errors = %d/%d\n", equal_errors, equal_tests);
    fprintf(utests_out(), "memory_compare: lower_errors = %d/%d\n", lower_errors, lg_tests);
    fprintf(utests_out(), "memory_compare: greater_errors = %d/%d\n", greater_errors, lg_tests);

    CU_ASSERT (equal_errors == 0);
    CU_ASSERT (lower_errors == 0);
    CU_ASSERT (greater_errors == 0);
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
void ssm_memory_compare_test_init(void)
{
    const CU_pSuite suite = CU_add_suite ("memory_compare", init_suite, cleanup_suite);
    CU_ADD_TEST (suite, test_null);
    CU_ADD_TEST (suite, test_compare);
    CU_ADD_TEST (suite, test_all_alignments);
}

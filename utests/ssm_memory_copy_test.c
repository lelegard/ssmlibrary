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
 *  CUnit test suite for the memory copy functions.
 *
 *-----------------------------------------------------------------------------
 *
 *  Modification history:
 *    2014-01-10 Thierry Lelegard
 *               Original creation.
 *
 *-----------------------------------------------------------------------------
 */

#include "ssm_memory_copy_test.h"
#include "ssm.h"
#include "utests.h"

/*
 * Test cases
 */
static void test_copy(void)
{
    char buf[12];
    size_t size = 0;

    CU_ASSERT (ssm_copy(NULL, 10, "abcdefghij", 10, NULL) == SSM_NULLOUT);
    CU_ASSERT (ssm_copy(buf, sizeof(buf), "abcdefghij", SSM_SIZE_MAX + 1, NULL) == SSM_SIZETOOLARGE);

    CU_ASSERT (ssm_copy(buf, sizeof(buf), "abcdefghij", 10, NULL) == SSM_OK);
    CU_ASSERT (memcmp(buf, "abcdefghij", 10) == 0);

    size = 0;
    CU_ASSERT (ssm_copy(buf, sizeof(buf), "0123456789", 10, &size) == SSM_OK);
    CU_ASSERT (memcmp(buf, "0123456789", 10) == 0);
    CU_ASSERT (size == 10);

    CU_ASSERT (ssm_copy(buf, sizeof(buf), NULL, 10, NULL) == SSM_OK);
    CU_ASSERT (memcmp(buf, "0123456789", 10) == 0); /* check unmodified */

    CU_ASSERT (ssm_copy(buf, sizeof(buf), NULL, 10, &size) == SSM_OK);
    CU_ASSERT (memcmp(buf, "0123456789", 10) == 0); /* check unmodified */
    CU_ASSERT (size == 0);

    CU_ASSERT (ssm_copy(buf, sizeof(buf), "abcdefghijkl", 12, NULL) == SSM_OK);
    CU_ASSERT (memcmp(buf, "abcdefghijkl", 12) == 0);

    size = 0;
    CU_ASSERT (ssm_copy(buf, sizeof(buf), "0123456789AB", 12, &size) == SSM_OK);
    CU_ASSERT (memcmp(buf, "0123456789AB", 12) == 0);
    CU_ASSERT (size == 12);

    CU_ASSERT (ssm_copy(buf, sizeof(buf), "abcdefghijklm", 13, NULL) == SSM_TRUNCATED);
    CU_ASSERT (memcmp(buf, "abcdefghijkl", 12) == 0);

    size = 0;
    CU_ASSERT (ssm_copy(buf, sizeof(buf), "0123456789ABC", 13, &size) == SSM_TRUNCATED);
    CU_ASSERT (memcmp(buf, "0123456789AB", 12) == 0);
    CU_ASSERT (size == 12);
}

static void test_small(void)
{
    static char const data[] = "abcdefghi"; /* 9 chars */
    char buf[100];
    size_t size = 1;

    CU_ASSERT (ssm_copy(buf, sizeof(buf), data + 9, 0, &size) == SSM_OK);
    CU_ASSERT (size == 0);

    CU_ASSERT (ssm_copy(buf, sizeof(buf), data + 8, 1, &size) == SSM_OK);
    CU_ASSERT (memcmp(buf, "i", 1) == 0);
    CU_ASSERT (size == 1);

    CU_ASSERT (ssm_copy(buf, sizeof(buf), data + 7, 2, &size) == SSM_OK);
    CU_ASSERT (memcmp(buf, "hi", 2) == 0);
    CU_ASSERT (size == 2);

    CU_ASSERT (ssm_copy(buf, sizeof(buf), data + 6, 3, &size) == SSM_OK);
    CU_ASSERT (memcmp(buf, "ghi", 3) == 0);
    CU_ASSERT (size == 3);

    CU_ASSERT (ssm_copy(buf, sizeof(buf), data + 5, 4, &size) == SSM_OK);
    CU_ASSERT (memcmp(buf, "fghi", 4) == 0);
    CU_ASSERT (size == 4);

    CU_ASSERT (ssm_copy(buf, sizeof(buf), data + 4, 5, &size) == SSM_OK);
    CU_ASSERT (memcmp(buf, "efghi", 5) == 0);
    CU_ASSERT (size == 5);

    CU_ASSERT (ssm_copy(buf, sizeof(buf), data + 3, 6, &size) == SSM_OK);
    CU_ASSERT (memcmp(buf, "defghi", 6) == 0);
    CU_ASSERT (size == 6);

    CU_ASSERT (ssm_copy(buf, sizeof(buf), data + 2, 7, &size) == SSM_OK);
    CU_ASSERT (memcmp(buf, "cdefghi", 7) == 0);
    CU_ASSERT (size == 7);

    CU_ASSERT (ssm_copy(buf, sizeof(buf), data + 1, 8, &size) == SSM_OK);
    CU_ASSERT (memcmp(buf, "bcdefghi", 8) == 0);
    CU_ASSERT (size == 8);

    CU_ASSERT (ssm_copy(buf, sizeof(buf), data, 9, &size) == SSM_OK);
    CU_ASSERT (memcmp(buf, "abcdefghi", 9) == 0);
    CU_ASSERT (size == 9);
}

static void test_alignment(void)
{
    static char const data[] = "abcdefghijklmnopqrs"; /* 19 chars */
    char buf[100];

    CU_ASSERT (ssm_copy(buf, sizeof(buf), data, 19, NULL) == SSM_OK);
    CU_ASSERT (memcmp(buf, "abcdefghijklmnopqrs", 19) == 0);

    CU_ASSERT (ssm_copy(buf, sizeof(buf), data + 1, 18, NULL) == SSM_OK);
    CU_ASSERT (memcmp(buf, "bcdefghijklmnopqrss", 19) == 0);

    CU_ASSERT (ssm_copy(buf, sizeof(buf), data + 2, 17, NULL) == SSM_OK);
    CU_ASSERT (memcmp(buf, "cdefghijklmnopqrsss", 19) == 0);

    CU_ASSERT (ssm_copy(buf, sizeof(buf), data + 3, 16, NULL) == SSM_OK);
    CU_ASSERT (memcmp(buf, "defghijklmnopqrssss", 19) == 0);

    CU_ASSERT (ssm_copy(buf, sizeof(buf), data + 4, 15, NULL) == SSM_OK);
    CU_ASSERT (memcmp(buf, "efghijklmnopqrsssss", 19) == 0);

    CU_ASSERT (ssm_copy(buf, sizeof(buf), data + 5, 14, NULL) == SSM_OK);
    CU_ASSERT (memcmp(buf, "fghijklmnopqrssssss", 19) == 0);

    CU_ASSERT (ssm_copy(buf, sizeof(buf), data + 6, 13, NULL) == SSM_OK);
    CU_ASSERT (memcmp(buf, "ghijklmnopqrsssssss", 19) == 0);

    CU_ASSERT (ssm_copy(buf, sizeof(buf), data + 7, 12, NULL) == SSM_OK);
    CU_ASSERT (memcmp(buf, "hijklmnopqrssssssss", 19) == 0);

    CU_ASSERT (ssm_copy(buf, sizeof(buf), data, 19, NULL) == SSM_OK);
    CU_ASSERT (memcmp(buf, "abcdefghijklmnopqrs", 19) == 0);

    CU_ASSERT (ssm_copy(buf + 1, sizeof(buf) - 1, data, 18, NULL) == SSM_OK);
    CU_ASSERT (memcmp(buf, "aabcdefghijklmnopqr", 19) == 0);

    CU_ASSERT (ssm_copy(buf + 2, sizeof(buf) - 2, data, 17, NULL) == SSM_OK);
    CU_ASSERT (memcmp(buf, "aaabcdefghijklmnopq", 19) == 0);

    CU_ASSERT (ssm_copy(buf + 3, sizeof(buf) - 3, data, 16, NULL) == SSM_OK);
    CU_ASSERT (memcmp(buf, "aaaabcdefghijklmnop", 19) == 0);

    CU_ASSERT (ssm_copy(buf + 4, sizeof(buf) - 4, data, 15, NULL) == SSM_OK);
    CU_ASSERT (memcmp(buf, "aaaaabcdefghijklmno", 19) == 0);

    CU_ASSERT (ssm_copy(buf + 5, sizeof(buf) - 5, data, 14, NULL) == SSM_OK);
    CU_ASSERT (memcmp(buf, "aaaaaabcdefghijklmn", 19) == 0);

    CU_ASSERT (ssm_copy(buf + 6, sizeof(buf) - 6, data, 13, NULL) == SSM_OK);
    CU_ASSERT (memcmp(buf, "aaaaaaabcdefghijklm", 19) == 0);

    CU_ASSERT (ssm_copy(buf + 7, sizeof(buf) - 7, data, 12, NULL) == SSM_OK);
    CU_ASSERT (memcmp(buf, "aaaaaaaabcdefghijkl", 19) == 0);
}

static void test_overlap(void)
{
    char buf[12];
    size_t size = 0;

    CU_ASSERT (ssm_copy(buf, sizeof(buf), "abcdefghijkl", 12, &size) == SSM_OK);
    fprintf (utests_out(), "platform_test: test_copy_overlap(1): %.*s\n", (int)sizeof(buf), buf);
    CU_ASSERT (memcmp(buf, "abcdefghijkl", 12) == 0);
    CU_ASSERT (size == 12);

    CU_ASSERT (ssm_copy(buf, sizeof(buf), buf + 3, 7, &size) == SSM_OK);
    fprintf (utests_out(), "platform_test: test_copy_overlap(2): %.*s\n", (int)sizeof(buf), buf);
    CU_ASSERT (memcmp(buf, "defghijhijkl", 12) == 0);
    CU_ASSERT (size == 7);

    CU_ASSERT (ssm_copy(buf + 3, sizeof(buf) - 3, buf + 1, 5, &size) == SSM_OK);
    fprintf (utests_out(), "platform_test: test_copy_overlap(3): %.*s\n", (int)sizeof(buf), buf);
    CU_ASSERT (memcmp(buf, "defefghiijkl", 12) == 0);
    CU_ASSERT (size == 5);
}

static void test_overlap_alignment_forward(void)
{
    static char const data[] = "abcdefghijklmnopqrs"; /* 19 chars + trailing zero */
    char buf[20];
    size_t size = 0;

    memcpy(buf, data, 20);
    CU_ASSERT (ssm_copy(buf, 20, buf, 20, &size) == SSM_OK);
    CU_ASSERT (size == 20);
    CU_ASSERT (memcmp(buf, "abcdefghijklmnopqrs", 20) == 0);

    memcpy(buf, data, 20);
    CU_ASSERT (ssm_copy(buf, 20, buf, 19, &size) == SSM_OK);
    CU_ASSERT (size == 19);
    CU_ASSERT (memcmp(buf, "abcdefghijklmnopqrs", 20) == 0);

    memcpy(buf, data, 20);
    CU_ASSERT (ssm_copy(buf, 20, buf + 1, 18, &size) == SSM_OK);
    CU_ASSERT (size == 18);
    CU_ASSERT (memcmp(buf, "bcdefghijklmnopqrss", 20) == 0);

    memcpy(buf, data, 20);
    CU_ASSERT (ssm_copy(buf, 20, buf + 2, 17, &size) == SSM_OK);
    CU_ASSERT (size == 17);
    CU_ASSERT (memcmp(buf, "cdefghijklmnopqrsrs", 20) == 0);

    memcpy(buf, data, 20);
    CU_ASSERT (ssm_copy(buf, 20, buf + 3, 16, &size) == SSM_OK);
    CU_ASSERT (size == 16);
    CU_ASSERT (memcmp(buf, "defghijklmnopqrsqrs", 20) == 0);

    memcpy(buf, data, 20);
    CU_ASSERT (ssm_copy(buf, 20, buf + 4, 15, &size) == SSM_OK);
    CU_ASSERT (size == 15);
    CU_ASSERT (memcmp(buf, "efghijklmnopqrspqrs", 20) == 0);

    memcpy(buf, data, 20);
    CU_ASSERT (ssm_copy(buf, 20, buf + 5, 14, &size) == SSM_OK);
    CU_ASSERT (size == 14);
    CU_ASSERT (memcmp(buf, "fghijklmnopqrsopqrs", 20) == 0);

    memcpy(buf, data, 20);
    CU_ASSERT (ssm_copy(buf, 20, buf + 6, 13, &size) == SSM_OK);
    CU_ASSERT (size == 13);
    CU_ASSERT (memcmp(buf, "ghijklmnopqrsnopqrs", 20) == 0);

    memcpy(buf, data, 20);
    CU_ASSERT (ssm_copy(buf, 20, buf + 7, 12, &size) == SSM_OK);
    CU_ASSERT (size == 12);
    CU_ASSERT (memcmp(buf, "hijklmnopqrsmnopqrs", 20) == 0);

    memcpy(buf, data, 20);
    CU_ASSERT (ssm_copy(buf, 20, buf + 8, 11, &size) == SSM_OK);
    CU_ASSERT (size == 11);
    CU_ASSERT (memcmp(buf, "ijklmnopqrslmnopqrs", 20) == 0);

    memcpy(buf, data, 20);
    CU_ASSERT (ssm_copy(buf, 20, buf + 9, 10, &size) == SSM_OK);
    CU_ASSERT (size == 10);
    CU_ASSERT (memcmp(buf, "jklmnopqrsklmnopqrs", 20) == 0);

    memcpy(buf, data, 20);
    CU_ASSERT (ssm_copy(buf, 20, buf + 10, 9, &size) == SSM_OK);
    CU_ASSERT (size == 9);
    CU_ASSERT (memcmp(buf, "klmnopqrsjklmnopqrs", 20) == 0);

    memcpy(buf, data, 20);
    CU_ASSERT (ssm_copy(buf, 20, buf + 11, 8, &size) == SSM_OK);
    CU_ASSERT (size == 8);
    CU_ASSERT (memcmp(buf, "lmnopqrsijklmnopqrs", 20) == 0);

    memcpy(buf, data, 20);
    CU_ASSERT (ssm_copy(buf, 20, buf + 12, 7, &size) == SSM_OK);
    CU_ASSERT (size == 7);
    CU_ASSERT (memcmp(buf, "mnopqrshijklmnopqrs", 20) == 0);

    memcpy(buf, data, 20);
    CU_ASSERT (ssm_copy(buf, 20, buf + 13, 6, &size) == SSM_OK);
    CU_ASSERT (size == 6);
    CU_ASSERT (memcmp(buf, "nopqrsghijklmnopqrs", 20) == 0);

    memcpy(buf, data, 20);
    CU_ASSERT (ssm_copy(buf, 20, buf + 14, 5, &size) == SSM_OK);
    CU_ASSERT (size == 5);
    CU_ASSERT (memcmp(buf, "opqrsfghijklmnopqrs", 20) == 0);

    memcpy(buf, data, 20);
    CU_ASSERT (ssm_copy(buf, 20, buf + 15, 4, &size) == SSM_OK);
    CU_ASSERT (size == 4);
    CU_ASSERT (memcmp(buf, "pqrsefghijklmnopqrs", 20) == 0);

    memcpy(buf, data, 20);
    CU_ASSERT (ssm_copy(buf, 20, buf + 16, 3, &size) == SSM_OK);
    CU_ASSERT (size == 3);
    CU_ASSERT (memcmp(buf, "qrsdefghijklmnopqrs", 20) == 0);

    memcpy(buf, data, 20);
    CU_ASSERT (ssm_copy(buf, 20, buf + 17, 2, &size) == SSM_OK);
    CU_ASSERT (size == 2);
    CU_ASSERT (memcmp(buf, "rscdefghijklmnopqrs", 20) == 0);

    memcpy(buf, data, 20);
    CU_ASSERT (ssm_copy(buf, 20, buf + 18, 1, &size) == SSM_OK);
    CU_ASSERT (size == 1);
    CU_ASSERT (memcmp(buf, "sbcdefghijklmnopqrs", 20) == 0);
}

static void test_overlap_alignment_backward(void)
{
    static char const data[] = "abcdefghijklmnopqrs"; /* 19 chars + trailing zero */
    char buf[20];
    size_t size = 0;

    memcpy(buf, data, 20);
    CU_ASSERT (ssm_copy(buf + 1, 19, buf, 20, &size) == SSM_TRUNCATED);
    CU_ASSERT (size == 19);
    CU_ASSERT (memcmp(buf, "aabcdefghijklmnopqrs", 20) == 0);

    memcpy(buf, data, 20);
    CU_ASSERT (ssm_copy(buf + 2, 18, buf, 20, &size) == SSM_TRUNCATED);
    CU_ASSERT (size == 18);
    CU_ASSERT (memcmp(buf, "ababcdefghijklmnopqr", 20) == 0);

    memcpy(buf, data, 20);
    CU_ASSERT (ssm_copy(buf + 3, 17, buf, 20, &size) == SSM_TRUNCATED);
    CU_ASSERT (size == 17);
    CU_ASSERT (memcmp(buf, "abcabcdefghijklmnopq", 20) == 0);

    memcpy(buf, data, 20);
    CU_ASSERT (ssm_copy(buf + 4, 16, buf, 20, &size) == SSM_TRUNCATED);
    CU_ASSERT (size == 16);
    CU_ASSERT (memcmp(buf, "abcdabcdefghijklmnop", 20) == 0);

    memcpy(buf, data, 20);
    CU_ASSERT (ssm_copy(buf + 5, 15, buf, 20, &size) == SSM_TRUNCATED);
    CU_ASSERT (size == 15);
    CU_ASSERT (memcmp(buf, "abcdeabcdefghijklmno", 20) == 0);

    memcpy(buf, data, 20);
    CU_ASSERT (ssm_copy(buf + 6, 14, buf, 20, &size) == SSM_TRUNCATED);
    CU_ASSERT (size == 14);
    CU_ASSERT (memcmp(buf, "abcdefabcdefghijklmn", 20) == 0);

    memcpy(buf, data, 20);
    CU_ASSERT (ssm_copy(buf + 7, 13, buf, 20, &size) == SSM_TRUNCATED);
    CU_ASSERT (size == 13);
    CU_ASSERT (memcmp(buf, "abcdefgabcdefghijklm", 20) == 0);

    memcpy(buf, data, 20);
    CU_ASSERT (ssm_copy(buf + 8, 12, buf, 20, &size) == SSM_TRUNCATED);
    CU_ASSERT (size == 12);
    CU_ASSERT (memcmp(buf, "abcdefghabcdefghijkl", 20) == 0);

    memcpy(buf, data, 20);
    CU_ASSERT (ssm_copy(buf + 9, 11, buf, 20, &size) == SSM_TRUNCATED);
    CU_ASSERT (size == 11);
    CU_ASSERT (memcmp(buf, "abcdefghiabcdefghijk", 20) == 0);

    memcpy(buf, data, 20);
    CU_ASSERT (ssm_copy(buf + 10, 10, buf, 20, &size) == SSM_TRUNCATED);
    CU_ASSERT (size == 10);
    CU_ASSERT (memcmp(buf, "abcdefghijabcdefghij", 20) == 0);
}

static void test_all_overlap_alignments(void)
{
    uint8_t buf[128];
    ssm_status_t status;
    size_t size = 0;
    size_t srcSize;
    size_t srcIndex;
    size_t destIndex;
    size_t i;

    /* CUnit does not like a huge number of CU_ASSERT (ends up in an infinite loop). */
    /* Count errors and then assert that there was no error. */
    int status_errors = 0;
    int size_errors = 0;
    int value_errors = 0;

    /* Test all combinations of destination offset, source offset and length */
    for (destIndex = 0; destIndex < sizeof(buf); destIndex++) {
        for (srcIndex = 0; srcIndex < sizeof(buf); srcIndex++) {
            for (srcSize = 0; srcSize <= sizeof(buf) - srcIndex; srcSize++) {

                /* Fill buffer with index values 00 01 .. FE FF */
                for (i = 0; i < sizeof(buf); i++) {
                    buf[i] = (uint8_t)i;
                }

                status = ssm_copy(buf + destIndex, sizeof(buf) - destIndex, buf + srcIndex, srcSize, &size);

                if (srcSize <= sizeof(buf) - destIndex) {
                    status_errors += status != SSM_OK;
                    size_errors += size != srcSize;
                }
                else {
                    status_errors += status != SSM_TRUNCATED;
                    size_errors += size != sizeof(buf) - destIndex;
                }

                for (i = 0; i < sizeof(buf); i++) {
                    if (i >= destIndex && i < destIndex + srcSize) {
                        value_errors += buf[i] != i + srcIndex - destIndex;
                    }
                    else {
                        value_errors += buf[i] != i;
                    }
                }
            }
        }
    }

    CU_ASSERT (status_errors == 0);
    CU_ASSERT (size_errors == 0);
    CU_ASSERT (value_errors == 0);
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
void ssm_memory_copy_test_init(void)
{
    const CU_pSuite suite = CU_add_suite ("memory_copy", init_suite, cleanup_suite);
    CU_ADD_TEST (suite, test_copy);
    CU_ADD_TEST (suite, test_small);
    CU_ADD_TEST (suite, test_alignment);
    CU_ADD_TEST (suite, test_overlap);
    CU_ADD_TEST (suite, test_overlap_alignment_forward);
    CU_ADD_TEST (suite, test_overlap_alignment_backward);
    CU_ADD_TEST (suite, test_all_overlap_alignments);
}

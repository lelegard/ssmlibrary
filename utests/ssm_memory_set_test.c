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
 *  CUnit test suite for the memory set functions.
 *
 *-----------------------------------------------------------------------------
 *
 *  Modification history:
 *    2014-01-10 Thierry Lelegard
 *               Original creation.
 *
 *-----------------------------------------------------------------------------
 */

#include "ssm_memory_set_test.h"
#include "ssm.h"
#include "utests.h"

/*
 * Test cases
 */
static void test_set(void)
{
    char buf[11];
    static const char aa[] = "aaaaaaaaaaa";
    static const char ab[] = "aaabbbbbaaa";

    CU_ASSERT (ssm_set(NULL, 10, 'x') == SSM_NULLOUT);
    CU_ASSERT (ssm_set(buf, SSM_SIZE_MAX + 1, 'x') == SSM_SIZETOOLARGE);

    CU_ASSERT (ssm_set(buf, sizeof(buf), 'a') == SSM_OK);
    CU_ASSERT (memcmp(buf, aa, sizeof(buf)) == 0);

    CU_ASSERT (ssm_set(buf, 0, 'x') == SSM_OK);
    CU_ASSERT (memcmp(buf, aa, sizeof(buf)) == 0);

    CU_ASSERT (ssm_set(buf + 3, 5, 'b') == SSM_OK);
    CU_ASSERT (memcmp(buf, ab, sizeof(buf)) == 0);
}

static void test_all_alignments(void)
{
    uint8_t buf[128];
    ssm_status_t status;
    size_t size;
    size_t index;
    size_t i;

    /* CUnit does not like a huge number of CU_ASSERT . */
    /* Count errors and then assert that there was no error. */
    int status_errors = 0;
    int value_errors = 0;

    /* Test all combinations of offset and length */
    for (index = 0; index < sizeof(buf); index++) {
        for (size = 0; size <= sizeof(buf) - index; size++) {

            /* Fill buffer with index values 00 01 .. FE FF */
            for (i = 0; i < sizeof(buf); i++) {
                buf[i] = (uint8_t)i;
            }

            status = ssm_set(buf + index, size, 27);
            status_errors += status != SSM_OK;

            for (i = 0; i < sizeof(buf); i++) {
                if (i >= index && i < index + size) {
                    value_errors += buf[i] != 27;
                }
                else {
                    value_errors += buf[i] != i;
                }
            }
        }
    }

    CU_ASSERT (status_errors == 0);
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
void ssm_memory_set_test_init(void)
{
    const CU_pSuite suite = CU_add_suite ("memory_set", init_suite, cleanup_suite);
    CU_ADD_TEST (suite, test_set);
    CU_ADD_TEST (suite, test_all_alignments);
}

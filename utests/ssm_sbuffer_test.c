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
 *  CUnit test suite for ssm_sbuffer_t
 *
 *-----------------------------------------------------------------------------
 *
 *  Modification history:
 *    2014-01-10 Thierry Lelegard
 *               Original creation.
 *
 *-----------------------------------------------------------------------------
 */

#include "ssm_sbuffer_test.h"
#include "ssm.h"
#include "utests.h"
#include <string.h>

/*
 * Test cases
 */
static void test_null (void)
{
    CU_ASSERT (ssm_sbuffer_length(NULL) == 0);
    CU_ASSERT (ssm_sbuffer_max_length(NULL) == 0);
    CU_ASSERT (ssm_sbuffer_data(NULL) != NULL);
    CU_ASSERT (ssm_sbuffer_import(NULL, "", 0) == SSM_NULLOUT);
    CU_ASSERT (ssm_sbuffer_import(NULL, NULL, 0) == SSM_NULLOUT);
    CU_ASSERT (ssm_sbuffer_resize(NULL, 100) == SSM_NULLOUT);
    CU_ASSERT (ssm_sbuffer_set(NULL, 0) == SSM_NULLOUT);
    CU_ASSERT (ssm_sbuffer_set_range(NULL, 0, 0, 0) == SSM_NULLOUT);
    CU_ASSERT (ssm_sbuffer_copy(NULL, NULL) == SSM_NULLOUT);
    CU_ASSERT (ssm_sbuffer_concat(NULL, NULL) == SSM_NULLOUT);
    CU_ASSERT (ssm_sbuffer_compare(NULL, NULL) == SSM_EQUAL);
    CU_ASSERT (ssm_sbuffer_insert(NULL, 0, 10) == SSM_NULLOUT);
}

static void test_zero (void)
{
    ssm_sbuffer_declare (b, 0);

    CU_ASSERT (ssm_sbuffer_length(&b.buf) == 0);
    CU_ASSERT (ssm_sbuffer_max_length(&b.buf) == 0);

    CU_ASSERT (ssm_sbuffer_import(&b.buf, "abcd", 4) == SSM_TRUNCATED);
    CU_ASSERT (ssm_sbuffer_length(&b.buf) == 0);
}

static void test_import (void)
{
    ssm_sbuffer_declare (b, 10);
    fprintf (utests_out(), "ssm_sbuffer_test: size of 10-bytes static buffer: %d bytes\n", (int)sizeof(b));

    CU_ASSERT (ssm_sbuffer_length(&b.buf) == 0);
    CU_ASSERT (ssm_sbuffer_max_length(&b.buf) == 10);

    CU_ASSERT (ssm_sbuffer_import(&b.buf, "", SSM_SIZE_MAX + 1) == SSM_SIZETOOLARGE);
    CU_ASSERT (ssm_sbuffer_length(&b.buf) == 0);
    CU_ASSERT (ssm_sbuffer_max_length(&b.buf) == 10);

    CU_ASSERT (ssm_sbuffer_import(&b.buf, "abcd", 4) == SSM_OK);
    CU_ASSERT (ssm_sbuffer_length(&b.buf) == 4);
    CU_ASSERT (ssm_sbuffer_max_length(&b.buf) == 10);
    CU_ASSERT (memcmp(ssm_sbuffer_data(&b.buf), "abcd", 4) == 0);

    CU_ASSERT (ssm_sbuffer_import(&b.buf, NULL, 0) == SSM_OK);
    CU_ASSERT (ssm_sbuffer_length(&b.buf) == 0);
    CU_ASSERT (ssm_sbuffer_max_length(&b.buf) == 10);

    CU_ASSERT (ssm_sbuffer_import(&b.buf, "abcd", 4) == SSM_OK);
    CU_ASSERT (ssm_sbuffer_length(&b.buf) == 4);
    CU_ASSERT (ssm_sbuffer_max_length(&b.buf) == 10);
    CU_ASSERT (memcmp(ssm_sbuffer_data(&b.buf), "abcd", 4) == 0);

    CU_ASSERT (ssm_sbuffer_import(&b.buf, NULL, 100) == SSM_OK);
    CU_ASSERT (ssm_sbuffer_length(&b.buf) == 0);
    CU_ASSERT (ssm_sbuffer_max_length(&b.buf) == 10);
 
    CU_ASSERT (ssm_sbuffer_import(&b.buf, "ABCDEFGHI", 9) == SSM_OK);
    CU_ASSERT (ssm_sbuffer_length(&b.buf) == 9);
    CU_ASSERT (ssm_sbuffer_max_length(&b.buf) == 10);
    CU_ASSERT (memcmp(ssm_sbuffer_data(&b.buf), "ABCDEFGHI", 9) == 0);

    CU_ASSERT (ssm_sbuffer_import(&b.buf, "abcdefghij", 10) == SSM_OK);
    CU_ASSERT (ssm_sbuffer_length(&b.buf) == 10);
    CU_ASSERT (ssm_sbuffer_max_length(&b.buf) == 10);
    CU_ASSERT (memcmp(ssm_sbuffer_data(&b.buf), "abcdefghij", 10) == 0);

    CU_ASSERT (ssm_sbuffer_import(&b.buf, "ABCDEFGHIJK", 11) == SSM_TRUNCATED);
    CU_ASSERT (ssm_sbuffer_length(&b.buf) == 10);
    CU_ASSERT (ssm_sbuffer_max_length(&b.buf) == 10);
    CU_ASSERT (memcmp(ssm_sbuffer_data(&b.buf), "ABCDEFGHIJ", 10) == 0);

    CU_ASSERT (ssm_sbuffer_import(&b.buf, "abcdefghijklmnop", 16) == SSM_TRUNCATED);
    CU_ASSERT (ssm_sbuffer_length(&b.buf) == 10);
    CU_ASSERT (ssm_sbuffer_max_length(&b.buf) == 10);
    CU_ASSERT (memcmp(ssm_sbuffer_data(&b.buf), "abcdefghij", 10) == 0);
}

static void test_resize(void)
{
    ssm_sbuffer_declare (b, 10);

    CU_ASSERT (ssm_sbuffer_import(&b.buf, "abcdefgh", 8) == SSM_OK);
    CU_ASSERT (ssm_sbuffer_length(&b.buf) == 8);
    CU_ASSERT (ssm_sbuffer_max_length(&b.buf) == 10);
    CU_ASSERT (memcmp(ssm_sbuffer_data(&b.buf), "abcdefgh", 8) == 0);

    CU_ASSERT (ssm_sbuffer_resize(&b.buf, SSM_SIZE_MAX + 1) == SSM_SIZETOOLARGE);
    CU_ASSERT (ssm_sbuffer_length(&b.buf) == 8);
    CU_ASSERT (ssm_sbuffer_max_length(&b.buf) == 10);
    CU_ASSERT (memcmp(ssm_sbuffer_data(&b.buf), "abcdefgh", 8) == 0);

    CU_ASSERT (ssm_sbuffer_resize(&b.buf, 4) == SSM_OK);
    CU_ASSERT (ssm_sbuffer_length(&b.buf) == 4);
    CU_ASSERT (ssm_sbuffer_max_length(&b.buf) == 10);
    /* also check the presence of the trailing zero */
    CU_ASSERT (memcmp(ssm_sbuffer_data(&b.buf), "abcd", 5) == 0);

    CU_ASSERT (ssm_sbuffer_resize(&b.buf, 11) == SSM_TRUNCATED);
    CU_ASSERT (ssm_sbuffer_length(&b.buf) == 10);
    CU_ASSERT (ssm_sbuffer_max_length(&b.buf) == 10);
    /* also check the presence of the trailing zero */
    CU_ASSERT (memcmp(ssm_sbuffer_data(&b.buf), "abcd", 5) == 0);
}

static void test_set(void)
{
    ssm_sbuffer_declare (b, 10);

    CU_ASSERT (ssm_sbuffer_set(&b.buf, 'x') == SSM_OK);
    CU_ASSERT (ssm_sbuffer_length(&b.buf) == 0);
    CU_ASSERT (ssm_sbuffer_max_length(&b.buf) == 10);

    CU_ASSERT (ssm_sbuffer_import(&b.buf, "abcdefgh", 8) == SSM_OK);
    CU_ASSERT (ssm_sbuffer_length(&b.buf) == 8);
    CU_ASSERT (ssm_sbuffer_max_length(&b.buf) == 10);
    CU_ASSERT (memcmp(ssm_sbuffer_data(&b.buf), "abcdefgh", 8) == 0);

    CU_ASSERT (ssm_sbuffer_set(&b.buf, 'x') == SSM_OK);
    CU_ASSERT (ssm_sbuffer_length(&b.buf) == 8);
    CU_ASSERT (ssm_sbuffer_max_length(&b.buf) == 10);
    /* also check the presence of the trailing zero */
    CU_ASSERT (memcmp(ssm_sbuffer_data(&b.buf), "xxxxxxxx", 9) == 0);
}

static void test_set_range(void)
{
    ssm_sbuffer_declare (b, 10);

    CU_ASSERT (ssm_sbuffer_set_range(&b.buf, 0, 0, 'x') == SSM_OK);
    CU_ASSERT (ssm_sbuffer_length(&b.buf) == 0);
    CU_ASSERT (ssm_sbuffer_max_length(&b.buf) == 10);

    CU_ASSERT (ssm_sbuffer_set_range(&b.buf, 10, 10, 'x') == SSM_TRUNCATED);
    CU_ASSERT (ssm_sbuffer_length(&b.buf) == 0);
    CU_ASSERT (ssm_sbuffer_max_length(&b.buf) == 10);

    CU_ASSERT (ssm_sbuffer_set_range(&b.buf, 0, 10, 'x') == SSM_TRUNCATED);
    CU_ASSERT (ssm_sbuffer_length(&b.buf) == 0);
    CU_ASSERT (ssm_sbuffer_max_length(&b.buf) == 10);

    CU_ASSERT (ssm_sbuffer_import(&b.buf, "abcdefgh", 8) == SSM_OK);
    CU_ASSERT (ssm_sbuffer_length(&b.buf) == 8);
    CU_ASSERT (ssm_sbuffer_max_length(&b.buf) == 10);
    CU_ASSERT (memcmp(ssm_sbuffer_data(&b.buf), "abcdefgh", 8) == 0);

    CU_ASSERT (ssm_sbuffer_set_range(&b.buf, 2, 3, 'x') == SSM_OK);
    CU_ASSERT (ssm_sbuffer_length(&b.buf) == 8);
    CU_ASSERT (ssm_sbuffer_max_length(&b.buf) == 10);
    /* also check the presence of the trailing zero */
    CU_ASSERT (memcmp(ssm_sbuffer_data(&b.buf), "abxxxfgh", 9) == 0);

    CU_ASSERT (ssm_sbuffer_set_range(&b.buf, 6, 1, 'y') == SSM_OK);
    CU_ASSERT (ssm_sbuffer_length(&b.buf) == 8);
    CU_ASSERT (ssm_sbuffer_max_length(&b.buf) == 10);
    /* also check the presence of the trailing zero */
    CU_ASSERT (memcmp(ssm_sbuffer_data(&b.buf), "abxxxfyh", 9) == 0);

    CU_ASSERT (ssm_sbuffer_set_range(&b.buf, 4, 66, 'z') == SSM_TRUNCATED);
    CU_ASSERT (ssm_sbuffer_length(&b.buf) == 8);
    CU_ASSERT (ssm_sbuffer_max_length(&b.buf) == 10);
    /* also check the presence of the trailing zero */
    CU_ASSERT (memcmp(ssm_sbuffer_data(&b.buf), "abxxzzzz", 9) == 0);

    CU_ASSERT (ssm_sbuffer_set_range(&b.buf, SSM_SIZE_MAX + 1, 2, 12) == SSM_SIZETOOLARGE);
    CU_ASSERT (ssm_sbuffer_length(&b.buf) == 8);
    CU_ASSERT (ssm_sbuffer_max_length(&b.buf) == 10);
    /* also check the presence of the trailing zero */
    CU_ASSERT (memcmp(ssm_sbuffer_data(&b.buf), "abxxzzzz", 9) == 0);

    CU_ASSERT (ssm_sbuffer_set_range(&b.buf, 1, SSM_SIZE_MAX + 1, 12) == SSM_SIZETOOLARGE);
    CU_ASSERT (ssm_sbuffer_length(&b.buf) == 8);
    CU_ASSERT (ssm_sbuffer_max_length(&b.buf) == 10);
    /* also check the presence of the trailing zero */
    CU_ASSERT (memcmp(ssm_sbuffer_data(&b.buf), "abxxzzzz", 9) == 0);

    CU_ASSERT (ssm_sbuffer_set_range(&b.buf, 0, 7, 'p') == SSM_OK);
    CU_ASSERT (ssm_sbuffer_length(&b.buf) == 8);
    CU_ASSERT (ssm_sbuffer_max_length(&b.buf) == 10);
    /* also check the presence of the trailing zero */
    CU_ASSERT (memcmp(ssm_sbuffer_data(&b.buf), "pppppppz", 9) == 0);

    CU_ASSERT (ssm_sbuffer_set_range(&b.buf, 8, 0, 's') == SSM_OK);
    CU_ASSERT (ssm_sbuffer_length(&b.buf) == 8);
    CU_ASSERT (ssm_sbuffer_max_length(&b.buf) == 10);
    /* also check the presence of the trailing zero */
    CU_ASSERT (memcmp(ssm_sbuffer_data(&b.buf), "pppppppz", 9) == 0);

    CU_ASSERT (ssm_sbuffer_set_range(&b.buf, 9, 0, 's') == SSM_TRUNCATED);
    CU_ASSERT (ssm_sbuffer_length(&b.buf) == 8);
    CU_ASSERT (ssm_sbuffer_max_length(&b.buf) == 10);
    /* also check the presence of the trailing zero */
    CU_ASSERT (memcmp(ssm_sbuffer_data(&b.buf), "pppppppz", 9) == 0);

    CU_ASSERT (ssm_sbuffer_set_range(&b.buf, 8, 1, 's') == SSM_TRUNCATED);
    CU_ASSERT (ssm_sbuffer_length(&b.buf) == 8);
    CU_ASSERT (ssm_sbuffer_max_length(&b.buf) == 10);
    /* also check the presence of the trailing zero */
    CU_ASSERT (memcmp(ssm_sbuffer_data(&b.buf), "pppppppz", 9) == 0);
}

static void test_copy (void)
{
    ssm_sbuffer_declare (b1, 5);
    ssm_sbuffer_declare (b2, 10);

    CU_ASSERT (ssm_sbuffer_import(&b1.buf, "abcd", 4) == SSM_OK);
    CU_ASSERT (ssm_sbuffer_length(&b1.buf) == 4);
    CU_ASSERT (ssm_sbuffer_max_length(&b1.buf) == 5);
    CU_ASSERT (memcmp(ssm_sbuffer_data(&b1.buf), "abcd", 4) == 0);

    CU_ASSERT (ssm_sbuffer_copy(&b1.buf, NULL) == SSM_OK);
    CU_ASSERT (ssm_sbuffer_length(&b1.buf) == 0);
    CU_ASSERT (ssm_sbuffer_max_length(&b1.buf) == 5);

    CU_ASSERT (ssm_sbuffer_import(&b1.buf, "abcd", 4) == SSM_OK);
    CU_ASSERT (ssm_sbuffer_length(&b1.buf) == 4);
    CU_ASSERT (ssm_sbuffer_max_length(&b1.buf) == 5);
    CU_ASSERT (memcmp(ssm_sbuffer_data(&b1.buf), "abcd", 4) == 0);

    CU_ASSERT (ssm_sbuffer_import(&b2.buf, "ef", 2) == SSM_OK);
    CU_ASSERT (ssm_sbuffer_length(&b2.buf) == 2);
    CU_ASSERT (ssm_sbuffer_max_length(&b2.buf) == 10);
    CU_ASSERT (memcmp(ssm_sbuffer_data(&b2.buf), "ef", 2) == 0);

    CU_ASSERT (ssm_sbuffer_copy(&b1.buf, &b2.buf) == SSM_OK);
    CU_ASSERT (ssm_sbuffer_length(&b1.buf) == 2);
    CU_ASSERT (ssm_sbuffer_max_length(&b1.buf) == 5);
    CU_ASSERT (ssm_sbuffer_max_length(&b2.buf) == 10);
    CU_ASSERT (memcmp(ssm_sbuffer_data(&b1.buf), "ef", 2) == 0);

    CU_ASSERT (ssm_sbuffer_import(&b2.buf, "ghijklmn", 8) == SSM_OK);
    CU_ASSERT (ssm_sbuffer_length(&b2.buf) == 8);
    CU_ASSERT (ssm_sbuffer_max_length(&b1.buf) == 5);
    CU_ASSERT (ssm_sbuffer_max_length(&b2.buf) == 10);
    CU_ASSERT (memcmp(ssm_sbuffer_data(&b2.buf), "ghijklmn", 8) == 0);

    CU_ASSERT (ssm_sbuffer_copy(&b1.buf, &b2.buf) == SSM_TRUNCATED);
    CU_ASSERT (ssm_sbuffer_length(&b1.buf) == 5);
    CU_ASSERT (ssm_sbuffer_max_length(&b1.buf) == 5);
    CU_ASSERT (ssm_sbuffer_max_length(&b2.buf) == 10);
    CU_ASSERT (memcmp(ssm_sbuffer_data(&b1.buf), "ghijk", 5) == 0);
}

static void test_copy_overlap (void)
{
    ssm_sbuffer_declare (b, 50);

    CU_ASSERT (ssm_sbuffer_length(&b.buf) == 0);
    CU_ASSERT (ssm_sbuffer_max_length(&b.buf) == 50);

    CU_ASSERT (ssm_sbuffer_copy(&b.buf, &b.buf) == SSM_OK);
    CU_ASSERT (ssm_sbuffer_length(&b.buf) == 0);

    CU_ASSERT (ssm_sbuffer_import(&b.buf, "abcd", 4) == SSM_OK);
    CU_ASSERT (ssm_sbuffer_length(&b.buf) == 4);
    CU_ASSERT (memcmp(ssm_sbuffer_data(&b.buf), "abcd", 4) == 0);

    CU_ASSERT (ssm_sbuffer_copy(&b.buf, &b.buf) == SSM_OK);
    CU_ASSERT (ssm_sbuffer_length(&b.buf) == 4);
    CU_ASSERT (memcmp(ssm_sbuffer_data(&b.buf), "abcd", 4) == 0);
}

static void test_concat (void)
{
    ssm_sbuffer_declare (b1, 10);
    ssm_sbuffer_declare (b2, 10);

    CU_ASSERT (ssm_sbuffer_import(&b1.buf, "abcd", 4) == SSM_OK);
    CU_ASSERT (ssm_sbuffer_length(&b1.buf) == 4);
    CU_ASSERT (ssm_sbuffer_max_length(&b1.buf) == 10);
    CU_ASSERT (memcmp(ssm_sbuffer_data(&b1.buf), "abcd", 4) == 0);

    CU_ASSERT (ssm_sbuffer_concat(&b1.buf, NULL) == SSM_OK);
    CU_ASSERT (ssm_sbuffer_length(&b1.buf) == 4);
    CU_ASSERT (ssm_sbuffer_max_length(&b1.buf) == 10);
    CU_ASSERT (memcmp(ssm_sbuffer_data(&b1.buf), "abcd", 4) == 0);

    CU_ASSERT (ssm_sbuffer_import(&b2.buf, "ef", 2) == SSM_OK);
    CU_ASSERT (ssm_sbuffer_length(&b2.buf) == 2);
    CU_ASSERT (ssm_sbuffer_max_length(&b2.buf) == 10);
    CU_ASSERT (memcmp(ssm_sbuffer_data(&b2.buf), "ef", 2) == 0);

    CU_ASSERT (ssm_sbuffer_concat(&b1.buf, &b2.buf) == SSM_OK);
    CU_ASSERT (ssm_sbuffer_length(&b1.buf) == 6);
    CU_ASSERT (ssm_sbuffer_max_length(&b1.buf) == 10);
    CU_ASSERT (ssm_sbuffer_max_length(&b2.buf) == 10);
    CU_ASSERT (memcmp(ssm_sbuffer_data(&b1.buf), "abcdef", 6) == 0);

    CU_ASSERT (ssm_sbuffer_import(&b2.buf, "ghijklmn", 8) == SSM_OK);
    CU_ASSERT (ssm_sbuffer_length(&b2.buf) == 8);
    CU_ASSERT (ssm_sbuffer_max_length(&b1.buf) == 10);
    CU_ASSERT (ssm_sbuffer_max_length(&b2.buf) == 10);
    CU_ASSERT (memcmp(ssm_sbuffer_data(&b2.buf), "ghijklmn", 8) == 0);

    CU_ASSERT (ssm_sbuffer_concat(&b1.buf, &b2.buf) == SSM_TRUNCATED);
    CU_ASSERT (ssm_sbuffer_length(&b1.buf) == 10);
    CU_ASSERT (ssm_sbuffer_max_length(&b1.buf) == 10);
    CU_ASSERT (ssm_sbuffer_max_length(&b2.buf) == 10);
    CU_ASSERT (memcmp(ssm_sbuffer_data(&b1.buf), "abcdefghij", 10) == 0);
}

static void test_concat_overlap (void)
{
    ssm_sbuffer_declare (b, 50);

    CU_ASSERT (ssm_sbuffer_length(&b.buf) == 0);
    CU_ASSERT (ssm_sbuffer_max_length(&b.buf) == 50);

    CU_ASSERT (ssm_sbuffer_concat(&b.buf, &b.buf) == SSM_OK);
    CU_ASSERT (ssm_sbuffer_length(&b.buf) == 0);

    CU_ASSERT (ssm_sbuffer_import(&b.buf, "abcd", 4) == SSM_OK);
    CU_ASSERT (ssm_sbuffer_length(&b.buf) == 4);
    CU_ASSERT (memcmp(ssm_sbuffer_data(&b.buf), "abcd", 4) == 0);

    CU_ASSERT (ssm_sbuffer_concat(&b.buf, &b.buf) == SSM_OK);
    CU_ASSERT (ssm_sbuffer_length(&b.buf) == 8);
    CU_ASSERT (memcmp(ssm_sbuffer_data(&b.buf), "abcdabcd", 8) == 0);
}

static void test_compare (void)
{
    ssm_sbuffer_declare (b1, 10);
    ssm_sbuffer_declare (b2, 10);

    CU_ASSERT (ssm_sbuffer_compare(&b1.buf, NULL) == SSM_EQUAL);
    CU_ASSERT (ssm_sbuffer_compare(&b2.buf, NULL) == SSM_EQUAL);
    CU_ASSERT (ssm_sbuffer_compare(&b1.buf, &b2.buf) == SSM_EQUAL);

    CU_ASSERT (ssm_sbuffer_import(&b1.buf, "abcd", 4) == SSM_OK);
    CU_ASSERT (ssm_sbuffer_length(&b1.buf) == 4);
    CU_ASSERT (memcmp(ssm_sbuffer_data(&b1.buf), "abcd", 4) == 0);

    CU_ASSERT (ssm_sbuffer_compare(&b1.buf, NULL) == SSM_GREATER);
    CU_ASSERT (ssm_sbuffer_compare(&b1.buf, &b2.buf) == SSM_GREATER);
    CU_ASSERT (ssm_sbuffer_compare(&b2.buf, &b1.buf) == SSM_LOWER);

    CU_ASSERT (ssm_sbuffer_import(&b2.buf, "abedef", 6) == SSM_OK);
    CU_ASSERT (ssm_sbuffer_length(&b2.buf) == 6);
    CU_ASSERT (memcmp(ssm_sbuffer_data(&b2.buf), "abedef", 6) == 0);

    CU_ASSERT (ssm_sbuffer_compare(&b1.buf, &b2.buf) == SSM_LOWER);
    CU_ASSERT (ssm_sbuffer_compare(&b2.buf, &b1.buf) == SSM_GREATER);

    CU_ASSERT (ssm_sbuffer_import(&b2.buf, "abcd", 4) == SSM_OK);
    CU_ASSERT (ssm_sbuffer_length(&b2.buf) == 4);
    CU_ASSERT (memcmp(ssm_sbuffer_data(&b2.buf), "abcd", 4) == 0);

    CU_ASSERT (ssm_sbuffer_compare(&b1.buf, &b2.buf) == SSM_EQUAL);
    CU_ASSERT (ssm_sbuffer_compare(&b2.buf, &b1.buf) == SSM_EQUAL);
}

static void test_insert (void)
{
    ssm_sbuffer_declare (b, 16);

    CU_ASSERT (ssm_sbuffer_length(&b.buf) == 0);
    CU_ASSERT (ssm_sbuffer_max_length(&b.buf) == 16);

    CU_ASSERT (ssm_sbuffer_import(&b.buf, "abcdefgh", 8) == SSM_OK);
    CU_ASSERT (ssm_sbuffer_length(&b.buf) == 8);
    CU_ASSERT (memcmp(ssm_sbuffer_data(&b.buf), "abcdefgh", 8) == 0);

    CU_ASSERT (ssm_sbuffer_insert(&b.buf, 2, 3) == SSM_OK);
    CU_ASSERT (ssm_sbuffer_length(&b.buf) == 11);
    CU_ASSERT (memcmp(ssm_sbuffer_data(&b.buf), "abcdecdefgh", 11) == 0);

    CU_ASSERT (ssm_sbuffer_insert(&b.buf, 20, 3) == SSM_INDEXRANGE);
    CU_ASSERT (ssm_sbuffer_length(&b.buf) == 11);
    CU_ASSERT (memcmp(ssm_sbuffer_data(&b.buf), "abcdecdefgh", 11) == 0);

    CU_ASSERT (ssm_sbuffer_insert(&b.buf, 12, 3) == SSM_INDEXRANGE);
    CU_ASSERT (ssm_sbuffer_length(&b.buf) == 11);
    CU_ASSERT (memcmp(ssm_sbuffer_data(&b.buf), "abcdecdefgh", 11) == 0);

    CU_ASSERT (ssm_sbuffer_insert(&b.buf, 10, SSM_SIZE_MAX + 1) == SSM_SIZETOOLARGE);
    CU_ASSERT (ssm_sbuffer_length(&b.buf) == 11);
    CU_ASSERT (memcmp(ssm_sbuffer_data(&b.buf), "abcdecdefgh", 11) == 0);

    CU_ASSERT (ssm_sbuffer_insert(&b.buf, 10, 0) == SSM_OK);
    CU_ASSERT (ssm_sbuffer_length(&b.buf) == 11);
    CU_ASSERT (memcmp(ssm_sbuffer_data(&b.buf), "abcdecdefgh", 11) == 0);

    CU_ASSERT (ssm_sbuffer_insert(&b.buf, 11, 3) == SSM_OK);
    CU_ASSERT (ssm_sbuffer_length(&b.buf) == 14);
    CU_ASSERT (memcmp(ssm_sbuffer_data(&b.buf), "abcdecdefgh", 11) == 0);

    CU_ASSERT (ssm_sbuffer_insert(&b.buf, 0, 8) == SSM_TRUNCATED);
    CU_ASSERT (ssm_sbuffer_length(&b.buf) == 16);
    CU_ASSERT (memcmp(ssm_sbuffer_data(&b.buf), "abcdecdeabcdecde", 16) == 0);

    CU_ASSERT (ssm_sbuffer_import(&b.buf, "abcdefghijkl", 12) == SSM_OK);
    CU_ASSERT (ssm_sbuffer_length(&b.buf) == 12);
    CU_ASSERT (memcmp(ssm_sbuffer_data(&b.buf), "abcdefghijkl", 12) == 0);

    CU_ASSERT (ssm_sbuffer_insert(&b.buf, 10, 8) == SSM_TRUNCATED);
    CU_ASSERT (ssm_sbuffer_length(&b.buf) == 16);
    CU_ASSERT (memcmp(ssm_sbuffer_data(&b.buf), "abcdefghijkl", 12) == 0);
}

static void test_struct (void)
{
    typedef struct {
        int before;
        ssm_sbuffer_struct(50) buf;
        int after;
    } TestStruct;

    TestStruct a = {
        0x1234567,
        ssm_sbuffer_init(50),
        0x1ABCDEF
    };

    static const char ref[] = "elkfvzelvlevlkzrlbrbnrtnyortgfgernvrjvfnkvze7892Hurt";

    CU_ASSERT (a.before == 0x1234567);
    CU_ASSERT (ssm_sbuffer_max_length(&a.buf.buf) == 50);
    CU_ASSERT (ssm_sbuffer_length(&a.buf.buf) == 0);
    CU_ASSERT (a.after == 0x1ABCDEF);

    CU_ASSERT (ssm_sbuffer_import(&a.buf.buf, ref, sizeof(ref)) == SSM_TRUNCATED);

    CU_ASSERT (a.before == 0x1234567);
    CU_ASSERT (ssm_sbuffer_max_length(&a.buf.buf) == 50);
    CU_ASSERT (ssm_sbuffer_length(&a.buf.buf) == 50);
    CU_ASSERT (memcmp(ssm_sbuffer_data(&a.buf.buf), ref, 50) == 0);
    CU_ASSERT (a.after == 0x1ABCDEF);
}

static void test_trashed (void)
{
    ssm_sbuffer_declare (b1, 10);
    ssm_sbuffer_declare (b2, 10);

    CU_ASSERT (ssm_sbuffer_import(&b1.buf, "abcd", 4) == SSM_OK);
    CU_ASSERT (ssm_sbuffer_length(&b1.buf) == 4);
    CU_ASSERT (ssm_sbuffer_max_length(&b1.buf) == 10);
    CU_ASSERT (memcmp(ssm_sbuffer_data(&b1.buf), "abcd", 4) == 0);

    ((char*)(ssm_sbuffer_data(&b1.buf)))[4] = 1; /* trashing hidden null terminator */

    CU_ASSERT (ssm_sbuffer_import(&b1.buf, "xy", 1) == SSM_CORRUPTED);
    CU_ASSERT (ssm_sbuffer_copy(&b1.buf, &b2.buf) == SSM_CORRUPTED);
    CU_ASSERT (ssm_sbuffer_concat(&b1.buf, &b2.buf) == SSM_CORRUPTED);
    CU_ASSERT (ssm_sbuffer_compare(&b1.buf, &b2.buf) == SSM_CORRUPTED);
    CU_ASSERT (ssm_sbuffer_resize(&b1.buf, 1) == SSM_CORRUPTED);
    CU_ASSERT (ssm_sbuffer_set(&b1.buf, 99) == SSM_CORRUPTED);
    CU_ASSERT (ssm_sbuffer_set_range(&b1.buf, 1, 2, 99) == SSM_CORRUPTED);
    CU_ASSERT (ssm_sbuffer_insert(&b1.buf, 0, 2) == SSM_CORRUPTED);
}

/*
 * Add the tests in this module in a specified test suite.
 */
static void ssm_sbuffer_test_add_tests(CU_pSuite suite)
{
    CU_ADD_TEST (suite, test_null);
    CU_ADD_TEST (suite, test_zero);
    CU_ADD_TEST (suite, test_import);
    CU_ADD_TEST (suite, test_resize);
    CU_ADD_TEST (suite, test_set);
    CU_ADD_TEST (suite, test_set_range);
    CU_ADD_TEST (suite, test_copy);
    CU_ADD_TEST (suite, test_copy_overlap);
    CU_ADD_TEST (suite, test_concat);
    CU_ADD_TEST (suite, test_concat_overlap);
    CU_ADD_TEST (suite, test_compare);
    CU_ADD_TEST (suite, test_insert);
    CU_ADD_TEST (suite, test_struct);
    CU_ADD_TEST (suite, test_trashed);
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
#if !defined(SSM_USE_CANARY)
void ssm_sbuffer_test_init(void)
{
    const CU_pSuite suite = CU_add_suite ("sbuffer", init_suite, cleanup_suite);
    ssm_sbuffer_test_add_tests(suite);
}
#endif /* SSM_USE_CANARY */

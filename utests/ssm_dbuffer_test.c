
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
 *  CUnit test suite for ssm_dbuffer_t
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

#include "ssm_dbuffer_test.h"
#include "ssm.h"
#include "utests.h"
#include <string.h>

/*
 * Test cases
 */
static void test_null (void)
{
    CU_ASSERT (ssm_dbuffer_length(NULL) == 0);
    CU_ASSERT (ssm_dbuffer_data(NULL) != NULL);
    CU_ASSERT (ssm_dbuffer_import(NULL, "", 0) == SSM_NULLOUT);
    CU_ASSERT (ssm_dbuffer_import(NULL, NULL, 0) == SSM_NULLOUT);
    CU_ASSERT (ssm_dbuffer_resize(NULL, 100) == SSM_NULLOUT);
    CU_ASSERT (ssm_dbuffer_set(NULL, 0) == SSM_NULLOUT);
    CU_ASSERT (ssm_dbuffer_set_range(NULL, 0, 0, 0) == SSM_NULLOUT);
    CU_ASSERT (ssm_dbuffer_copy(NULL, NULL) == SSM_NULLOUT);
    CU_ASSERT (ssm_dbuffer_concat(NULL, NULL) == SSM_NULLOUT);
    CU_ASSERT (ssm_dbuffer_compare(NULL, NULL) == SSM_EQUAL);
    CU_ASSERT (ssm_dbuffer_free(NULL) == SSM_OK);
    CU_ASSERT (ssm_dbuffer_insert(NULL, 0, 10) == SSM_NULLOUT);
}

static void test_zero (void)
{
    ssm_dbuffer_declare (b);

    CU_ASSERT (ssm_dbuffer_length(&b) == 0);
    CU_ASSERT (ssm_dbuffer_free(&b) == SSM_OK);
    CU_ASSERT (ssm_dbuffer_length(&b) == 0);
}

static void test_import (void)
{
    ssm_dbuffer_declare (b);

    CU_ASSERT (ssm_dbuffer_length(&b) == 0);

    CU_ASSERT (ssm_dbuffer_import(&b, "", SSM_SIZE_MAX + 1) == SSM_SIZETOOLARGE);
    CU_ASSERT (ssm_dbuffer_length(&b) == 0);

    CU_ASSERT (ssm_dbuffer_import(&b, "abcd", 4) == SSM_OK);
    CU_ASSERT (ssm_dbuffer_length(&b) == 4);
    CU_ASSERT (memcmp(ssm_dbuffer_data(&b), "abcd", 4) == 0);

    CU_ASSERT (ssm_dbuffer_import(&b, NULL, 0) == SSM_OK);
    CU_ASSERT (ssm_dbuffer_length(&b) == 0);

    CU_ASSERT (ssm_dbuffer_import(&b, "ABCDEFGHIJKLMNOP", 16) == SSM_OK);
    CU_ASSERT (ssm_dbuffer_length(&b) == 16);
    CU_ASSERT (memcmp(ssm_dbuffer_data(&b), "ABCDEFGHIJKLMNOP", 16) == 0);

    CU_ASSERT (ssm_dbuffer_import(&b, NULL, 100) == SSM_OK);
    CU_ASSERT (ssm_dbuffer_length(&b) == 0);

    CU_ASSERT (ssm_dbuffer_import(&b, "abcd", 4) == SSM_OK);
    CU_ASSERT (ssm_dbuffer_length(&b) == 4);
    CU_ASSERT (memcmp(ssm_dbuffer_data(&b), "abcd", 4) == 0);

    CU_ASSERT (ssm_dbuffer_free(&b) == SSM_OK);
    CU_ASSERT (ssm_dbuffer_length(&b) == 0);
}

static void test_resize(void)
{
    ssm_dbuffer_declare (b);

    CU_ASSERT (ssm_dbuffer_import(&b, "abcdefgh", 8) == SSM_OK);
    CU_ASSERT (ssm_dbuffer_length(&b) == 8);
    CU_ASSERT (memcmp(ssm_dbuffer_data(&b), "abcdefgh", 8) == 0);

    CU_ASSERT (ssm_dbuffer_resize(&b, SSM_SIZE_MAX + 1) == SSM_SIZETOOLARGE);
    CU_ASSERT (ssm_dbuffer_length(&b) == 8);
    CU_ASSERT (memcmp(ssm_dbuffer_data(&b), "abcdefgh", 8) == 0);

    CU_ASSERT (ssm_dbuffer_resize(&b, 4) == SSM_OK);
    CU_ASSERT (ssm_dbuffer_length(&b) == 4);
    /* also check the presence of the trailing zero */
    CU_ASSERT (memcmp(ssm_dbuffer_data(&b), "abcd", 5) == 0);

    CU_ASSERT (ssm_dbuffer_resize(&b, 1024) == SSM_OK);
    CU_ASSERT (ssm_dbuffer_length(&b) == 1024);
    /* also check the presence of the trailing zero */
    CU_ASSERT (memcmp(ssm_dbuffer_data(&b), "abcd", 5) == 0);

    CU_ASSERT (ssm_dbuffer_free(&b) == SSM_OK);
    CU_ASSERT (ssm_dbuffer_length(&b) == 0);
}

static void test_set(void)
{
    ssm_dbuffer_declare (b);

    CU_ASSERT (ssm_dbuffer_set(&b, 'x') == SSM_OK);
    CU_ASSERT (ssm_dbuffer_length(&b) == 0);

    CU_ASSERT (ssm_dbuffer_import(&b, "abcdefgh", 8) == SSM_OK);
    CU_ASSERT (ssm_dbuffer_length(&b) == 8);
    CU_ASSERT (memcmp(ssm_dbuffer_data(&b), "abcdefgh", 8) == 0);

    CU_ASSERT (ssm_dbuffer_set(&b, 'x') == SSM_OK);
    CU_ASSERT (ssm_dbuffer_length(&b) == 8);
    /* also check the presence of the trailing zero */
    CU_ASSERT (memcmp(ssm_dbuffer_data(&b), "xxxxxxxx", 9) == 0);

    CU_ASSERT (ssm_dbuffer_free(&b) == SSM_OK);
    CU_ASSERT (ssm_dbuffer_length(&b) == 0);
}

static void test_set_range(void)
{
    ssm_dbuffer_declare (b);

    CU_ASSERT (ssm_dbuffer_set_range(&b, 0, 0, 'x') == SSM_OK);
    CU_ASSERT (ssm_dbuffer_length(&b) == 0);

    CU_ASSERT (ssm_dbuffer_set_range(&b, 10, 10, 'x') == SSM_TRUNCATED);
    CU_ASSERT (ssm_dbuffer_length(&b) == 0);

    CU_ASSERT (ssm_dbuffer_set_range(&b, 0, 10, 'x') == SSM_TRUNCATED);
    CU_ASSERT (ssm_dbuffer_length(&b) == 0);

    CU_ASSERT (ssm_dbuffer_import(&b, "abcdefgh", 8) == SSM_OK);
    CU_ASSERT (ssm_dbuffer_length(&b) == 8);
    CU_ASSERT (memcmp(ssm_dbuffer_data(&b), "abcdefgh", 8) == 0);

    CU_ASSERT (ssm_dbuffer_set_range(&b, 2, 3, 'x') == SSM_OK);
    CU_ASSERT (ssm_dbuffer_length(&b) == 8);
    /* also check the presence of the trailing zero */
    CU_ASSERT (memcmp(ssm_dbuffer_data(&b), "abxxxfgh", 9) == 0);

    CU_ASSERT (ssm_dbuffer_set_range(&b, 6, 1, 'y') == SSM_OK);
    CU_ASSERT (ssm_dbuffer_length(&b) == 8);
    /* also check the presence of the trailing zero */
    CU_ASSERT (memcmp(ssm_dbuffer_data(&b), "abxxxfyh", 9) == 0);

    CU_ASSERT (ssm_dbuffer_set_range(&b, 4, 66, 'z') == SSM_TRUNCATED);
    CU_ASSERT (ssm_dbuffer_length(&b) == 8);
    /* also check the presence of the trailing zero */
    CU_ASSERT (memcmp(ssm_dbuffer_data(&b), "abxxzzzz", 9) == 0);

    CU_ASSERT (ssm_dbuffer_set_range(&b, SSM_SIZE_MAX + 1, 2, 12) == SSM_SIZETOOLARGE);
    CU_ASSERT (ssm_dbuffer_length(&b) == 8);
    /* also check the presence of the trailing zero */
    CU_ASSERT (memcmp(ssm_dbuffer_data(&b), "abxxzzzz", 9) == 0);

    CU_ASSERT (ssm_dbuffer_set_range(&b, 1, SSM_SIZE_MAX + 1, 12) == SSM_SIZETOOLARGE);
    CU_ASSERT (ssm_dbuffer_length(&b) == 8);
    /* also check the presence of the trailing zero */
    CU_ASSERT (memcmp(ssm_dbuffer_data(&b), "abxxzzzz", 9) == 0);

    CU_ASSERT (ssm_dbuffer_set_range(&b, 0, 7, 'p') == SSM_OK);
    CU_ASSERT (ssm_dbuffer_length(&b) == 8);
    /* also check the presence of the trailing zero */
    CU_ASSERT (memcmp(ssm_dbuffer_data(&b), "pppppppz", 9) == 0);

    CU_ASSERT (ssm_dbuffer_set_range(&b, 8, 0, 's') == SSM_OK);
    CU_ASSERT (ssm_dbuffer_length(&b) == 8);
    /* also check the presence of the trailing zero */
    CU_ASSERT (memcmp(ssm_dbuffer_data(&b), "pppppppz", 9) == 0);

    CU_ASSERT (ssm_dbuffer_set_range(&b, 9, 0, 's') == SSM_TRUNCATED);
    CU_ASSERT (ssm_dbuffer_length(&b) == 8);
    /* also check the presence of the trailing zero */
    CU_ASSERT (memcmp(ssm_dbuffer_data(&b), "pppppppz", 9) == 0);

    CU_ASSERT (ssm_dbuffer_set_range(&b, 8, 1, 's') == SSM_TRUNCATED);
    CU_ASSERT (ssm_dbuffer_length(&b) == 8);
    /* also check the presence of the trailing zero */
    CU_ASSERT (memcmp(ssm_dbuffer_data(&b), "pppppppz", 9) == 0);

    CU_ASSERT (ssm_dbuffer_free(&b) == SSM_OK);
    CU_ASSERT (ssm_dbuffer_length(&b) == 0);
}

static void test_copy (void)
{
    ssm_dbuffer_declare (b1);
    ssm_dbuffer_declare (b2);

    CU_ASSERT (ssm_dbuffer_import(&b1, "abcd", 4) == SSM_OK);
    CU_ASSERT (ssm_dbuffer_length(&b1) == 4);
    CU_ASSERT (memcmp(ssm_dbuffer_data(&b1), "abcd", 4) == 0);

    CU_ASSERT (ssm_dbuffer_copy(&b1, NULL) == SSM_OK);
    CU_ASSERT (ssm_dbuffer_length(&b1) == 0);

    CU_ASSERT (ssm_dbuffer_import(&b2, "ef", 2) == SSM_OK);
    CU_ASSERT (ssm_dbuffer_length(&b2) == 2);
    CU_ASSERT (memcmp(ssm_dbuffer_data(&b2), "ef", 2) == 0);

    CU_ASSERT (ssm_dbuffer_copy(&b1, &b2) == SSM_OK);
    CU_ASSERT (ssm_dbuffer_length(&b1) == 2);
    CU_ASSERT (memcmp(ssm_dbuffer_data(&b1), "ef", 2) == 0);

    CU_ASSERT (ssm_dbuffer_import(&b2, "ghijklmn", 8) == SSM_OK);
    CU_ASSERT (ssm_dbuffer_length(&b2) == 8);
    CU_ASSERT (memcmp(ssm_dbuffer_data(&b2), "ghijklmn", 8) == 0);

    CU_ASSERT (ssm_dbuffer_copy(&b1, &b2) == SSM_OK);
    CU_ASSERT (ssm_dbuffer_length(&b1) == 8);
    CU_ASSERT (memcmp(ssm_dbuffer_data(&b1), "ghijklmn", 8) == 0);

    CU_ASSERT (ssm_dbuffer_free(&b1) == SSM_OK);
    CU_ASSERT (ssm_dbuffer_length(&b1) == 0);

    CU_ASSERT (ssm_dbuffer_free(&b2) == SSM_OK);
    CU_ASSERT (ssm_dbuffer_length(&b2) == 0);
}

static void test_copy_overlap (void)
{
    ssm_dbuffer_declare (b);

    CU_ASSERT (ssm_dbuffer_length(&b) == 0);

    CU_ASSERT (ssm_dbuffer_copy(&b, &b) == SSM_OK);
    CU_ASSERT (ssm_dbuffer_length(&b) == 0);

    CU_ASSERT (ssm_dbuffer_import(&b, "abcd", 4) == SSM_OK);
    CU_ASSERT (ssm_dbuffer_length(&b) == 4);
    CU_ASSERT (memcmp(ssm_dbuffer_data(&b), "abcd", 4) == 0);

    CU_ASSERT (ssm_dbuffer_copy(&b, &b) == SSM_OK);
    CU_ASSERT (ssm_dbuffer_length(&b) == 4);
    CU_ASSERT (memcmp(ssm_dbuffer_data(&b), "abcd", 4) == 0);

    CU_ASSERT (ssm_dbuffer_free(&b) == SSM_OK);
    CU_ASSERT (ssm_dbuffer_length(&b) == 0);
}

static void test_concat (void)
{
    ssm_dbuffer_declare (b1);
    ssm_dbuffer_declare (b2);

    CU_ASSERT (ssm_dbuffer_import(&b1, "abcd", 4) == SSM_OK);
    CU_ASSERT (ssm_dbuffer_length(&b1) == 4);
    CU_ASSERT (memcmp(ssm_dbuffer_data(&b1), "abcd", 4) == 0);

    CU_ASSERT (ssm_dbuffer_concat(&b1, NULL) == SSM_OK);
    CU_ASSERT (ssm_dbuffer_length(&b1) == 4);
    CU_ASSERT (memcmp(ssm_dbuffer_data(&b1), "abcd", 4) == 0);

    CU_ASSERT (ssm_dbuffer_import(&b2, "ef", 2) == SSM_OK);
    CU_ASSERT (ssm_dbuffer_length(&b2) == 2);
    CU_ASSERT (memcmp(ssm_dbuffer_data(&b2), "ef", 2) == 0);

    CU_ASSERT (ssm_dbuffer_concat(&b1, &b2) == SSM_OK);
    CU_ASSERT (ssm_dbuffer_length(&b1) == 6);
    CU_ASSERT (memcmp(ssm_dbuffer_data(&b1), "abcdef", 6) == 0);

    CU_ASSERT (ssm_dbuffer_import(&b2, "ghijklmn", 8) == SSM_OK);
    CU_ASSERT (ssm_dbuffer_length(&b2) == 8);
    CU_ASSERT (memcmp(ssm_dbuffer_data(&b2), "ghijklmn", 8) == 0);

    CU_ASSERT (ssm_dbuffer_concat(&b1, &b2) == SSM_OK);
    CU_ASSERT (ssm_dbuffer_length(&b1) == 14);
    CU_ASSERT (memcmp(ssm_dbuffer_data(&b1), "abcdefghijklmn", 14) == 0);

    CU_ASSERT (ssm_dbuffer_free(&b1) == SSM_OK);
    CU_ASSERT (ssm_dbuffer_length(&b1) == 0);

    CU_ASSERT (ssm_dbuffer_free(&b2) == SSM_OK);
    CU_ASSERT (ssm_dbuffer_length(&b2) == 0);
}

static void test_concat_overlap (void)
{
    ssm_dbuffer_declare (b);

    CU_ASSERT (ssm_dbuffer_length(&b) == 0);

    CU_ASSERT (ssm_dbuffer_concat(&b, &b) == SSM_OK);
    CU_ASSERT (ssm_dbuffer_length(&b) == 0);

    CU_ASSERT (ssm_dbuffer_import(&b, "abcd", 4) == SSM_OK);
    CU_ASSERT (ssm_dbuffer_length(&b) == 4);
    CU_ASSERT (memcmp(ssm_dbuffer_data(&b), "abcd", 4) == 0);

    CU_ASSERT (ssm_dbuffer_concat(&b, &b) == SSM_OK);
    CU_ASSERT (ssm_dbuffer_length(&b) == 8);
    CU_ASSERT (memcmp(ssm_dbuffer_data(&b), "abcdabcd", 8) == 0);

    CU_ASSERT (ssm_dbuffer_free(&b) == SSM_OK);
    CU_ASSERT (ssm_dbuffer_length(&b) == 0);
}

static void test_compare (void)
{
    ssm_dbuffer_declare (b1);
    ssm_dbuffer_declare (b2);

    CU_ASSERT (ssm_dbuffer_compare(&b1, NULL) == SSM_EQUAL);
    CU_ASSERT (ssm_dbuffer_compare(&b2, NULL) == SSM_EQUAL);
    CU_ASSERT (ssm_dbuffer_compare(&b1, &b2) == SSM_EQUAL);

    CU_ASSERT (ssm_dbuffer_import(&b1, "abcd", 4) == SSM_OK);
    CU_ASSERT (ssm_dbuffer_length(&b1) == 4);
    CU_ASSERT (memcmp(ssm_dbuffer_data(&b1), "abcd", 4) == 0);

    CU_ASSERT (ssm_dbuffer_compare(&b1, NULL) == SSM_GREATER);
    CU_ASSERT (ssm_dbuffer_compare(&b1, &b2) == SSM_GREATER);
    CU_ASSERT (ssm_dbuffer_compare(&b2, &b1) == SSM_LOWER);

    CU_ASSERT (ssm_dbuffer_import(&b2, "abedef", 6) == SSM_OK);
    CU_ASSERT (ssm_dbuffer_length(&b2) == 6);
    CU_ASSERT (memcmp(ssm_dbuffer_data(&b2), "abedef", 6) == 0);

    CU_ASSERT (ssm_dbuffer_compare(&b1, &b2) == SSM_LOWER);
    CU_ASSERT (ssm_dbuffer_compare(&b2, &b1) == SSM_GREATER);

    CU_ASSERT (ssm_dbuffer_import(&b2, "abcd", 4) == SSM_OK);
    CU_ASSERT (ssm_dbuffer_length(&b2) == 4);
    CU_ASSERT (memcmp(ssm_dbuffer_data(&b2), "abcd", 4) == 0);

    CU_ASSERT (ssm_dbuffer_compare(&b1, &b2) == SSM_EQUAL);
    CU_ASSERT (ssm_dbuffer_compare(&b2, &b1) == SSM_EQUAL);

    CU_ASSERT (ssm_dbuffer_free(&b1) == SSM_OK);
    CU_ASSERT (ssm_dbuffer_length(&b1) == 0);

    CU_ASSERT (ssm_dbuffer_free(&b2) == SSM_OK);
    CU_ASSERT (ssm_dbuffer_length(&b2) == 0);
}

static void test_insert (void)
{
    ssm_dbuffer_declare (b);

    CU_ASSERT (ssm_dbuffer_length(&b) == 0);

    CU_ASSERT (ssm_dbuffer_import(&b, "abcdefgh", 8) == SSM_OK);
    CU_ASSERT (ssm_dbuffer_length(&b) == 8);
    CU_ASSERT (memcmp(ssm_dbuffer_data(&b), "abcdefgh", 8) == 0);

    CU_ASSERT (ssm_dbuffer_insert(&b, 2, 3) == SSM_OK);
    CU_ASSERT (ssm_dbuffer_length(&b) == 11);
    CU_ASSERT (memcmp(ssm_dbuffer_data(&b), "abcdecdefgh", 11) == 0);

    CU_ASSERT (ssm_dbuffer_insert(&b, 10, 0) == SSM_OK);
    CU_ASSERT (ssm_dbuffer_length(&b) == 11);
    CU_ASSERT (memcmp(ssm_dbuffer_data(&b), "abcdecdefgh", 11) == 0);

    CU_ASSERT (ssm_dbuffer_insert(&b, 10, SSM_SIZE_MAX + 1) == SSM_SIZETOOLARGE);
    CU_ASSERT (ssm_dbuffer_length(&b) == 11);
    CU_ASSERT (memcmp(ssm_dbuffer_data(&b), "abcdecdefgh", 11) == 0);

    CU_ASSERT (ssm_dbuffer_insert(&b, 20, 3) == SSM_INDEXRANGE);
    CU_ASSERT (ssm_dbuffer_length(&b) == 11);
    CU_ASSERT (memcmp(ssm_dbuffer_data(&b), "abcdecdefgh", 11) == 0);

    CU_ASSERT (ssm_dbuffer_insert(&b, 12, 3) == SSM_INDEXRANGE);
    CU_ASSERT (ssm_dbuffer_length(&b) == 11);
    CU_ASSERT (memcmp(ssm_dbuffer_data(&b), "abcdecdefgh", 11) == 0);

    CU_ASSERT (ssm_dbuffer_insert(&b, 0, 8) == SSM_OK);
    CU_ASSERT (ssm_dbuffer_length(&b) == 19);
    CU_ASSERT (memcmp(ssm_dbuffer_data(&b), "abcdecdeabcdecdefgh", 19) == 0);

    CU_ASSERT (ssm_dbuffer_free(&b) == SSM_OK);
    CU_ASSERT (ssm_dbuffer_length(&b) == 0);
}

static void test_struct (void)
{
    typedef struct {
        int before;
        ssm_dbuffer_t buf;
        int after;
    } TestStruct;

    TestStruct a = {
        0x1234567,
        ssm_dbuffer_init,
        0x1ABCDEF
    };

    static const char ref[] = "elkfvzelvlevlkzrlbrbnrtnyortgfgernvrjvfnkvze7892Hurt";

    CU_ASSERT (a.before == 0x1234567);
    CU_ASSERT (ssm_dbuffer_length(&a.buf) == 0);
    CU_ASSERT (a.after == 0x1ABCDEF);

    CU_ASSERT (ssm_dbuffer_import(&a.buf, ref, 52) == SSM_OK);

    CU_ASSERT (a.before == 0x1234567);
    CU_ASSERT (ssm_dbuffer_length(&a.buf) == 52);
    CU_ASSERT (memcmp(ssm_dbuffer_data(&a.buf), ref, 52) == 0);
    CU_ASSERT (a.after == 0x1ABCDEF);

    CU_ASSERT (ssm_dbuffer_free(&a.buf) == SSM_OK);
    CU_ASSERT (ssm_dbuffer_length(&a.buf) == 0);
}

static void test_trashed (void)
{
    const char* terminator;
    const void* previousData;
    ssm_dbuffer_declare (b1);
    ssm_dbuffer_declare (b2);

    CU_ASSERT (ssm_dbuffer_import(&b1, "abcd", 4) == SSM_OK);
    CU_ASSERT (ssm_dbuffer_length(&b1) == 4);
    CU_ASSERT (memcmp(ssm_dbuffer_data(&b1), "abcd", 4) == 0);

    /* Address of null terminator */
    terminator = (const char*)ssm_dbuffer_data(&b1) + 4;
    CU_ASSERT (*terminator == '\0');

    /* Trashing the null terminator */
    *(char*)terminator = 'x';

    /* Warning - use of private data */
    /* Keep track of last allocated area */
    previousData = b1._data;

    CU_ASSERT (ssm_dbuffer_import(&b1, "xy", 1) == SSM_CORRUPTED);
    CU_ASSERT (ssm_dbuffer_copy(&b1, &b2) == SSM_CORRUPTED);
    CU_ASSERT (ssm_dbuffer_concat(&b1, &b2) == SSM_CORRUPTED);
    CU_ASSERT (ssm_dbuffer_compare(&b1, &b2) == SSM_CORRUPTED);
    CU_ASSERT (ssm_dbuffer_resize(&b1, 1) == SSM_CORRUPTED);
    CU_ASSERT (ssm_dbuffer_set(&b1, 99) == SSM_CORRUPTED);
    CU_ASSERT (ssm_dbuffer_set_range(&b1, 1, 2, 99) == SSM_CORRUPTED);
    CU_ASSERT (ssm_dbuffer_insert(&b1, 0, 2) == SSM_CORRUPTED);

    /* Warning - use of private data */
    /* Check that allocated area was unchanged */
    CU_ASSERT (b1._data == previousData);

    /* Restore null terminator */
    CU_ASSERT (*terminator == 'x');
    *(char*)terminator = '\0';

    CU_ASSERT (ssm_dbuffer_free(&b1) == SSM_OK);
    CU_ASSERT (ssm_dbuffer_length(&b1) == 0);

    CU_ASSERT (ssm_dbuffer_free(&b2) == SSM_OK);
    CU_ASSERT (ssm_dbuffer_length(&b2) == 0);
}

/*
 * Add the tests in this module in a specified test suite.
 */
static void ssm_dbuffer_add_tests(CU_pSuite suite)
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
void ssm_dbuffer_test_init(void)
{
    const CU_pSuite suite = CU_add_suite ("dbuffer", init_suite, cleanup_suite);
    ssm_dbuffer_add_tests(suite);
}
#endif /* SSM_USE_CANARY */

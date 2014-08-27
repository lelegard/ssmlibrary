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
 *  CUnit test suite for ssm_dstring_t
 *
 *  Do not forget to run this test suite under valgrind to look for memory leak.
 *
 *-----------------------------------------------------------------------------
 *
 *  Modification history:
 *    2014-01-06 Thierry Lelegard
 *               Original creation.
 *    2014-06-20 Thierry Lelegard
 *               Added ssm_dstring_status_string test.
 *
 *-----------------------------------------------------------------------------
 */

#include "ssm_dstring_test.h"
#include "ssm.h"
#include "utests.h"
#include <string.h>

/*
 * Test cases
 */
static void test_null(void)
{
    ssm_dstring_t* const null = NULL;

    CU_ASSERT (ssm_dstring_length(null) == 0);
    CU_ASSERT (ssm_dstring_chars(null) != NULL);
    CU_ASSERT (*ssm_dstring_chars(null) == '\0');
    CU_ASSERT (ssm_dstring_import(null, "") == SSM_NULLOUT);
    CU_ASSERT (ssm_dstring_import(null, NULL) == SSM_NULLOUT);
    CU_ASSERT (ssm_dstring_import_size(null, "", 10) == SSM_NULLOUT);
    CU_ASSERT (ssm_dstring_import_size(null, NULL, 10) == SSM_NULLOUT);
    CU_ASSERT (ssm_dstring_set(null, 'x') == SSM_NULLOUT);
    CU_ASSERT (ssm_dstring_set_range(null, 1, 2, 'x') == SSM_NULLOUT);
    CU_ASSERT (ssm_dstring_copy(null, null) == SSM_NULLOUT);
    CU_ASSERT (ssm_dstring_concat(null, null) == SSM_NULLOUT);
    CU_ASSERT (ssm_dstring_compare(null, null) == SSM_EQUAL);
    CU_ASSERT (ssm_dstring_free(null) == SSM_OK);
}

static void test_zero(void)
{
    ssm_dstring_declare(s);

    CU_ASSERT (ssm_dstring_length(&s) == 0);
    CU_ASSERT (strcmp(ssm_dstring_chars(&s), "") == 0);

    CU_ASSERT (ssm_dstring_free(&s) == SSM_OK);
    CU_ASSERT (ssm_dstring_length(&s) == 0);
    CU_ASSERT (strcmp(ssm_dstring_chars(&s), "") == 0);

    CU_ASSERT (ssm_dstring_free(&s) == SSM_OK);
    CU_ASSERT (ssm_dstring_length(&s) == 0);
    CU_ASSERT (strcmp(ssm_dstring_chars(&s), "") == 0);
}

static void test_import(void)
{
    ssm_dstring_declare(s);

    CU_ASSERT (ssm_dstring_length(&s) == 0);
    CU_ASSERT (strcmp(ssm_dstring_chars(&s), "") == 0);

    CU_ASSERT (ssm_dstring_import(&s, "abcd") == SSM_OK);
    CU_ASSERT (ssm_dstring_length(&s) == 4);
    CU_ASSERT (strcmp(ssm_dstring_chars(&s), "abcd") == 0);

    CU_ASSERT (ssm_dstring_import(&s, NULL) == SSM_OK);
    CU_ASSERT (ssm_dstring_length(&s) == 0);
    CU_ASSERT (strcmp(ssm_dstring_chars(&s), "") == 0);

    CU_ASSERT (ssm_dstring_import(&s, "abcd") == SSM_OK);
    CU_ASSERT (ssm_dstring_length(&s) == 4);
    CU_ASSERT (strcmp(ssm_dstring_chars(&s), "abcd") == 0);

    CU_ASSERT (ssm_dstring_import(&s, "ABCDEFGHIJKLMNOP") == SSM_OK);
    CU_ASSERT (ssm_dstring_length(&s) == 16);
    CU_ASSERT (strcmp(ssm_dstring_chars(&s), "ABCDEFGHIJKLMNOP") == 0);

    CU_ASSERT (ssm_dstring_free(&s) == SSM_OK);
    CU_ASSERT (ssm_dstring_length(&s) == 0);
    CU_ASSERT (strcmp(ssm_dstring_chars(&s), "") == 0);
}

static void test_alignment (void)
{
    static const char data[] = "abcdefghijklmnopqrst"; /* 20 chars */
    ssm_dstring_declare (s);

    /* Test various source data alignments */

    CU_ASSERT (ssm_dstring_import(&s, data) == SSM_OK);
    CU_ASSERT (ssm_dstring_length(&s) == 20);
    CU_ASSERT (strcmp(ssm_dstring_chars(&s), data) == 0);

    CU_ASSERT (ssm_dstring_import(&s, data + 1) == SSM_OK);
    CU_ASSERT (ssm_dstring_length(&s) == 19);
    CU_ASSERT (strcmp(ssm_dstring_chars(&s), data + 1) == 0);

    CU_ASSERT (ssm_dstring_import(&s, data + 2) == SSM_OK);
    CU_ASSERT (ssm_dstring_length(&s) == 18);
    CU_ASSERT (strcmp(ssm_dstring_chars(&s), data + 2) == 0);

    CU_ASSERT (ssm_dstring_import(&s, data + 3) == SSM_OK);
    CU_ASSERT (ssm_dstring_length(&s) == 17);
    CU_ASSERT (strcmp(ssm_dstring_chars(&s), data + 3) == 0);

    CU_ASSERT (ssm_dstring_import(&s, data + 4) == SSM_OK);
    CU_ASSERT (ssm_dstring_length(&s) == 16);
    CU_ASSERT (strcmp(ssm_dstring_chars(&s), data + 4) == 0);

    CU_ASSERT (ssm_dstring_import(&s, data + 5) == SSM_OK);
    CU_ASSERT (ssm_dstring_length(&s) == 15);
    CU_ASSERT (strcmp(ssm_dstring_chars(&s), data + 5) == 0);

    CU_ASSERT (ssm_dstring_import(&s, data + 6) == SSM_OK);
    CU_ASSERT (ssm_dstring_length(&s) == 14);
    CU_ASSERT (strcmp(ssm_dstring_chars(&s), data + 6) == 0);

    CU_ASSERT (ssm_dstring_import(&s, data + 7) == SSM_OK);
    CU_ASSERT (ssm_dstring_length(&s) == 13);
    CU_ASSERT (strcmp(ssm_dstring_chars(&s), data + 7) == 0);

    CU_ASSERT (ssm_dstring_free(&s) == SSM_OK);
    CU_ASSERT (ssm_dstring_length(&s) == 0);
    CU_ASSERT (strcmp(ssm_dstring_chars(&s), "") == 0);
}

static void test_import_size(void)
{
    ssm_dstring_declare(s);

    CU_ASSERT (ssm_dstring_length(&s) == 0);
    CU_ASSERT (strcmp(ssm_dstring_chars(&s), "") == 0);

    CU_ASSERT (ssm_dstring_import_size(&s, "", SSM_SIZE_MAX + 1) == SSM_SIZETOOLARGE);
    CU_ASSERT (ssm_dstring_length(&s) == 0);
    CU_ASSERT (strcmp(ssm_dstring_chars(&s), "") == 0);

    CU_ASSERT (ssm_dstring_import_size(&s, "abcd", 30) == SSM_OK);
    CU_ASSERT (ssm_dstring_length(&s) == 4);
    CU_ASSERT (strcmp(ssm_dstring_chars(&s), "abcd") == 0);

    CU_ASSERT (ssm_dstring_import_size(&s, NULL, 0) == SSM_OK);
    CU_ASSERT (ssm_dstring_length(&s) == 0);
    CU_ASSERT (strcmp(ssm_dstring_chars(&s), "") == 0);

    CU_ASSERT (ssm_dstring_import_size(&s, "abcd", 30) == SSM_OK);
    CU_ASSERT (ssm_dstring_length(&s) == 4);
    CU_ASSERT (strcmp(ssm_dstring_chars(&s), "abcd") == 0);

    CU_ASSERT (ssm_dstring_import_size(&s, NULL, 200) == SSM_OK);
    CU_ASSERT (ssm_dstring_length(&s) == 0);
    CU_ASSERT (strcmp(ssm_dstring_chars(&s), "") == 0);

    CU_ASSERT (ssm_dstring_import_size(&s, "abcd", 30) == SSM_OK);
    CU_ASSERT (ssm_dstring_length(&s) == 4);
    CU_ASSERT (strcmp(ssm_dstring_chars(&s), "abcd") == 0);

    CU_ASSERT (ssm_dstring_import_size(&s, "ABCD", 3) == SSM_OK);
    CU_ASSERT (ssm_dstring_length(&s) == 3);
    CU_ASSERT (strcmp(ssm_dstring_chars(&s), "ABC") == 0);

    CU_ASSERT (ssm_dstring_import_size(&s, "abcdefghij", 9) == SSM_OK);
    CU_ASSERT (ssm_dstring_length(&s) == 9);
    CU_ASSERT (strcmp(ssm_dstring_chars(&s), "abcdefghi") == 0);

    CU_ASSERT (ssm_dstring_import_size(&s, "ABCDEFGHIJ", 10) == SSM_OK);
    CU_ASSERT (ssm_dstring_length(&s) == 10);
    CU_ASSERT (strcmp(ssm_dstring_chars(&s), "ABCDEFGHIJ") == 0);

    CU_ASSERT (ssm_dstring_import_size(&s, "abcdefghij", 11) == SSM_OK);
    CU_ASSERT (ssm_dstring_length(&s) == 10);
    CU_ASSERT (strcmp(ssm_dstring_chars(&s), "abcdefghij") == 0);

    CU_ASSERT (ssm_dstring_import_size(&s, "ABCDEFGHIJK", 12) == SSM_OK);
    CU_ASSERT (ssm_dstring_length(&s) == 11);
    CU_ASSERT (strcmp(ssm_dstring_chars(&s), "ABCDEFGHIJK") == 0);

    CU_ASSERT (ssm_dstring_free(&s) == SSM_OK);
    CU_ASSERT (ssm_dstring_length(&s) == 0);
    CU_ASSERT (strcmp(ssm_dstring_chars(&s), "") == 0);
}

static void test_set(void)
{
    ssm_dstring_declare (s);

    CU_ASSERT (ssm_dstring_set(&s, 'x') == SSM_OK);
    CU_ASSERT (ssm_dstring_length(&s) == 0);

    CU_ASSERT (ssm_dstring_import(&s, "abcdefgh") == SSM_OK);
    CU_ASSERT (ssm_dstring_length(&s) == 8);
    CU_ASSERT (strcmp(ssm_dstring_chars(&s), "abcdefgh") == 0);

    CU_ASSERT (ssm_dstring_set(&s, 'x') == SSM_OK);
    CU_ASSERT (ssm_dstring_length(&s) == 8);
    CU_ASSERT (strcmp(ssm_dstring_chars(&s), "xxxxxxxx") == 0);

    CU_ASSERT (ssm_dstring_free(&s) == SSM_OK);
    CU_ASSERT (ssm_dstring_length(&s) == 0);
    CU_ASSERT (strcmp(ssm_dstring_chars(&s), "") == 0);
}

static void test_set_range(void)
{
    ssm_dstring_declare (s);

    CU_ASSERT (ssm_dstring_set_range(&s, 0, 0, 'x') == SSM_OK);
    CU_ASSERT (ssm_dstring_length(&s) == 0);

    CU_ASSERT (ssm_dstring_set_range(&s, 10, 10, 'x') == SSM_TRUNCATED);
    CU_ASSERT (ssm_dstring_length(&s) == 0);

    CU_ASSERT (ssm_dstring_set_range(&s, 0, 10, 'x') == SSM_TRUNCATED);
    CU_ASSERT (ssm_dstring_length(&s) == 0);

    CU_ASSERT (ssm_dstring_import(&s, "abcdefgh") == SSM_OK);
    CU_ASSERT (ssm_dstring_length(&s) == 8);
    CU_ASSERT (strcmp(ssm_dstring_chars(&s), "abcdefgh") == 0);

    CU_ASSERT (ssm_dstring_set_range(&s, 2, 3, 'x') == SSM_OK);
    CU_ASSERT (ssm_dstring_length(&s) == 8);
    CU_ASSERT (strcmp(ssm_dstring_chars(&s), "abxxxfgh") == 0);

    CU_ASSERT (ssm_dstring_set_range(&s, 6, 1, 'y') == SSM_OK);
    CU_ASSERT (ssm_dstring_length(&s) == 8);
    CU_ASSERT (strcmp(ssm_dstring_chars(&s), "abxxxfyh") == 0);

    CU_ASSERT (ssm_dstring_set_range(&s, 4, 66, 'z') == SSM_TRUNCATED);
    CU_ASSERT (ssm_dstring_length(&s) == 8);
    CU_ASSERT (strcmp(ssm_dstring_chars(&s), "abxxzzzz") == 0);

    CU_ASSERT (ssm_dstring_set_range(&s, SSM_SIZE_MAX + 1, 2, 12) == SSM_SIZETOOLARGE);
    CU_ASSERT (ssm_dstring_length(&s) == 8);
    CU_ASSERT (strcmp(ssm_dstring_chars(&s), "abxxzzzz") == 0);

    CU_ASSERT (ssm_dstring_set_range(&s, 1, SSM_SIZE_MAX + 1, 12) == SSM_SIZETOOLARGE);
    CU_ASSERT (ssm_dstring_length(&s) == 8);
    CU_ASSERT (strcmp(ssm_dstring_chars(&s), "abxxzzzz") == 0);

    CU_ASSERT (ssm_dstring_set_range(&s, 0, 7, 'p') == SSM_OK);
    CU_ASSERT (ssm_dstring_length(&s) == 8);
    CU_ASSERT (strcmp(ssm_dstring_chars(&s), "pppppppz") == 0);

    CU_ASSERT (ssm_dstring_set_range(&s, 8, 0, 's') == SSM_OK);
    CU_ASSERT (ssm_dstring_length(&s) == 8);
    CU_ASSERT (strcmp(ssm_dstring_chars(&s), "pppppppz") == 0);

    CU_ASSERT (ssm_dstring_set_range(&s, 9, 0, 's') == SSM_TRUNCATED);
    CU_ASSERT (ssm_dstring_length(&s) == 8);
    CU_ASSERT (strcmp(ssm_dstring_chars(&s), "pppppppz") == 0);

    CU_ASSERT (ssm_dstring_set_range(&s, 8, 1, 's') == SSM_TRUNCATED);
    CU_ASSERT (ssm_dstring_length(&s) == 8);
    CU_ASSERT (strcmp(ssm_dstring_chars(&s), "pppppppz") == 0);

    CU_ASSERT (ssm_dstring_free(&s) == SSM_OK);
    CU_ASSERT (ssm_dstring_length(&s) == 0);
    CU_ASSERT (strcmp(ssm_dstring_chars(&s), "") == 0);
}

static void test_copy(void)
{
    ssm_dstring_t* const null = NULL;
    ssm_dstring_declare(s1);
    ssm_dstring_declare(s2);

    CU_ASSERT (ssm_dstring_import(&s1, "abcd") == SSM_OK);
    CU_ASSERT (ssm_dstring_length(&s1) == 4);
    CU_ASSERT (strcmp(ssm_dstring_chars(&s1), "abcd") == 0);

    CU_ASSERT (ssm_dstring_copy(&s1, null) == SSM_OK);
    CU_ASSERT (ssm_dstring_length(&s1) == 0);
    CU_ASSERT (strcmp(ssm_dstring_chars(&s1), "") == 0);

    CU_ASSERT (ssm_dstring_import(&s1, "abcd") == SSM_OK);
    CU_ASSERT (ssm_dstring_length(&s1) == 4);
    CU_ASSERT (strcmp(ssm_dstring_chars(&s1), "abcd") == 0);

    CU_ASSERT (ssm_dstring_import(&s2, "ef") == SSM_OK);
    CU_ASSERT (ssm_dstring_length(&s2) == 2);
    CU_ASSERT (strcmp(ssm_dstring_chars(&s2), "ef") == 0);

    CU_ASSERT (ssm_dstring_copy(&s1, &s2) == SSM_OK);
    CU_ASSERT (ssm_dstring_length(&s1) == 2);
    CU_ASSERT (strcmp(ssm_dstring_chars(&s1), "ef") == 0);

    CU_ASSERT (ssm_dstring_import(&s2, "ghijklmn") == SSM_OK);
    CU_ASSERT (ssm_dstring_length(&s2) == 8);
    CU_ASSERT (strcmp(ssm_dstring_chars(&s2), "ghijklmn") == 0);

    CU_ASSERT (ssm_dstring_copy(&s1, &s2) == SSM_OK);
    CU_ASSERT (ssm_dstring_length(&s1) == 8);
    CU_ASSERT (strcmp(ssm_dstring_chars(&s1), "ghijklmn") == 0);

    CU_ASSERT (ssm_dstring_free(&s1) == SSM_OK);
    CU_ASSERT (ssm_dstring_length(&s1) == 0);
    CU_ASSERT (strcmp(ssm_dstring_chars(&s1), "") == 0);

    CU_ASSERT (ssm_dstring_free(&s2) == SSM_OK);
    CU_ASSERT (ssm_dstring_length(&s2) == 0);
    CU_ASSERT (strcmp(ssm_dstring_chars(&s2), "") == 0);
}

static void test_copy_overlap (void)
{
    ssm_dstring_declare(s);

    CU_ASSERT (ssm_dstring_length(&s) == 0);
    CU_ASSERT (strcmp(ssm_dstring_chars(&s), "") == 0);

    CU_ASSERT (ssm_dstring_copy(&s, &s) == SSM_OK);
    CU_ASSERT (ssm_dstring_length(&s) == 0);
    CU_ASSERT (strcmp(ssm_dstring_chars(&s), "") == 0);

    CU_ASSERT (ssm_dstring_import(&s, "abcd") == SSM_OK);
    CU_ASSERT (ssm_dstring_length(&s) == 4);
    CU_ASSERT (strcmp(ssm_dstring_chars(&s), "abcd") == 0);

    CU_ASSERT (ssm_dstring_copy(&s, &s) == SSM_OK);
    CU_ASSERT (ssm_dstring_length(&s) == 4);
    CU_ASSERT (strcmp(ssm_dstring_chars(&s), "abcd") == 0);

    CU_ASSERT (ssm_dstring_free(&s) == SSM_OK);
    CU_ASSERT (ssm_dstring_length(&s) == 0);
    CU_ASSERT (strcmp(ssm_dstring_chars(&s), "") == 0);
}

static void test_concat(void)
{
    ssm_dstring_t* const null = NULL;
    ssm_dstring_declare(s1);
    ssm_dstring_declare(s2);

    CU_ASSERT (ssm_dstring_import(&s1, "abcd") == SSM_OK);
    CU_ASSERT (ssm_dstring_length(&s1) == 4);
    CU_ASSERT (strcmp(ssm_dstring_chars(&s1), "abcd") == 0);

    CU_ASSERT (ssm_dstring_concat(&s1, null) == SSM_OK);
    CU_ASSERT (ssm_dstring_length(&s1) == 4);
    CU_ASSERT (strcmp(ssm_dstring_chars(&s1), "abcd") == 0);

    CU_ASSERT (ssm_dstring_concat(&s1, &s2) == SSM_OK);
    CU_ASSERT (ssm_dstring_length(&s1) == 4);
    CU_ASSERT (strcmp(ssm_dstring_chars(&s1), "abcd") == 0);

    CU_ASSERT (ssm_dstring_import(&s2, "ef") == SSM_OK);
    CU_ASSERT (ssm_dstring_length(&s2) == 2);
    CU_ASSERT (strcmp(ssm_dstring_chars(&s2), "ef") == 0);

    CU_ASSERT (ssm_dstring_concat(&s1, &s2) == SSM_OK);
    CU_ASSERT (ssm_dstring_length(&s1) == 6);
    CU_ASSERT (strcmp(ssm_dstring_chars(&s1), "abcdef") == 0);

    CU_ASSERT (ssm_dstring_import(&s2, "ghijklmn") == SSM_OK);
    CU_ASSERT (ssm_dstring_length(&s2) == 8);
    CU_ASSERT (strcmp(ssm_dstring_chars(&s2), "ghijklmn") == 0);

    CU_ASSERT (ssm_dstring_concat(&s1, &s2) == SSM_OK);
    CU_ASSERT (ssm_dstring_length(&s1) == 14);
    CU_ASSERT (strcmp(ssm_dstring_chars(&s1), "abcdefghijklmn") == 0);

    CU_ASSERT (ssm_dstring_free(&s1) == SSM_OK);
    CU_ASSERT (ssm_dstring_length(&s1) == 0);
    CU_ASSERT (strcmp(ssm_dstring_chars(&s1), "") == 0);

    CU_ASSERT (ssm_dstring_free(&s2) == SSM_OK);
    CU_ASSERT (ssm_dstring_length(&s2) == 0);
    CU_ASSERT (strcmp(ssm_dstring_chars(&s2), "") == 0);
}

static void test_concat_overlap (void)
{
    ssm_dstring_declare(s);

    CU_ASSERT (ssm_dstring_length(&s) == 0);
    CU_ASSERT (strcmp(ssm_dstring_chars(&s), "") == 0);

    CU_ASSERT (ssm_dstring_concat(&s, &s) == SSM_OK);
    CU_ASSERT (ssm_dstring_length(&s) == 0);
    CU_ASSERT (strcmp(ssm_dstring_chars(&s), "") == 0);

    CU_ASSERT (ssm_dstring_import(&s, "abcd") == SSM_OK);
    CU_ASSERT (ssm_dstring_length(&s) == 4);
    CU_ASSERT (strcmp(ssm_dstring_chars(&s), "abcd") == 0);

    CU_ASSERT (ssm_dstring_concat(&s, &s) == SSM_OK);
    CU_ASSERT (ssm_dstring_length(&s) == 8);
    CU_ASSERT (strcmp(ssm_dstring_chars(&s), "abcdabcd") == 0);

    CU_ASSERT (ssm_dstring_free(&s) == SSM_OK);
    CU_ASSERT (ssm_dstring_length(&s) == 0);
    CU_ASSERT (strcmp(ssm_dstring_chars(&s), "") == 0);
}

static void test_compare (void)
{
    ssm_dstring_declare (b1);
    ssm_dstring_declare (b2);
    ssm_dstring_t* const null = NULL;

    CU_ASSERT (ssm_dstring_compare(&b1, null) == SSM_EQUAL);
    CU_ASSERT (ssm_dstring_compare(&b2, null) == SSM_EQUAL);
    CU_ASSERT (ssm_dstring_compare(&b1, &b2) == SSM_EQUAL);

    CU_ASSERT (ssm_dstring_import(&b1, "abcd") == SSM_OK);
    CU_ASSERT (ssm_dstring_length(&b1) == 4);
    CU_ASSERT (strcmp(ssm_dstring_chars(&b1), "abcd") == 0);

    CU_ASSERT (ssm_dstring_compare(&b1, null) == SSM_GREATER);
    CU_ASSERT (ssm_dstring_compare(&b1, &b2) == SSM_GREATER);
    CU_ASSERT (ssm_dstring_compare(&b2, &b1) == SSM_LOWER);

    CU_ASSERT (ssm_dstring_import(&b2, "abedef") == SSM_OK);
    CU_ASSERT (ssm_dstring_length(&b2) == 6);
    CU_ASSERT (strcmp(ssm_dstring_chars(&b2), "abedef") == 0);

    CU_ASSERT (ssm_dstring_compare(&b1, &b2) == SSM_LOWER);
    CU_ASSERT (ssm_dstring_compare(&b2, &b1) == SSM_GREATER);

    CU_ASSERT (ssm_dstring_import(&b2, "abcd") == SSM_OK);
    CU_ASSERT (ssm_dstring_length(&b2) == 4);
    CU_ASSERT (strcmp(ssm_dstring_chars(&b2), "abcd") == 0);

    CU_ASSERT (ssm_dstring_compare(&b1, &b2) == SSM_EQUAL);
    CU_ASSERT (ssm_dstring_compare(&b2, &b1) == SSM_EQUAL);

    CU_ASSERT (ssm_dstring_free(&b1) == SSM_OK);
    CU_ASSERT (ssm_dstring_length(&b1) == 0);

    CU_ASSERT (ssm_dstring_free(&b2) == SSM_OK);
    CU_ASSERT (ssm_dstring_length(&b2) == 0);
}

static void test_struct (void)
{
    typedef struct {
        int before;
        ssm_dstring_t str;
        int after;
    } TestStruct;

    TestStruct a = {
        0x1234567,
        ssm_dstring_init,
        0x1ABCDEF
    };

    static const char ref[] = "elkfvzelvlevlkzrlbrbnrtnyortgfgernvrjvfnkvze7892Hurt";

    CU_ASSERT (a.before == 0x1234567);
    CU_ASSERT (ssm_dstring_length(&a.str) == 0);
    CU_ASSERT (strcmp(ssm_dstring_chars(&a.str), "") == 0);
    CU_ASSERT (a.after == 0x1ABCDEF);

    CU_ASSERT (ssm_dstring_import(&a.str, ref) == SSM_OK);

    CU_ASSERT (a.before == 0x1234567);
    CU_ASSERT (ssm_dstring_length(&a.str) == 52);
    CU_ASSERT (strcmp(ssm_dstring_chars(&a.str), ref) == 0);
    CU_ASSERT (a.after == 0x1ABCDEF);

    CU_ASSERT (ssm_dstring_free(&a.str) == SSM_OK);
    CU_ASSERT (ssm_dstring_length(&a.str) == 0);
    CU_ASSERT (strcmp(ssm_dstring_chars(&a.str), "") == 0);
}

static void test_trashed (void)
{
    const char* terminator;
    const void* previousData;
    ssm_dstring_declare (s1);
    ssm_dstring_declare (s2);

    CU_ASSERT (ssm_dstring_import(&s1, "abcd") == SSM_OK);
    CU_ASSERT (ssm_dstring_length(&s1) == 4);
    CU_ASSERT (strcmp(ssm_dstring_chars(&s1), "abcd") == 0);

    CU_ASSERT (ssm_dstring_import(&s2, "ef") == SSM_OK);
    CU_ASSERT (ssm_dstring_length(&s2) == 2);
    CU_ASSERT (strcmp(ssm_dstring_chars(&s2), "ef") == 0);

    /* Address of null terminator */
    terminator = ssm_dstring_chars(&s1) + 4;
    CU_ASSERT (*terminator == '\0');

    /* Trashing the null terminator */
    *(char*)terminator = 'x';

    /* Warning - use of private data */
    /* Keep track of last allocated area */
    previousData = s1._buf._data;

    CU_ASSERT (ssm_dstring_import(&s1, "xy") == SSM_CORRUPTED);
    CU_ASSERT (ssm_dstring_copy(&s1, &s2) == SSM_CORRUPTED);
    CU_ASSERT (ssm_dstring_concat(&s1, &s2) == SSM_CORRUPTED);
    CU_ASSERT (ssm_dstring_compare(&s1, &s2) == SSM_CORRUPTED);

    /* Warning - use of private data */
    /* Check that allocated area was unchanged */
    CU_ASSERT (s1._buf._data == previousData);

    /* Restore null terminator */
    CU_ASSERT (*terminator == 'x');
    *(char*)terminator = '\0';

    CU_ASSERT (ssm_dstring_free(&s1) == SSM_OK);
    CU_ASSERT (ssm_dstring_length(&s1) == 0);
    CU_ASSERT (strcmp(ssm_dstring_chars(&s1), "") == 0);

    CU_ASSERT (ssm_dstring_free(&s2) == SSM_OK);
    CU_ASSERT (ssm_dstring_length(&s2) == 0);
    CU_ASSERT (strcmp(ssm_dstring_chars(&s2), "") == 0);
}

static void test_status_string (void)
{
    int i;
    ssm_dstring_declare (s);

    for (i = 0; i < 256; i++) {
        const ssm_status_t status = (ssm_status_t)i;
        const char* msg;

        CU_ASSERT (ssm_dstring_status_string(&s, status) == SSM_OK);
        msg = ssm_dstring_chars(&s);

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
        case SSM_NULLIN:       CU_ASSERT_STRING_EQUAL (msg, "A NULL pointer was provided as input parameter"); break;
        case SSM_NOMEMORY:     CU_ASSERT_STRING_EQUAL (msg, "Memory allocation failure, result is unchanged"); break;
        case SSM_CORRUPTED:    CU_ASSERT_STRING_EQUAL (msg, "Memory was previously corrupted, result is undefined but safe"); break;
        case SSM_BUG:          CU_ASSERT_STRING_EQUAL (msg, "Internal inconsistency, there is a bug in the SSM library"); break;
        default:               CU_ASSERT_STRING_EQUAL (msg, "Unknown"); break;
        }
    }

    CU_ASSERT (ssm_dstring_free(&s) == SSM_OK);
    CU_ASSERT (ssm_dstring_length(&s) == 0);
}

/*
 * Add the tests in this module in a specified test suite.
 */
static void ssm_dstring_add_tests(CU_pSuite suite)
{
    CU_ADD_TEST (suite, test_null);
    CU_ADD_TEST (suite, test_zero);
    CU_ADD_TEST (suite, test_import);
    CU_ADD_TEST (suite, test_alignment);
    CU_ADD_TEST (suite, test_import_size);
    CU_ADD_TEST (suite, test_set);
    CU_ADD_TEST (suite, test_set_range);
    CU_ADD_TEST (suite, test_copy);
    CU_ADD_TEST (suite, test_copy_overlap);
    CU_ADD_TEST (suite, test_concat);
    CU_ADD_TEST (suite, test_concat_overlap);
    CU_ADD_TEST (suite, test_compare);
    CU_ADD_TEST (suite, test_struct);
    CU_ADD_TEST (suite, test_trashed);
    CU_ADD_TEST (suite, test_status_string);
}

/*
 * Test suite initialization function.
 * Returns zero on success, non-zero on error.
 */
static int init_suite(void)
{
    return 0;
}

/*
 * Test suite cleanup function.
 * Returns zero on success, non-zero on error.
 */
static int cleanup_suite(void)
{
    return 0;
}

/*
 * Initialize the test suite in this module.
 * We assume here that the main program has set CUnit error mode
 * as "abort" so there is no need to test individual errors.
 */
#if !defined(SSM_USE_CANARY)
void ssm_dstring_test_init(void)
{
    const CU_pSuite suite = CU_add_suite ("dstring", init_suite, cleanup_suite);
    ssm_dstring_add_tests(suite);
}
#endif /* SSM_USE_CANARY */

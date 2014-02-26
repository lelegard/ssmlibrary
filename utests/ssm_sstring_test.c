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
 *  CUnit test suite for ssm_sstring_t
 *
 *-----------------------------------------------------------------------------
 *
 *  Modification history:
 *    2014-01-06 Thierry Lelegard
 *               Original creation.
 *
 *-----------------------------------------------------------------------------
 */

#include "ssm_sstring_test.h"
#include "ssm.h"
#include "utests.h"
#include <string.h>

/*
 * Test cases
 */
static void test_null (void)
{
    ssm_sstring_t* const null = NULL;

    CU_ASSERT (ssm_sstring_length(null) == 0);
    CU_ASSERT (ssm_sstring_max_length(null) == 0);
    CU_ASSERT (ssm_sstring_chars(null) != NULL);
    CU_ASSERT (*ssm_sstring_chars(null) == '\0');
    CU_ASSERT (ssm_sstring_import(null, "") == SSM_NULLOUT);
    CU_ASSERT (ssm_sstring_import(null, NULL) == SSM_NULLOUT);
    CU_ASSERT (ssm_sstring_import_size(null, "", 10) == SSM_NULLOUT);
    CU_ASSERT (ssm_sstring_import_size(null, NULL, 10) == SSM_NULLOUT);
    CU_ASSERT (ssm_sstring_set(null, 'x') == SSM_NULLOUT);
    CU_ASSERT (ssm_sstring_set_range(null, 1, 2, 'x') == SSM_NULLOUT);
    CU_ASSERT (ssm_sstring_copy(null, null) == SSM_NULLOUT);
    CU_ASSERT (ssm_sstring_concat(null, null) == SSM_NULLOUT);
    CU_ASSERT (ssm_sstring_compare(null, null) == SSM_EQUAL);
}

static void test_zero (void)
{
    ssm_sstring_declare (s, 0);

    CU_ASSERT (ssm_sstring_length(&s.str) == 0);
    CU_ASSERT (ssm_sstring_max_length(&s.str) == 0);
    CU_ASSERT (strcmp(ssm_sstring_chars(&s.str), "") == 0);

    CU_ASSERT (ssm_sstring_import(&s.str, "abcd") == SSM_TRUNCATED);
    CU_ASSERT (ssm_sstring_length(&s.str) == 0);
    CU_ASSERT (strcmp(ssm_sstring_chars(&s.str), "") == 0);
}

static void test_import (void)
{
    ssm_sstring_declare (s, 10);

    CU_ASSERT (ssm_sstring_length(&s.str) == 0);
    CU_ASSERT (ssm_sstring_max_length(&s.str) == 10);
    CU_ASSERT (strcmp(ssm_sstring_chars(&s.str), "") == 0);

    CU_ASSERT (ssm_sstring_import(&s.str, "abcd") == SSM_OK);
    CU_ASSERT (ssm_sstring_length(&s.str) == 4);
    CU_ASSERT (ssm_sstring_max_length(&s.str) == 10);
    CU_ASSERT (strcmp(ssm_sstring_chars(&s.str), "abcd") == 0);

    CU_ASSERT (ssm_sstring_import(&s.str, NULL) == SSM_OK);
    CU_ASSERT (ssm_sstring_length(&s.str) == 0);
    CU_ASSERT (ssm_sstring_max_length(&s.str) == 10);
    CU_ASSERT (strcmp(ssm_sstring_chars(&s.str), "") == 0);

    CU_ASSERT (ssm_sstring_import(&s.str, "ABCDEFGHIJ") == SSM_OK);
    CU_ASSERT (ssm_sstring_length(&s.str) == 10);
    CU_ASSERT (ssm_sstring_max_length(&s.str) == 10);
    CU_ASSERT (strcmp(ssm_sstring_chars(&s.str), "ABCDEFGHIJ") == 0);

    CU_ASSERT (ssm_sstring_import(&s.str, "abcdefghijk") == SSM_TRUNCATED);
    CU_ASSERT (ssm_sstring_length(&s.str) == 10);
    CU_ASSERT (ssm_sstring_max_length(&s.str) == 10);
    CU_ASSERT (strcmp(ssm_sstring_chars(&s.str), "abcdefghij") == 0);

    CU_ASSERT (ssm_sstring_import(&s.str, "ABCDEFGHIJKLMNOP") == SSM_TRUNCATED);
    CU_ASSERT (ssm_sstring_length(&s.str) == 10);
    CU_ASSERT (ssm_sstring_max_length(&s.str) == 10);
    CU_ASSERT (strcmp(ssm_sstring_chars(&s.str), "ABCDEFGHIJ") == 0);
}

static void test_alignment (void)
{
    static const char data[] = "abcdefghijklmnopqrst"; /* 20 chars */
    ssm_sstring_declare (s, 40);

    /* Test various source data alignments */

    CU_ASSERT (ssm_sstring_import(&s.str, data) == SSM_OK);
    CU_ASSERT (ssm_sstring_length(&s.str) == 20);
    CU_ASSERT (strcmp(ssm_sstring_chars(&s.str), data) == 0);

    CU_ASSERT (ssm_sstring_import(&s.str, data + 1) == SSM_OK);
    CU_ASSERT (ssm_sstring_length(&s.str) == 19);
    CU_ASSERT (strcmp(ssm_sstring_chars(&s.str), data + 1) == 0);

    CU_ASSERT (ssm_sstring_import(&s.str, data + 2) == SSM_OK);
    CU_ASSERT (ssm_sstring_length(&s.str) == 18);
    CU_ASSERT (strcmp(ssm_sstring_chars(&s.str), data + 2) == 0);

    CU_ASSERT (ssm_sstring_import(&s.str, data + 3) == SSM_OK);
    CU_ASSERT (ssm_sstring_length(&s.str) == 17);
    CU_ASSERT (strcmp(ssm_sstring_chars(&s.str), data + 3) == 0);

    CU_ASSERT (ssm_sstring_import(&s.str, data + 4) == SSM_OK);
    CU_ASSERT (ssm_sstring_length(&s.str) == 16);
    CU_ASSERT (strcmp(ssm_sstring_chars(&s.str), data + 4) == 0);

    CU_ASSERT (ssm_sstring_import(&s.str, data + 5) == SSM_OK);
    CU_ASSERT (ssm_sstring_length(&s.str) == 15);
    CU_ASSERT (strcmp(ssm_sstring_chars(&s.str), data + 5) == 0);

    CU_ASSERT (ssm_sstring_import(&s.str, data + 6) == SSM_OK);
    CU_ASSERT (ssm_sstring_length(&s.str) == 14);
    CU_ASSERT (strcmp(ssm_sstring_chars(&s.str), data + 6) == 0);

    CU_ASSERT (ssm_sstring_import(&s.str, data + 7) == SSM_OK);
    CU_ASSERT (ssm_sstring_length(&s.str) == 13);
    CU_ASSERT (strcmp(ssm_sstring_chars(&s.str), data + 7) == 0);
}

static void test_import_size (void)
{
    ssm_sstring_declare (s, 10);

    CU_ASSERT (ssm_sstring_length(&s.str) == 0);
    CU_ASSERT (ssm_sstring_max_length(&s.str) == 10);
    CU_ASSERT (strcmp(ssm_sstring_chars(&s.str), "") == 0);

    CU_ASSERT (ssm_sstring_import_size(&s.str, "", SSM_SIZE_MAX + 1) == SSM_SIZETOOLARGE);
    CU_ASSERT (ssm_sstring_length(&s.str) == 0);
    CU_ASSERT (ssm_sstring_max_length(&s.str) == 10);
    CU_ASSERT (strcmp(ssm_sstring_chars(&s.str), "") == 0);

    CU_ASSERT (ssm_sstring_import_size(&s.str, "abcd", 30) == SSM_OK);
    CU_ASSERT (ssm_sstring_length(&s.str) == 4);
    CU_ASSERT (ssm_sstring_max_length(&s.str) == 10);
    CU_ASSERT (strcmp(ssm_sstring_chars(&s.str), "abcd") == 0);

    CU_ASSERT (ssm_sstring_import_size(&s.str, NULL, 0) == SSM_OK);
    CU_ASSERT (ssm_sstring_length(&s.str) == 0);
    CU_ASSERT (ssm_sstring_max_length(&s.str) == 10);
    CU_ASSERT (strcmp(ssm_sstring_chars(&s.str), "") == 0);

    CU_ASSERT (ssm_sstring_import_size(&s.str, "ABCD", 3) == SSM_OK);
    CU_ASSERT (ssm_sstring_length(&s.str) == 3);
    CU_ASSERT (ssm_sstring_max_length(&s.str) == 10);
    CU_ASSERT (strcmp(ssm_sstring_chars(&s.str), "ABC") == 0);

    CU_ASSERT (ssm_sstring_import_size(&s.str, NULL, 100) == SSM_OK);
    CU_ASSERT (ssm_sstring_length(&s.str) == 0);
    CU_ASSERT (ssm_sstring_max_length(&s.str) == 10);
    CU_ASSERT (strcmp(ssm_sstring_chars(&s.str), "") == 0);

    CU_ASSERT (ssm_sstring_import_size(&s.str, "abcdefghij", 9) == SSM_OK);
    CU_ASSERT (ssm_sstring_length(&s.str) == 9);
    CU_ASSERT (ssm_sstring_max_length(&s.str) == 10);
    CU_ASSERT (strcmp(ssm_sstring_chars(&s.str), "abcdefghi") == 0);

    CU_ASSERT (ssm_sstring_import_size(&s.str, "ABCDEFGHIJ", 10) == SSM_OK);
    CU_ASSERT (ssm_sstring_length(&s.str) == 10);
    CU_ASSERT (ssm_sstring_max_length(&s.str) == 10);
    CU_ASSERT (strcmp(ssm_sstring_chars(&s.str), "ABCDEFGHIJ") == 0);

    CU_ASSERT (ssm_sstring_import_size(&s.str, "abcdefghij", 11) == SSM_OK);
    CU_ASSERT (ssm_sstring_length(&s.str) == 10);
    CU_ASSERT (ssm_sstring_max_length(&s.str) == 10);
    CU_ASSERT (strcmp(ssm_sstring_chars(&s.str), "abcdefghij") == 0);

    CU_ASSERT (ssm_sstring_import_size(&s.str, "ABCDEFGHIJK", 20) == SSM_TRUNCATED);
    CU_ASSERT (ssm_sstring_length(&s.str) == 10);
    CU_ASSERT (ssm_sstring_max_length(&s.str) == 10);
    CU_ASSERT (strcmp(ssm_sstring_chars(&s.str), "ABCDEFGHIJ") == 0);

    CU_ASSERT (ssm_sstring_import_size(&s.str, "abcdefghijk", 8) == SSM_OK);
    CU_ASSERT (ssm_sstring_length(&s.str) == 8);
    CU_ASSERT (ssm_sstring_max_length(&s.str) == 10);
    CU_ASSERT (strcmp(ssm_sstring_chars(&s.str), "abcdefgh") == 0);

    CU_ASSERT (ssm_sstring_import_size(&s.str, "ABCDEFGHIJKLMNOP", 40) == SSM_TRUNCATED);
    CU_ASSERT (ssm_sstring_length(&s.str) == 10);
    CU_ASSERT (ssm_sstring_max_length(&s.str) == 10);
    CU_ASSERT (strcmp(ssm_sstring_chars(&s.str), "ABCDEFGHIJ") == 0);

    CU_ASSERT (ssm_sstring_import_size(&s.str, "abcdefghijklmnop", 10) == SSM_OK);
    CU_ASSERT (ssm_sstring_length(&s.str) == 10);
    CU_ASSERT (ssm_sstring_max_length(&s.str) == 10);
    CU_ASSERT (strcmp(ssm_sstring_chars(&s.str), "abcdefghij") == 0);

    CU_ASSERT (ssm_sstring_import_size(&s.str, "ABCDEFGHIJKLMNOP", 9) == SSM_OK);
    CU_ASSERT (ssm_sstring_length(&s.str) == 9);
    CU_ASSERT (ssm_sstring_max_length(&s.str) == 10);
    CU_ASSERT (strcmp(ssm_sstring_chars(&s.str), "ABCDEFGHI") == 0);
}

static void test_set(void)
{
    ssm_sstring_declare (s, 10);

    CU_ASSERT (ssm_sstring_set(&s.str, 'x') == SSM_OK);
    CU_ASSERT (ssm_sstring_length(&s.str) == 0);
    CU_ASSERT (ssm_sstring_max_length(&s.str) == 10);

    CU_ASSERT (ssm_sstring_import(&s.str, "abcdefgh") == SSM_OK);
    CU_ASSERT (ssm_sstring_length(&s.str) == 8);
    CU_ASSERT (ssm_sstring_max_length(&s.str) == 10);
    CU_ASSERT (strcmp(ssm_sstring_chars(&s.str), "abcdefgh") == 0);

    CU_ASSERT (ssm_sstring_set(&s.str, 'x') == SSM_OK);
    CU_ASSERT (ssm_sstring_length(&s.str) == 8);
    CU_ASSERT (ssm_sstring_max_length(&s.str) == 10);
    CU_ASSERT (strcmp(ssm_sstring_chars(&s.str), "xxxxxxxx") == 0);
}

static void test_set_range(void)
{
    ssm_sstring_declare (s, 10);

    CU_ASSERT (ssm_sstring_set_range(&s.str, 0, 0, 'x') == SSM_OK);
    CU_ASSERT (ssm_sstring_length(&s.str) == 0);
    CU_ASSERT (ssm_sstring_max_length(&s.str) == 10);

    CU_ASSERT (ssm_sstring_set_range(&s.str, 10, 10, 'x') == SSM_TRUNCATED);
    CU_ASSERT (ssm_sstring_length(&s.str) == 0);
    CU_ASSERT (ssm_sstring_max_length(&s.str) == 10);

    CU_ASSERT (ssm_sstring_set_range(&s.str, 0, 10, 'x') == SSM_TRUNCATED);
    CU_ASSERT (ssm_sstring_length(&s.str) == 0);
    CU_ASSERT (ssm_sstring_max_length(&s.str) == 10);

    CU_ASSERT (ssm_sstring_import(&s.str, "abcdefgh") == SSM_OK);
    CU_ASSERT (ssm_sstring_length(&s.str) == 8);
    CU_ASSERT (ssm_sstring_max_length(&s.str) == 10);
    CU_ASSERT (strcmp(ssm_sstring_chars(&s.str), "abcdefgh") == 0);

    CU_ASSERT (ssm_sstring_set_range(&s.str, 2, 3, 'x') == SSM_OK);
    CU_ASSERT (ssm_sstring_length(&s.str) == 8);
    CU_ASSERT (ssm_sstring_max_length(&s.str) == 10);
    CU_ASSERT (strcmp(ssm_sstring_chars(&s.str), "abxxxfgh") == 0);

    CU_ASSERT (ssm_sstring_set_range(&s.str, 6, 1, 'y') == SSM_OK);
    CU_ASSERT (ssm_sstring_length(&s.str) == 8);
    CU_ASSERT (ssm_sstring_max_length(&s.str) == 10);
    CU_ASSERT (strcmp(ssm_sstring_chars(&s.str), "abxxxfyh") == 0);

    CU_ASSERT (ssm_sstring_set_range(&s.str, 4, 66, 'z') == SSM_TRUNCATED);
    CU_ASSERT (ssm_sstring_length(&s.str) == 8);
    CU_ASSERT (ssm_sstring_max_length(&s.str) == 10);
    CU_ASSERT (strcmp(ssm_sstring_chars(&s.str), "abxxzzzz") == 0);

    CU_ASSERT (ssm_sstring_set_range(&s.str, SSM_SIZE_MAX + 1, 2, 12) == SSM_SIZETOOLARGE);
    CU_ASSERT (ssm_sstring_length(&s.str) == 8);
    CU_ASSERT (ssm_sstring_max_length(&s.str) == 10);
    CU_ASSERT (strcmp(ssm_sstring_chars(&s.str), "abxxzzzz") == 0);

    CU_ASSERT (ssm_sstring_set_range(&s.str, 1, SSM_SIZE_MAX + 1, 12) == SSM_SIZETOOLARGE);
    CU_ASSERT (ssm_sstring_length(&s.str) == 8);
    CU_ASSERT (ssm_sstring_max_length(&s.str) == 10);
    CU_ASSERT (strcmp(ssm_sstring_chars(&s.str), "abxxzzzz") == 0);

    CU_ASSERT (ssm_sstring_set_range(&s.str, 0, 7, 'p') == SSM_OK);
    CU_ASSERT (ssm_sstring_length(&s.str) == 8);
    CU_ASSERT (ssm_sstring_max_length(&s.str) == 10);
    CU_ASSERT (strcmp(ssm_sstring_chars(&s.str), "pppppppz") == 0);

    CU_ASSERT (ssm_sstring_set_range(&s.str, 8, 0, 's') == SSM_OK);
    CU_ASSERT (ssm_sstring_length(&s.str) == 8);
    CU_ASSERT (ssm_sstring_max_length(&s.str) == 10);
    CU_ASSERT (strcmp(ssm_sstring_chars(&s.str), "pppppppz") == 0);

    CU_ASSERT (ssm_sstring_set_range(&s.str, 9, 0, 's') == SSM_TRUNCATED);
    CU_ASSERT (ssm_sstring_length(&s.str) == 8);
    CU_ASSERT (ssm_sstring_max_length(&s.str) == 10);
    CU_ASSERT (strcmp(ssm_sstring_chars(&s.str), "pppppppz") == 0);

    CU_ASSERT (ssm_sstring_set_range(&s.str, 8, 1, 's') == SSM_TRUNCATED);
    CU_ASSERT (ssm_sstring_length(&s.str) == 8);
    CU_ASSERT (ssm_sstring_max_length(&s.str) == 10);
    CU_ASSERT (strcmp(ssm_sstring_chars(&s.str), "pppppppz") == 0);
}

static void test_copy (void)
{
    ssm_sstring_t* const null = NULL;
    ssm_sstring_declare (s1, 5);
    ssm_sstring_declare (s2, 10);

    CU_ASSERT (ssm_sstring_import(&s1.str, "abcd") == SSM_OK);
    CU_ASSERT (ssm_sstring_length(&s1.str) == 4);
    CU_ASSERT (ssm_sstring_max_length(&s1.str) == 5);
    CU_ASSERT (strcmp(ssm_sstring_chars(&s1.str), "abcd") == 0);

    CU_ASSERT (ssm_sstring_copy(&s1.str, null) == SSM_OK);
    CU_ASSERT (ssm_sstring_length(&s1.str) == 0);
    CU_ASSERT (ssm_sstring_max_length(&s1.str) == 5);
    CU_ASSERT (strcmp(ssm_sstring_chars(&s1.str), "") == 0);

    CU_ASSERT (ssm_sstring_import(&s1.str, "abcd") == SSM_OK);
    CU_ASSERT (ssm_sstring_length(&s1.str) == 4);
    CU_ASSERT (ssm_sstring_max_length(&s1.str) == 5);
    CU_ASSERT (strcmp(ssm_sstring_chars(&s1.str), "abcd") == 0);

    CU_ASSERT (ssm_sstring_import(&s2.str, "ef") == SSM_OK);
    CU_ASSERT (ssm_sstring_length(&s2.str) == 2);
    CU_ASSERT (ssm_sstring_max_length(&s2.str) == 10);
    CU_ASSERT (strcmp(ssm_sstring_chars(&s2.str), "ef") == 0);

    CU_ASSERT (ssm_sstring_copy(&s1.str, &s2.str) == SSM_OK);
    CU_ASSERT (ssm_sstring_length(&s1.str) == 2);
    CU_ASSERT (ssm_sstring_max_length(&s1.str) == 5);
    CU_ASSERT (ssm_sstring_max_length(&s2.str) == 10);
    CU_ASSERT (strcmp(ssm_sstring_chars(&s1.str), "ef") == 0);

    CU_ASSERT (ssm_sstring_import(&s2.str, "ghijklmn") == SSM_OK);
    CU_ASSERT (ssm_sstring_length(&s2.str) == 8);
    CU_ASSERT (ssm_sstring_max_length(&s1.str) == 5);
    CU_ASSERT (ssm_sstring_max_length(&s2.str) == 10);
    CU_ASSERT (strcmp(ssm_sstring_chars(&s2.str), "ghijklmn") == 0);

    CU_ASSERT (ssm_sstring_copy(&s1.str, &s2.str) == SSM_TRUNCATED);
    CU_ASSERT (ssm_sstring_length(&s1.str) == 5);
    CU_ASSERT (ssm_sstring_max_length(&s1.str) == 5);
    CU_ASSERT (ssm_sstring_max_length(&s2.str) == 10);
    CU_ASSERT (strcmp(ssm_sstring_chars(&s1.str), "ghijk") == 0);
}

static void test_copy_overlap (void)
{
    ssm_sstring_declare (s, 50);

    CU_ASSERT (ssm_sstring_length(&s.str) == 0);
    CU_ASSERT (strcmp(ssm_sstring_chars(&s.str), "") == 0);

    CU_ASSERT (ssm_sstring_copy(&s.str, &s.str) == SSM_OK);
    CU_ASSERT (ssm_sstring_length(&s.str) == 0);
    CU_ASSERT (strcmp(ssm_sstring_chars(&s.str), "") == 0);

    CU_ASSERT (ssm_sstring_import(&s.str, "abcd") == SSM_OK);
    CU_ASSERT (ssm_sstring_length(&s.str) == 4);
    CU_ASSERT (strcmp(ssm_sstring_chars(&s.str), "abcd") == 0);

    CU_ASSERT (ssm_sstring_copy(&s.str, &s.str) == SSM_OK);
    CU_ASSERT (ssm_sstring_length(&s.str) == 4);
    CU_ASSERT (strcmp(ssm_sstring_chars(&s.str), "abcd") == 0);
}

static void test_concat (void)
{
    ssm_sstring_t* const null = NULL;
    ssm_sstring_declare (s1, 10);
    ssm_sstring_declare (s2, 10);

    CU_ASSERT (ssm_sstring_import(&s1.str, "abcd") == SSM_OK);
    CU_ASSERT (ssm_sstring_length(&s1.str) == 4);
    CU_ASSERT (ssm_sstring_max_length(&s1.str) == 10);
    CU_ASSERT (strcmp(ssm_sstring_chars(&s1.str), "abcd") == 0);

    CU_ASSERT (ssm_sstring_concat(&s1.str, null) == SSM_OK);
    CU_ASSERT (ssm_sstring_length(&s1.str) == 4);
    CU_ASSERT (ssm_sstring_max_length(&s1.str) == 10);
    CU_ASSERT (strcmp(ssm_sstring_chars(&s1.str), "abcd") == 0);

    CU_ASSERT (ssm_sstring_import(&s2.str, "ef") == SSM_OK);
    CU_ASSERT (ssm_sstring_length(&s2.str) == 2);
    CU_ASSERT (ssm_sstring_max_length(&s2.str) == 10);
    CU_ASSERT (strcmp(ssm_sstring_chars(&s2.str), "ef") == 0);

    CU_ASSERT (ssm_sstring_concat(&s1.str, &s2.str) == SSM_OK);
    CU_ASSERT (ssm_sstring_length(&s1.str) == 6);
    CU_ASSERT (ssm_sstring_max_length(&s1.str) == 10);
    CU_ASSERT (ssm_sstring_max_length(&s2.str) == 10);
    CU_ASSERT (strcmp(ssm_sstring_chars(&s1.str), "abcdef") == 0);

    CU_ASSERT (ssm_sstring_import(&s2.str, "ghijklmn") == SSM_OK);
    CU_ASSERT (ssm_sstring_length(&s2.str) == 8);
    CU_ASSERT (ssm_sstring_max_length(&s1.str) == 10);
    CU_ASSERT (ssm_sstring_max_length(&s2.str) == 10);
    CU_ASSERT (strcmp(ssm_sstring_chars(&s2.str), "ghijklmn") == 0);

    CU_ASSERT (ssm_sstring_concat(&s1.str, &s2.str) == SSM_TRUNCATED);
    CU_ASSERT (ssm_sstring_length(&s1.str) == 10);
    CU_ASSERT (ssm_sstring_max_length(&s1.str) == 10);
    CU_ASSERT (ssm_sstring_max_length(&s2.str) == 10);
    CU_ASSERT (strcmp(ssm_sstring_chars(&s1.str), "abcdefghij") == 0);
}

static void test_concat_overlap (void)
{
    ssm_sstring_declare (s, 50);

    CU_ASSERT (ssm_sstring_length(&s.str) == 0);
    CU_ASSERT (strcmp(ssm_sstring_chars(&s.str), "") == 0);

    CU_ASSERT (ssm_sstring_concat(&s.str, &s.str) == SSM_OK);
    CU_ASSERT (ssm_sstring_length(&s.str) == 0);
    CU_ASSERT (strcmp(ssm_sstring_chars(&s.str), "") == 0);

    CU_ASSERT (ssm_sstring_import(&s.str, "abcd") == SSM_OK);
    CU_ASSERT (ssm_sstring_length(&s.str) == 4);
    CU_ASSERT (strcmp(ssm_sstring_chars(&s.str), "abcd") == 0);

    CU_ASSERT (ssm_sstring_concat(&s.str, &s.str) == SSM_OK);
    CU_ASSERT (ssm_sstring_length(&s.str) == 8);
    CU_ASSERT (strcmp(ssm_sstring_chars(&s.str), "abcdabcd") == 0);
}

static void test_compare (void)
{
    ssm_sstring_declare (b1, 10);
    ssm_sstring_declare (b2, 10);
    ssm_sstring_t* const null = NULL;

    CU_ASSERT (ssm_sstring_compare(&b1.str, null) == SSM_EQUAL);
    CU_ASSERT (ssm_sstring_compare(&b2.str, null) == SSM_EQUAL);
    CU_ASSERT (ssm_sstring_compare(&b1.str, &b2.str) == SSM_EQUAL);

    CU_ASSERT (ssm_sstring_import(&b1.str, "abcd") == SSM_OK);
    CU_ASSERT (ssm_sstring_length(&b1.str) == 4);
    CU_ASSERT (strcmp(ssm_sstring_chars(&b1.str), "abcd") == 0);

    CU_ASSERT (ssm_sstring_compare(&b1.str, null) == SSM_GREATER);
    CU_ASSERT (ssm_sstring_compare(&b1.str, &b2.str) == SSM_GREATER);
    CU_ASSERT (ssm_sstring_compare(&b2.str, &b1.str) == SSM_LOWER);

    CU_ASSERT (ssm_sstring_import(&b2.str, "abedef") == SSM_OK);
    CU_ASSERT (ssm_sstring_length(&b2.str) == 6);
    CU_ASSERT (strcmp(ssm_sstring_chars(&b2.str), "abedef") == 0);

    CU_ASSERT (ssm_sstring_compare(&b1.str, &b2.str) == SSM_LOWER);
    CU_ASSERT (ssm_sstring_compare(&b2.str, &b1.str) == SSM_GREATER);

    CU_ASSERT (ssm_sstring_import(&b2.str, "abcd") == SSM_OK);
    CU_ASSERT (ssm_sstring_length(&b2.str) == 4);
    CU_ASSERT (strcmp(ssm_sstring_chars(&b2.str), "abcd") == 0);

    CU_ASSERT (ssm_sstring_compare(&b1.str, &b2.str) == SSM_EQUAL);
    CU_ASSERT (ssm_sstring_compare(&b2.str, &b1.str) == SSM_EQUAL);

}

static void test_struct (void)
{
    typedef struct {
        int before;
        ssm_sstring_struct(50) str;
        int after;
    } TestStruct;

    TestStruct a = {
        0x1234567,
        ssm_sstring_init(50),
        0x1ABCDEF
    };

    static const char ref[] = "elkfvzelvlevlkzrlbrbnrtnyortgfgernvrjvfnkvze7892Hurt";

    CU_ASSERT (a.before == 0x1234567);
    CU_ASSERT (ssm_sstring_max_length(&a.str.str) == 50);
    CU_ASSERT (ssm_sstring_length(&a.str.str) == 0);
    CU_ASSERT (strcmp(ssm_sstring_chars(&a.str.str), "") == 0);
    CU_ASSERT (a.after == 0x1ABCDEF);

    CU_ASSERT (ssm_sstring_import(&a.str.str, ref) == SSM_TRUNCATED);

    CU_ASSERT (a.before == 0x1234567);
    CU_ASSERT (ssm_sstring_max_length(&a.str.str) == 50);
    CU_ASSERT (ssm_sstring_length(&a.str.str) == 50);
    CU_ASSERT (memcmp(ssm_sstring_chars(&a.str.str), ref, 50) == 0);
    CU_ASSERT (a.after == 0x1ABCDEF);
}

static void test_trashed (void)
{
    ssm_sstring_declare (s1, 10);
    ssm_sstring_declare (s2, 10);

    CU_ASSERT (ssm_sstring_import(&s1.str, "abcd") == SSM_OK);
    CU_ASSERT (ssm_sstring_length(&s1.str) == 4);
    CU_ASSERT (strcmp(ssm_sstring_chars(&s1.str), "abcd") == 0);

    ((char*)(ssm_sstring_chars(&s1.str)))[4] = 'x'; /* trashing null terminator */

    CU_ASSERT (ssm_sstring_import(&s1.str, "xy") == SSM_CORRUPTED);
    CU_ASSERT (ssm_sstring_copy(&s1.str, &s2.str) == SSM_CORRUPTED);
    CU_ASSERT (ssm_sstring_concat(&s1.str, &s2.str) == SSM_CORRUPTED);
    CU_ASSERT (ssm_sstring_compare(&s1.str, &s2.str) == SSM_CORRUPTED);
}

/*
 * Add the tests in this module in a specified test suite.
 */
static void ssm_sstring_test_add_tests(CU_pSuite suite)
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
void ssm_sstring_test_init(void)
{
    const CU_pSuite suite = CU_add_suite ("sstring", init_suite, cleanup_suite);
    ssm_sstring_test_add_tests(suite);
}
#endif /* SSM_USE_CANARY */

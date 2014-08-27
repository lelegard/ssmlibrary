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
 *  CUnit test suite for the platform on which SSM is running.
 *
 *-----------------------------------------------------------------------------
 *
 *  Modification history:
 *    2014-01-10 Thierry Lelegard
 *               Original creation.
 *
 *-----------------------------------------------------------------------------
 */

#include "ssm_platform_test.h"
#include "ssm.h"
#include "utests.h"

/*
 * Test cases
 */
static void test_size_type (void)
{
    /*
     * It is essential for the SSM library that size_t is an unsigned type,
     * as specified by the C language standard. If a non-conformant
     * implementation defines size_t as a signed type, many things won't work.
     */
    CU_ASSERT ((size_t)(-1) > 0);
}

static void test_address_max(void)
{
    const void* a = SSM_ADDRESS_MAX;
    const uint8_t* b = (const uint8_t*)&a;
    int errors = 0;
    size_t i;

    for (i = 0; i < sizeof(a); ++i) {
        errors += b[i] != 0xFF;
    }
    CU_ASSERT (errors == 0);
}

static void test_addr_size(void)
{
    CU_ASSERT ((uintptr_t)ssm_addr_size(NULL, 0) == 0);
    CU_ASSERT ((uintptr_t)ssm_addr_size(NULL, 100) == 100);

    CU_ASSERT (ssm_addr_size((const char*)SSM_ADDRESS_MAX - 100, 0) == (const char*)SSM_ADDRESS_MAX - 100);
    CU_ASSERT (ssm_addr_size((const char*)SSM_ADDRESS_MAX - 100, 99) == (const char*)SSM_ADDRESS_MAX - 1);
    CU_ASSERT (ssm_addr_size((const char*)SSM_ADDRESS_MAX - 100, 100) == SSM_ADDRESS_MAX);
    CU_ASSERT (ssm_addr_size((const char*)SSM_ADDRESS_MAX - 100, 101) == SSM_ADDRESS_MAX);
    CU_ASSERT (ssm_addr_size((const char*)SSM_ADDRESS_MAX - 100, 1000) == SSM_ADDRESS_MAX);

    CU_ASSERT (ssm_addr_size(SSM_ADDRESS_MAX, 0) == SSM_ADDRESS_MAX);
    CU_ASSERT (ssm_addr_size(SSM_ADDRESS_MAX, 1) == SSM_ADDRESS_MAX);
    CU_ASSERT (ssm_addr_size(SSM_ADDRESS_MAX, 1000) == SSM_ADDRESS_MAX);
}

static void test_version(void)
{
    char buf[100];

    /* MSVC considers snprintf() as unsecure, don't care in this test suite. */
    #if defined(_MSC_VER)
    #pragma warning (disable:4996)
    #endif

    snprintf(buf, sizeof(buf), "%d.%d", SSM_MAJOR_VERSION, SSM_MINOR_VERSION);
    buf[sizeof(buf)-1] = '\0';

    CU_ASSERT (100 * SSM_MAJOR_VERSION + SSM_MINOR_VERSION == SSM_VERSION);
    CU_ASSERT (strcmp(SSM_VERSION_STRING, buf) == 0);
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
void ssm_platform_test_init(void)
{
    const CU_pSuite suite = CU_add_suite ("platform", init_suite, cleanup_suite);
    CU_ADD_TEST (suite, test_size_type);
    CU_ADD_TEST (suite, test_address_max);
    CU_ADD_TEST (suite, test_addr_size);
    CU_ADD_TEST (suite, test_version);
}

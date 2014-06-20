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
 * Main program for the unitary tests of the SSM library.
 *
 *-----------------------------------------------------------------------------
 *
 *  Modification history:
 *    2014-01-06 Thierry Lelegard
 *               Original creation.
 *
 *-----------------------------------------------------------------------------
 */

#include "utests.h"
#include "ssm_platform_test.h"
#include "ssm_status_test.h"
#include "ssm_cstring_test.h"
#include "ssm_memory_copy_test.h"
#include "ssm_memory_compare_test.h"
#include "ssm_memory_set_test.h"
#include "ssm_memory_management_test.h"
#include "ssm_memory_management_canary_test.h"
#include "ssm_sstring_test.h"
#include "ssm_sstring_canary_test.h"
#include "ssm_dstring_test.h"
#include "ssm_dstring_canary_test.h"
#include "ssm_sbuffer_test.h"
#include "ssm_sbuffer_canary_test.h"
#include "ssm_dbuffer_test.h"
#include "ssm_dbuffer_canary_test.h"
#include "ssm_c11k_test.h"

int main(int argc, char* argv[])
{
    static const UTestsInitFunction init_functions[] = {
        ssm_platform_test_init,
        ssm_status_test_init,
        ssm_cstring_test_init,
        ssm_memory_copy_test_init,
        ssm_memory_compare_test_init,
        ssm_memory_set_test_init,
        ssm_memory_management_test_init,
        ssm_memory_management_canary_test_init,
        ssm_sstring_test_init,
        ssm_sstring_canary_test_init,
        ssm_dstring_test_init,
        ssm_dstring_canary_test_init,
        ssm_sbuffer_test_init,
        ssm_sbuffer_canary_test_init,
        ssm_dbuffer_test_init,
        ssm_dbuffer_canary_test_init,
        ssm_c11k_test_init,
        NULL,
    };
    return utests_main(argc, argv, "SSM", init_functions);
}

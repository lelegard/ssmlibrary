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
 *  CUnit test suite for ssm_dbuffer_t with canary runtime checks.
 *
 *-----------------------------------------------------------------------------
 *
 *  Modification history:
 *    2014-01-10 Thierry Lelegard
 *               Original creation.
 *
 *-----------------------------------------------------------------------------
 */

#if !defined (_SSM_DBUFFER_CANARY_TEST_H)
#define _SSM_DBUFFER_CANARY_TEST_H 1

/*
 * Initialize the test suite in this module.
 */
void ssm_dbuffer_canary_test_init(void);

#endif /* _SSM_DBUFFER_CANARY_TEST_H */

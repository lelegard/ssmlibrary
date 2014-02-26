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
 */
/**
 *  @file utests.h
 *
 *  Unitary tests support module for CUnit.
 *
 *  This header file provides all required headers from CUnit to write
 *  a unitary test suite. It shall be included by all test suites and
 *  by the main program of the unitary tests.
 */
/*-----------------------------------------------------------------------------
 *
 *  Modification history:
 *    2014-01-06 Thierry Lelegard
 *               Original creation.
 *
 *-----------------------------------------------------------------------------
 */

#if !defined (_UTESTS_H)
#define _UTESTS_H 1

#include <stdio.h>
#include <string.h>
#include <CUnit/CUnit.h>

/**
 * Each test suite is initialized using a void function.
 */
typedef void (*UTestsInitFunction)(void);

/**
 * Main program for unitary tests.
 *
 * The command line arguments @c argc and @c argv are analyzed to setup
 * the unitary tests. The accepted command line arguments are:
 *
 * @li -a : Automated test mode. Produces an XML report. The default XML
 *          output file name is <i>MyProjectName</i>-Results.xml.
 * @li -c : Interactive console mode.
 * @li -d : Debug messages from the various unitary tests are output on
 *          standard error. By default, they are dropped.
 * @li -l : List all tests but do not execute them.
 * @li -n : Normal basic mode (default)
 * @li -o name : Specify an alternate output file prefix with -a instead
 *          of <i>MyProjectName</i>. The suffix -Results.xml will be added.
 * @li -p name[=value] : Define a user-specified parameter.
 * @li -s : Silent basic mode.
 * @li -t name : Run only one test or test suite (use -l for test list).
 * @li -v : Verbose basic mode.
 *
 * @param [in] argc Number of arguments from command line.
 * @param [in] argv Arguments from command line.
 * @param [in] outName Prefix of the output XML file name
 * in automated (-a) mode. The actual file name will be
 * <code><i>outName</i>-Results.xml</code>.
 * @param [in] init_functions An array of pointers to the initialization
 * functions of all tests suites. The array shall be terminated by a
 * NULL pointer.
 * @return @c EXIT_SUCCESS if all tests passed, @c EXIT_FAILURE otherwise.
 * Thus, the result can be used as exit status in the unitary test driver.
 */
int utests_main(int argc, char* argv[], const char* outName, const UTestsInitFunction init_functions[]);

/**
 * Get the value of a user-specified parameter.
 *
 * @param [in] paramName Name of the parameter, as specified in <code>-p name[=value]</code>.
 * @return Address of the parameter value or NULL if not specified.
 */
const char* utest_param_value(const char* paramName);

/**
 * This function returns a file which, just like @c stdout or @c stderr,
 * is used by unitary tests to log messages.
 *
 * A unitary test typically does not display anything. It simply
 * performs assertions. A complete set of unitary test suites
 * reports successes or failures using CUnit.
 *
 * However, there are cases where the unitary test may want to
 * issue trace, log or debug messages. Such messages should be
 * sent to this output file.
 *
 * By default, these messages are discarded. However, when the
 * option -d (debug) is specified on the command line of the
 * unitary test driver, these messages are reported on the
 * standard error stream.
 *
 * @return A @c FiLE* which is used by unitary tests to log messages.
 * This is typically @c stderr in debug mode and a file redirecting
 * to the null device otherwise.
 */
FILE* utests_out (void);

#endif /* _UTESTS_H */

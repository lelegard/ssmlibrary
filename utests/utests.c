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
 *  Unitary tests support module for CUnit.
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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <CUnit/CUnit.h>
#include <CUnit/Automated.h>
#include <CUnit/Basic.h>
#include <CUnit/Console.h>

/*
 * System-specfic null device.
 */
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
#define DEV_NULL "NUL:"
#else
#define DEV_NULL "/dev/null"
#endif


/*-----------------------------------------------------------------------------
 * This function returns a file which is used by unitary tests to log messages.
 *-----------------------------------------------------------------------------
 */

static FILE* utests_out_file = NULL;

FILE* utests_out(void)
{
    return utests_out_file != NULL ? utests_out_file : stderr;
}


/*-----------------------------------------------------------------------------
 * Accessing the user-specified parameters "-p name[=value]"
 *-----------------------------------------------------------------------------
 */

typedef struct utest_param_s {
    const char* name;
    const char* value;
    struct utest_param_s* next;
} utest_param_t;

/*
 * Linked list of utest_param_t structures.
 */
static utest_param_t* utest_params = NULL;

/*
 * Free the list of parameters.
 */
static void utest_params_free(void)
{
    while (utest_params != NULL) {
        utest_param_t* next = utest_params->next;
        free(utest_params);
        utest_params = next;
    }
}

/*
 * Add a parameter definition.
 */
static void utest_add_param(char* definition)
{
    utest_param_t* param = NULL;
    char* value = strchr(definition, '=');

    if (value == NULL) {
        /* Empty value */
        value = "";
    }
    else {
        /* Null-terminate the name and point to value */
        *value++ = '\0';
    }

    /* Allocate a new param structure */
    param = (utest_param_t*)(malloc(sizeof(utest_param_t)));
    if (param == NULL) {
        abort();
    }
    param->name = definition;
    param->value = value;
    param->next = utest_params;
    utest_params = param;
}

/*
 * Getting the value of a user-specified parameter.
 */
const char* utest_param_value(const char* paramName)
{
    /* Browse the list to get the parameter */
    const utest_param_t* current = utest_params;
    while (current != NULL) {
        if (strcmp(paramName, current->name) == 0) {
            /* Parameter found */
            return current->value;
        }
        current = current->next;
    }
    /* Parameter not defined */
    return NULL;
}


/*-----------------------------------------------------------------------------
 * Display the syntax of the command.
 *-----------------------------------------------------------------------------
 */
static int usage(const char* argv0, const char* outName)
{
    fprintf(stderr, "%s: invalid command\n\n", argv0);
    fprintf(stderr, "Syntax: %s [options]\n\n", argv0);
    fprintf(stderr, "The available options are:\n"
            "  -a : Automated test mode, default XML file: %s-Results.xml\n"
            "  -c : Interactive console mode\n"
            "  -l : List all tests but do not execute them\n"
            "  -n : Normal basic mode (default)\n"
            "  -o name : output file prefix with -a, -Results.xml added\n"
            "  -p name[=value] : Define a user-specified parameter\n"
            "  -s : Silent basic mode\n"
            "  -t name : Run only one test or test suite (use -l for test list)\n"
            "  -v : Verbose basic mode\n",
            outName);

    return EXIT_FAILURE;
}


/*-----------------------------------------------------------------------------
 * List all tests
 *-----------------------------------------------------------------------------
 */
static void list_tests(void)
{
    CU_pSuite suite;
    CU_pTest test;
    unsigned int suiteIndex;
    unsigned int testIndex;

    /* Loop on test suites */
    for (suiteIndex = 1; (suite = CU_get_suite_at_pos(suiteIndex)) != NULL; suiteIndex++) {
        printf("%s\n", suite->pName);
        for (testIndex = 1; (test = CU_get_test_at_pos(suite, testIndex)) != NULL; testIndex++) {
            printf("    %s.%s\n", suite->pName, test->pName);
        }
    }
}


/*-----------------------------------------------------------------------------
 * Main program for unitary tests.
 *-----------------------------------------------------------------------------
 */
int utests_main(int argc, char* argv[], const char* outName, const UTestsInitFunction init_functions[])
{
    int runMode = 'n';                     /* CUnit run mode, same as option character */
    int listMode = 0;                      /* Boolean: list tests only */
    int debug = 0;                         /* Boolean: enable debug messages */
    const char* programName = argv[0];     /* Program name */
    const char* suiteName = NULL;          /* Name of test suite */
    const char* testName = NULL;           /* Name of test */
    const char* outputName = outName;      /* Output XML file prefix */
    CU_pSuite suite = NULL;
    CU_pTest test = NULL;
    int success = 1;
    int arg;

    /*
     * Decode the command line.
     */
    for (arg = 1; arg < argc; arg++) {
        const char* opt = argv[arg];
        if (strlen(opt) != 2 || opt[0] != '-') {
            return usage(programName, outName);
        }
        switch (opt[1]) {
        case 'a':
        case 'c':
        case 'n':
        case 's':
        case 'v':
            runMode = opt[1];
            break;
        case 'd':
            debug = 1;
            break;
        case 'l':
            listMode = 1;
            break;
        case 'o':
            if (++arg >= argc) {
                return usage(programName, outName);
            }
            else {
                outputName = argv[arg];
            }
            break;
        case 'p':
            if (++arg >= argc) {
                return usage(programName, outName);
            }
            else {
                utest_add_param(argv[arg]);
            }
            break;
        case 't':
            if (++arg >= argc) {
                return usage(programName, outName);
            }
            else {
                char* dot = strchr(argv[arg], '.');
                suiteName = argv[arg];
                if (dot != NULL) {
                    testName = dot + 1;
                    *dot = '\0';
                }
            }
            break;
        default:
            return usage(programName, outName);
        }
    }

    /*
     * Tell CUnit to abort execution in case of error. Note that such errors are CUnit internal errors,
     * not test case failures. So, when such a CUnit internal error appears, there is no real reason to
     * continue and we may abort the test.
     */
    CU_set_error_action(CUEA_ABORT);

    /*
     * Initialize the CUnit registry with all test suites.
     */
    CU_initialize_registry();
    CU_set_output_filename(outputName);

    /*
     * Initialize all test suites in the project.
     */
    while (*init_functions != NULL) {
        (*init_functions++)();
    }

    /*
     * In list mode, only print the list of tests.
     */
    if (listMode) {
        list_tests();
        return EXIT_SUCCESS;
    }

    /*
     * In non debug mode, redirect debug messages to the null device
     */
    if (!debug) {
        /*
         * MSVC considers fopen() as unsecure, which is excessive, especially when portability is required.
         * warning C4996: 'fopen': This function or variable may be unsafe. Consider using fopen_s instead. [...]
         */
        #if defined(_MSC_VER)
        #pragma warning (push)
        #pragma warning (disable:4996)
        #endif
        utests_out_file = fopen(DEV_NULL, "w");
        #if defined(_MSC_VER)
        #pragma warning (pop)
        #endif

        if (utests_out_file == NULL) {
            fprintf(stderr, "%s: error opening %s\n", programName, DEV_NULL);
            return EXIT_FAILURE;
        }
    }

    /*
     * Set the basic mode
     */
    switch (runMode) {
    case 'n':
        CU_basic_set_mode(CU_BRM_NORMAL);
        break;
    case 's':
        CU_basic_set_mode(CU_BRM_SILENT);
        break;
    case 'v':
        CU_basic_set_mode(CU_BRM_VERBOSE);
        break;
    default:
        break;
    }

    /*
     * Run the test suites in the selected mode.
     */
    switch (runMode) {
    case 'a':
        CU_automated_run_tests();
        break;
    case 'c':
        CU_console_run_tests();
        break;
    case 'n':
    case 's':
    case 'v':
        if (suiteName == NULL) {
            /* Run all test suites */
            CU_basic_run_tests();
        }
        else if ((suite = CU_get_suite(suiteName)) == NULL) {
            /* Specified test suite not found */
            fprintf(stderr, "%s: test suite %s not found\n", programName, suiteName);
            success = 0;
        }
        else if (testName == NULL) {
            /* Run all tests in selected test suite */
            CU_basic_run_suite(suite);
        }
        else if ((test = CU_get_test(suite, testName)) == NULL) {
            /* Specified test not found in test suite */
            fprintf(stderr, "%s: test %s not found in %s\n", programName, testName, suiteName);
            success = 0;
        }
        else {
            /* Run one selected test in selected test suite */
            CU_basic_run_test(suite, test);
        }
        break;
    default:
        fprintf(stderr, "%s: internal error, unexpected run mode\n", programName);
        success = 0;
        break;
    }

    /*
     * Get tests global success/failure and cleanup CUnit
     */
    success = success && CU_get_number_of_suites_failed() == 0 && CU_get_number_of_tests_failed() == 0;
    CU_cleanup_registry();

    /*
     * Cleanup resources
     */
    utest_params_free();
    if (utests_out_file != NULL) {
        fclose(utests_out_file);
        utests_out_file = NULL;
    }

    /*
     * Exits with a success status if all tests passed.
     */
    return success ? EXIT_SUCCESS : EXIT_FAILURE;
}

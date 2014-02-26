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
 * Benchmark application for the SSM library.
 *
 *-----------------------------------------------------------------------------
 *
 *  Modification history:
 *    2014-01-25 Thierry Lelegard
 *               Original creation.
 *
 *-----------------------------------------------------------------------------
 */

#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64) || defined(__MINGW32__) || defined(__CYGWIN__)
#define WINDOWS 1
#endif

#include "ssm.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#if defined(WINDOWS)
#include <windows.h>
#else
#include <sys/time.h>
#include <sys/resource.h>
#endif


/*-----------------------------------------------------------------------------
 * Display the syntax of the command.
 *-----------------------------------------------------------------------------
 */
static void usage(const char* argv0)
{
    fprintf(stderr, "Syntax: %s [-r repeat-count] [-s data-size]\n", argv0);
    exit(EXIT_FAILURE);
}


/*-----------------------------------------------------------------------------
 * Get accumulated process user-mode CPU time in micro-seconds.
 *-----------------------------------------------------------------------------
 */
static uint64_t user_cpu_time(void)
{
#if defined(WINDOWS)
    union {
        FILETIME ft;
        uint64_t i;
    } creationTime, exitTime, kernelTime, userTime;
    if (GetProcessTimes(GetCurrentProcess(), &creationTime.ft, &exitTime.ft, &kernelTime.ft, &userTime.ft) == 0) {
        fprintf(stderr, "GetProcessTimes: error 0x%08X\n", (int)GetLastError());
        exit(EXIT_FAILURE);
    }
    /* FILETIME is a 64-bit value representing the number of 100-nanosecond intervals since January 1, 1601. */
    return userTime.i / 10;
#else
    struct rusage us;
    if (getrusage(RUSAGE_SELF, &us) < 0) {
        perror("getrusage");
        exit(EXIT_FAILURE);
    }
    return (uint64_t)us.ru_utime.tv_sec * 1000000 + (uint64_t)us.ru_utime.tv_usec;
#endif
}


/*-----------------------------------------------------------------------------
 * Report CPU time usage.
 *-----------------------------------------------------------------------------
 */
static void report_cpu_time(uint64_t since, int repeat, const char* title)
{
    const uint64_t now = user_cpu_time();
    printf("%s: %d micro-seconds / operation\n", title, (int)((now - since) / repeat));
}


/*-----------------------------------------------------------------------------
 * Memory allocation with filled value and abort on error.
 *-----------------------------------------------------------------------------
 */
static void* allocate(size_t dataSize, uint8_t value)
{
    void* mem = malloc(dataSize);
    if (mem == NULL) {
        fprintf(stderr, "Insufficient virtual memory\n");
        exit(EXIT_FAILURE);
    }
    memset(mem, value, dataSize);
    return mem;
}


/*-----------------------------------------------------------------------------
 * C-string length benchmarck.
 *-----------------------------------------------------------------------------
 */
static void bench_cstring_length(int repeat, size_t dataSize)
{
    uint64_t start;
    char* str;
    int i;

    str = (char*)allocate(dataSize, 'A');
    str[dataSize - 1] = '\0';
    start = user_cpu_time();
    for (i = repeat; i > 0; --i) {
        ssm_cstring_length(str, SSM_SIZE_MAX);
    }
    report_cpu_time(start, repeat, "ssm_cstring_length");
    free(str);
}


/*-----------------------------------------------------------------------------
 * Memory copy benchmarck.
 *-----------------------------------------------------------------------------
 */
static void bench_copy(int repeat, size_t dataSize)
{
    uint64_t start;
    void* mem1;
    void* mem2;
    int i;

    mem1 = allocate(dataSize, 1);
    mem2 = allocate(dataSize, 2);
    start = user_cpu_time();
    for (i = repeat; i > 0; --i) {
        ssm_copy(mem1, dataSize, mem2, dataSize, NULL);
    }
    report_cpu_time(start, repeat, "ssm_copy");
    free(mem1);
    free(mem2);
}


/*-----------------------------------------------------------------------------
 * Memory copy benchmarck.
 *-----------------------------------------------------------------------------
 */
static void bench_compare(int repeat, size_t dataSize)
{
    uint64_t start;
    void* mem1;
    void* mem2;
    int i;

    mem1 = allocate(dataSize, 1);
    mem2 = allocate(dataSize, 1);
    start = user_cpu_time();
    for (i = repeat; i > 0; --i) {
        ssm_compare(mem1, dataSize, mem2, dataSize);
    }
    report_cpu_time(start, repeat, "ssm_compare");
    free(mem1);
    free(mem2);
}


/*-----------------------------------------------------------------------------
 * Memory set benchmarck.
 *-----------------------------------------------------------------------------
 */
static void bench_set(int repeat, size_t dataSize)
{
    uint64_t start;
    void* mem;
    int i;

    mem = allocate(dataSize, 1);
    start = user_cpu_time();
    for (i = repeat; i > 0; --i) {
        ssm_set(mem, dataSize, 2);
    }
    report_cpu_time(start, repeat, "ssm_set");
    free(mem);
}


/*-----------------------------------------------------------------------------
 * Main application.
 *-----------------------------------------------------------------------------
 */
int main(int argc, char* argv[])
{
    int repeat = 1000;        /* Number of times each operation is repeated */
    size_t dataSize = 65536;  /* Reference data size in bytes */
    int arg;

    /*
     * Decode the command line.
     */
    for (arg = 1; arg < argc; arg++) {
        const char* opt = argv[arg];
        if (strlen(opt) != 2 || opt[0] != '-') {
            usage(argv[0]);
        }
        switch (opt[1]) {
        case 'r':
            if (++arg >= argc || (repeat = atoi(argv[arg])) <= 0) {
                usage(argv[0]);
            }
            break;
        case 's':
            if (++arg >= argc || (dataSize = atoi(argv[arg])) <= 0) {
                usage(argv[0]);
            }
            break;
        default:
            usage(argv[0]);
        }
    }

    printf("SSM Library benchmarks\n");
    printf("Each operation is repeated %d times\n", repeat);
    printf("Reference data size: %d bytes\n", (int)dataSize);

    bench_cstring_length(repeat, dataSize);
    bench_copy(repeat, dataSize);
    bench_compare(repeat, dataSize);
    bench_set(repeat, dataSize);

    return EXIT_SUCCESS;
}

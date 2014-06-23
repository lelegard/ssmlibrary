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
 * Demo Linux kernel module using the SSM library.
 *
 *-----------------------------------------------------------------------------
 *
 *  Modification history:
 *    2014-01-08 Thierry Lelegard
 *               Original creation.
 *
 *-----------------------------------------------------------------------------
 */

#include "ssm.h"
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/slab.h>

/*
 * Kernel logging.
 */
#define PRINT(format, args...) printk (KERN_INFO "ssmdemo: " format, ## args)

/*
 * For dynamic strings and buffers, we need to specify our allocation functions.
 * In this demo, we simply allocate kernel memory without locking or I/O constraint
 * but beware of what you are doing in real production modules.
 */
static void* ssmdemo_malloc(size_t size)
{
    return kmalloc(size, GFP_KERNEL);
}
static void ssmdemo_free(void* ptr)
{
    kfree(ptr);
}

/*
 * Static strings demo.
 */
static void test_static_strings(void)
{
    ssm_sstring_declare(s1, 9);
    ssm_sstring_declare(s2, 9);

    ssm_sstring_import(&s1.str, "foobar");
    PRINT("sstring 1: size = %d, value = \"%s\"\n", (int)ssm_sstring_length(&s1.str), ssm_sstring_chars(&s1.str));

    ssm_sstring_import(&s1.str, "string too large");
    PRINT("sstring 2: size = %d, value = \"%s\"\n", (int)ssm_sstring_length(&s1.str), ssm_sstring_chars(&s1.str));

    ssm_sstring_import(&s1.str, "foo");
    ssm_sstring_import(&s2.str, "bar");
    PRINT("sstring 4: size = %d, value = \"%s\"\n", (int)ssm_sstring_length(&s1.str), ssm_sstring_chars(&s1.str));
    PRINT("sstring 5: size = %d, value = \"%s\"\n", (int)ssm_sstring_length(&s2.str), ssm_sstring_chars(&s2.str));

    ssm_sstring_concat(&s1.str, &s2.str);
    PRINT("sstring 6: size = %d, value = \"%s\"\n", (int)ssm_sstring_length(&s1.str), ssm_sstring_chars(&s1.str));
}

/*
 * Dynamic strings demo.
 */
static void test_dynamic_strings(void)
{
    ssm_dstring_declare(s1);
    ssm_dstring_declare(s2);

    ssm_dstring_import(&s1, "foobar");
    PRINT("dstring 1: size = %d, value = \"%s\"\n", (int)ssm_dstring_length(&s1), ssm_dstring_chars(&s1));

    ssm_dstring_import(&s1, "a much larger string");
    PRINT("dstring 2: size = %d, value = \"%s\"\n", (int)ssm_dstring_length(&s1), ssm_dstring_chars(&s1));

    ssm_dstring_import(&s1, "foo");
    ssm_dstring_import(&s2, "bar");
    PRINT("dstring 4: size = %d, value = \"%s\"\n", (int)ssm_dstring_length(&s1), ssm_dstring_chars(&s1));
    PRINT("dstring 5: size = %d, value = \"%s\"\n", (int)ssm_dstring_length(&s2), ssm_dstring_chars(&s2));

    ssm_dstring_concat(&s1, &s2);
    PRINT("dstring 6: size = %d, value = \"%s\"\n", (int)ssm_dstring_length(&s1), ssm_dstring_chars(&s1));

    ssm_dstring_free(&s2);
    ssm_dstring_free(&s1);
}

/*
 * Static buffers demo.
 */
static void test_static_buffers(void)
{
    ssm_sbuffer_declare(b1, 9);
    ssm_sbuffer_declare(b2, 9);

    ssm_sbuffer_import(&b1.buf, "foobar", 6);
    PRINT("sbuffer 1: size = %d, value = \"%.*s\"\n",
          (int)ssm_sbuffer_length(&b1.buf),
          (int)ssm_sbuffer_length(&b1.buf),
          (const char*)ssm_sbuffer_data(&b1.buf));

    ssm_sbuffer_import(&b1.buf, "foo", 3);
    ssm_sbuffer_import(&b2.buf, "bar", 3);
    PRINT("sbuffer 2: size = %d, value = \"%.*s\"\n",
          (int)ssm_sbuffer_length(&b1.buf),
          (int)ssm_sbuffer_length(&b1.buf),
          (const char*)ssm_sbuffer_data(&b1.buf));
    PRINT("sbuffer 3: size = %d, value = \"%.*s\"\n",
          (int)ssm_sbuffer_length(&b2.buf),
          (int)ssm_sbuffer_length(&b2.buf),
          (const char*)ssm_sbuffer_data(&b2.buf));

    ssm_sbuffer_concat(&b1.buf, &b2.buf);
    PRINT("sbuffer 4: size = %d, value = \"%.*s\"\n",
          (int)ssm_sbuffer_length(&b1.buf),
          (int)ssm_sbuffer_length(&b1.buf),
          (const char*)ssm_sbuffer_data(&b1.buf));
}

/*
 * Dynamic buffers demo.
 */
static void test_dynamic_buffers(void)
{
    ssm_dbuffer_declare(b1);
    ssm_dbuffer_declare(b2);

    ssm_dbuffer_import(&b1, "foobar", 6);
    PRINT("dbuffer 1: size = %d, value = \"%.*s\"\n",
          (int)ssm_dbuffer_length(&b1),
          (int)ssm_dbuffer_length(&b1),
          (const char*)ssm_dbuffer_data(&b1));

    ssm_dbuffer_import(&b1, "foo", 3);
    ssm_dbuffer_import(&b2, "bar", 3);
    PRINT("dbuffer 2: size = %d, value = \"%.*s\"\n",
          (int)ssm_dbuffer_length(&b1),
          (int)ssm_dbuffer_length(&b1),
          (const char*)ssm_dbuffer_data(&b1));
    PRINT("dbuffer 3: size = %d, value = \"%.*s\"\n",
          (int)ssm_dbuffer_length(&b2),
          (int)ssm_dbuffer_length(&b2),
          (const char*)ssm_dbuffer_data(&b2));

    ssm_dbuffer_concat(&b1, &b2);
    PRINT("dbuffer 4: size = %d, value = \"%.*s\"\n",
          (int)ssm_dbuffer_length(&b1),
          (int)ssm_dbuffer_length(&b1),
          (const char*)ssm_dbuffer_data(&b1));

    ssm_dbuffer_free(&b2);
    ssm_dbuffer_free(&b1);
}

/*
 * Loadable kernel module initialization routine.
 */
static int __init ssmdemo_init(void)
{
    PRINT("init module\n");

    /* Establish our own dynamic memory management in the kernel. */
    /* This is required for dynamic strings and buffers. */
    ssm_set_memory_management(ssmdemo_malloc, ssmdemo_free);

    /* Run the demo as part of the initialization routine. */
    test_static_strings();
    test_dynamic_strings();
    test_static_buffers();
    test_dynamic_buffers();

    return 0;
}

/*
 * Loadable kernel module cleanup routine (upon rmmod).
 */
static void __exit ssmdemo_exit(void)
{
    PRINT("exit module\n");
}

/*
 * Loadable kernel module description.
 */
module_init(ssmdemo_init);
module_exit(ssmdemo_exit);
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Demo using the SSM library in a kernel module");

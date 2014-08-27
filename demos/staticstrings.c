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
 * Demo application for SSM static strings.
 *
 *-----------------------------------------------------------------------------
 *
 *  Modification history:
 *    2014-01-06 Thierry Lelegard
 *               Original creation.
 *
 *-----------------------------------------------------------------------------
 */

#include "ssm.h"
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char* argv[])
{
    ssm_sstring_declare(s1, 9);
    ssm_sstring_declare(s2, 9);

    ssm_sstring_import(&s1.str, "foobar");
    printf("size = %d, value = \"%s\"\n", (int)ssm_sstring_length(&s1.str), ssm_sstring_chars(&s1.str));

    ssm_sstring_import(&s1.str, "string too large");
    printf("size = %d, value = \"%s\"\n", (int)ssm_sstring_length(&s1.str), ssm_sstring_chars(&s1.str));

    ssm_sstring_import(&s1.str, "foo");
    ssm_sstring_import(&s2.str, "bar");
    printf("size = %d, value = \"%s\"\n", (int)ssm_sstring_length(&s1.str), ssm_sstring_chars(&s1.str));
    printf("size = %d, value = \"%s\"\n", (int)ssm_sstring_length(&s2.str), ssm_sstring_chars(&s2.str));

    ssm_sstring_concat(&s1.str, &s2.str);
    printf("size = %d, value = \"%s\"\n", (int)ssm_sstring_length(&s1.str), ssm_sstring_chars(&s1.str));

    return EXIT_SUCCESS;
}

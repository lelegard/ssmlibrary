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
 * Demo application for SSM dynamic strings.
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
    ssm_dstring_declare(s1);
    ssm_dstring_declare(s2);

    ssm_dstring_import(&s1, "foobar");
    printf("size = %d, value = \"%s\"\n", (int)ssm_dstring_length(&s1), ssm_dstring_chars(&s1));

    ssm_dstring_import(&s1, "a much larger string");
    printf("size = %d, value = \"%s\"\n", (int)ssm_dstring_length(&s1), ssm_dstring_chars(&s1));

    ssm_dstring_import(&s1, "foo");
    ssm_dstring_import(&s2, "bar");
    printf("size = %d, value = \"%s\"\n", (int)ssm_dstring_length(&s1), ssm_dstring_chars(&s1));
    printf("size = %d, value = \"%s\"\n", (int)ssm_dstring_length(&s2), ssm_dstring_chars(&s2));

    ssm_dstring_concat(&s1, &s2);
    printf("size = %d, value = \"%s\"\n", (int)ssm_dstring_length(&s1), ssm_dstring_chars(&s1));

    ssm_dstring_free(&s2);
    ssm_dstring_free(&s1);

    return EXIT_SUCCESS;
}

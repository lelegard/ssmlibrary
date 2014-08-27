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
 * Demo application for SSM static memory buffers.
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
    ssm_sbuffer_declare(b1, 9);
    ssm_sbuffer_declare(b2, 9);

    ssm_sbuffer_import(&b1.buf, "foobar", 6);
    printf("size = %d, value = \"%.*s\"\n",
           (int)ssm_sbuffer_length(&b1.buf),
           (int)ssm_sbuffer_length(&b1.buf),
           (const char*)ssm_sbuffer_data(&b1.buf));

    ssm_sbuffer_import(&b1.buf, "foo", 3);
    ssm_sbuffer_import(&b2.buf, "bar", 3);
    printf("size = %d, value = \"%.*s\"\n",
           (int)ssm_sbuffer_length(&b1.buf),
           (int)ssm_sbuffer_length(&b1.buf),
           (const char*)ssm_sbuffer_data(&b1.buf));
    printf("size = %d, value = \"%.*s\"\n",
           (int)ssm_sbuffer_length(&b2.buf),
           (int)ssm_sbuffer_length(&b2.buf),
           (const char*)ssm_sbuffer_data(&b2.buf));

    ssm_sbuffer_concat(&b1.buf, &b2.buf);
    printf("size = %d, value = \"%.*s\"\n",
           (int)ssm_sbuffer_length(&b1.buf),
           (int)ssm_sbuffer_length(&b1.buf),
           (const char*)ssm_sbuffer_data(&b1.buf));

    return EXIT_SUCCESS;
}

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
 * Demo application for SSM dynamic memory buffers.
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
    ssm_dbuffer_declare(b1);
    ssm_dbuffer_declare(b2);

    ssm_dbuffer_import(&b1, "foobar", 6);
    printf("size = %d, value = \"%.*s\"\n",
           (int)ssm_dbuffer_length(&b1),
           (int)ssm_dbuffer_length(&b1),
           (const char*)ssm_dbuffer_data(&b1));

    ssm_dbuffer_import(&b1, "foo", 3);
    ssm_dbuffer_import(&b2, "bar", 3);
    printf("size = %d, value = \"%.*s\"\n",
           (int)ssm_dbuffer_length(&b1),
           (int)ssm_dbuffer_length(&b1),
           (const char*)ssm_dbuffer_data(&b1));
    printf("size = %d, value = \"%.*s\"\n",
           (int)ssm_dbuffer_length(&b2),
           (int)ssm_dbuffer_length(&b2),
           (const char*)ssm_dbuffer_data(&b2));

    ssm_dbuffer_concat(&b1, &b2);
    printf("size = %d, value = \"%.*s\"\n",
           (int)ssm_dbuffer_length(&b1),
           (int)ssm_dbuffer_length(&b1),
           (const char*)ssm_dbuffer_data(&b1));

    ssm_dbuffer_free(&b2);
    ssm_dbuffer_free(&b1);

    return EXIT_SUCCESS;
}

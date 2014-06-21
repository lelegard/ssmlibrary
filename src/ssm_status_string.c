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
 * Implementation of function ssm_status_string
 *
 *-----------------------------------------------------------------------------
 *
 *  Modification history:
 *    2014-06-20 Thierry Lelegard
 *               Original creation.
 *
 *-----------------------------------------------------------------------------
 */

#include "ssm_internal.h"

const char* ssm_status_string(ssm_status_t status)
{
    switch (status) {
    case SSM_OK:           return "The function executed successfully";
    case SSM_TRUNCATED:    return "The result is truncated but safe";
    case SSM_EQUAL:        return "Objects are equal after comparison";
    case SSM_LOWER:        return "Object 1 is lower than object 2 after comparison";
    case SSM_GREATER:      return "Object 1 is greater than object 2 after comparison";
    case SSM_NULLOUT:      return "A NULL pointer was provided as output parameter";
    case SSM_SIZETOOLARGE: return "Some size is larger than SSM_SIZE_MAX";
    case SSM_INDEXRANGE:   return "An index parameter in out of range";
    case SSM_SIZEZERO:     return "Some size is zero";
    case SSM_NOMEMORY:     return "Memory allocation failure, result is unchanged";
    case SSM_CORRUPTED:    return "Memory was previously corrupted, result is undefined but safe";
    case SSM_BUG:          return "Internal inconsistency, there is a bug in the SSM library";
    default:               return "Unknown";
    }
}
